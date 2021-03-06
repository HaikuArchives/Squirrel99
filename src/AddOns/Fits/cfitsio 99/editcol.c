/*  This file, editcol.c, contains the set of FITSIO routines that    */
/*  insert or delete rows or columns in a table or resize an image    */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <string.h>
#include <stdlib.h>
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
int ffrsim(fitsfile *fptr,      /* I - FITS file pointer           */
           int bitpix,          /* I - bits per pixel              */
           int naxis,           /* I - number of axes in the array */
           long *naxes,         /* I - size of each axis           */
           int *status)         /* IO - error status               */
/*
   resize an existing primary array or IMAGE extension.
*/
{
    int ii, simple, obitpix, onaxis, extend, nmodify;
    long onaxes[99], pcount, gcount, newsize, oldsize, nblocks, longval;
    char comment[25], keyname[FLEN_KEYWORD], message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    /* get current image size parameters */
    if (ffghpr(fptr, 99, &simple, &obitpix, &onaxis, onaxes, &pcount,
               &gcount, &extend, status) > 0)
        return(*status);

    /* test that the new values are legal */

    if (bitpix != BYTE_IMG && bitpix != SHORT_IMG && 
        bitpix != LONG_IMG &&
        bitpix != FLOAT_IMG && bitpix != DOUBLE_IMG)
    {
        sprintf(message,
        "Illegal value for BITPIX keyword: %d", bitpix);
        ffpmsg(message);
        return(*status = BAD_BITPIX);
    }

    if (naxis < 0 || naxis > 999)
    {
        sprintf(message,
        "Illegal value for NAXIS keyword: %d", naxis);
        ffpmsg(message);
        return(*status = BAD_NAXIS);
    }

    if (naxis == 0)
        newsize = 0;
    else
        newsize = 1;

    for (ii = 0; ii < naxis; ii++)
    {
        if (naxes[ii] < 0)
        {
            sprintf(message,
            "Illegal value for NAXIS%d keyword: %ld", ii + 1,  naxes[ii]);
            ffpmsg(message);
            return(*status = BAD_NAXES);
        }

        newsize *= naxes[ii];  /* compute new image size, in pixels */
    }

    /* compute size of old image, in bytes */

    if (onaxis == 0)
        oldsize = 0;
    else
    {
        oldsize = 1;
        for (ii = 0; ii < onaxis; ii++)
            oldsize *= onaxes[ii];  
        oldsize = (oldsize + pcount) * gcount * abs(obitpix) / 8;
    }

    oldsize = (oldsize + 2879) / 2880; /* old size, in blocks */

    newsize = (newsize + pcount) * gcount * abs(bitpix) / 8;
    newsize = (newsize + 2879) / 2880; /* new size, in blocks */

    if (newsize > oldsize)   /* have to insert new blocks for image */
    {
        nblocks = newsize - oldsize;
        if (ffiblk(fptr, nblocks, 1, status) > 0)  
            return(*status);
    }
    else if (oldsize > newsize)  /* have to delete blocks from image */
    {
        nblocks = oldsize - newsize;
        if (ffdblk(fptr, nblocks, status) > 0)  
            return(*status);
    }

    /* now update the header keywords */

    strcpy(comment,"&");  /* special value to leave comments unchanged */

    if (bitpix != obitpix)
    {                         /* update BITPIX value */
        longval = bitpix;
        ffmkyj(fptr, "BITPIX", longval, comment, status);
    }

    if (naxis != onaxis)
    {                        /* update NAXIS value */
        longval = naxis;
        ffmkyj(fptr, "NAXIS", longval, comment, status);
    }

    /* modify the existing NAXISn keywords */
    nmodify = minvalue(naxis, onaxis); 
    for (ii = 0; ii < nmodify; ii++)
    {
        ffkeyn("NAXIS", ii+1, keyname, status);
        ffmkyj(fptr, keyname, naxes[ii], comment, status);
    }

    if (naxis > onaxis)  /* insert additional NAXISn keywords */
    {
        strcpy(comment,"length of data axis");  
        for (ii = onaxis; ii < naxis; ii++)
        {
            ffkeyn("NAXIS", ii+1, keyname, status);
            ffikyj(fptr, keyname, naxes[ii], comment, status);
        }
    }
    else if (onaxis > naxis) /* delete old NAXISn keywords */
    {
        for (ii = naxis; ii < onaxis; ii++)
        {
            ffkeyn("NAXIS", ii+1, keyname, status);
            ffdkey(fptr, keyname, status);
        }
    }

    /* re-read the header, to make sure structures are updated */
    ffrdef(fptr, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffirow(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstrow,   /* I - insert space AFTER this row              */
                            /*     0 = insert space at beginning of table   */
           long nrows,      /* I - number of rows to insert                 */
           int *status)     /* IO - error status                            */
/*
 insert NROWS blank rows immediated after row firstrow (1 = first row).
 Set firstrow = 0 to insert space at the beginning of the table.
*/
{
    int tstatus;
    long naxis1, naxis2;
    long datasize, freespace, firstbyte, nshift, nblock, nbytes;

    if (*status > 0)
        return(*status);

        /* reset position to the correct HDU if necessary */
    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
    {
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);
    }
         /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype == IMAGE_HDU)
    {
        ffpmsg("Can only add rows to TABLE or BINTABLE extension (ffirow)");
        return(*status = NOT_TABLE);
    }

    if (nrows < 0 )
        return(*status = NEG_BYTES);
    else if (nrows == 0)
        return(*status);   /* no op, so just return */

    /* get the current size of the table */
    /* use internal structure since NAXIS2 keyword may not be up to date */
    naxis1 = (fptr->Fptr)->rowlength;
    naxis2 = (fptr->Fptr)->numrows;

    if (firstrow > naxis2)
    {
        ffpmsg(
   "Insert position greater than the number of rows in the table (ffirow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow < 0)
    {
        ffpmsg("Insert position is less than 0 (ffirow)");
        return(*status = BAD_ROW_NUM);
    }

    datasize = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize; /* current data size */
    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nshift = naxis1 * nrows;              /* no. of bytes to add to table */

    if ( (freespace - nshift) < 0)   /* not enough existing space? */
    {
        nblock = (nshift - freespace + 2879) / 2880;   /* number of blocks */
        ffiblk(fptr, nblock, 1, status);               /* insert the blocks */
    }

    firstbyte = naxis1 * firstrow;    /* relative insert position */
    nbytes = datasize - firstbyte;    /* no. of bytes to shift down */
    firstbyte += ((fptr->Fptr)->datastart);   /* absolute insert position */

    ffshft(fptr, firstbyte, nbytes, nshift, status); /* shift rows and heap */

    /* update the heap starting address */
    (fptr->Fptr)->heapstart += nshift;

    /* update the THEAP keyword if it exists */
    tstatus = 0;
    ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);

    /* update the NAXIS2 keyword */
    ffmkyj(fptr, "NAXIS2", naxis2 + nrows, "&", status);
    ((fptr->Fptr)->numrows) += nrows;
    ((fptr->Fptr)->origrows) += nrows;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdrow(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstrow,   /* I - first row to delete (1 = first)          */
           long nrows,      /* I - number of rows to delete                 */
           int *status)     /* IO - error status                            */
/*
 delete NROWS rows from table starting with firstrow (1 = first row of table).
*/
{
    int tstatus;
    long naxis1, naxis2;
    long datasize, freespace, firstbyte, nshift, nblock, nbytes;
    char comm[FLEN_COMMENT];

    if (*status > 0)
        return(*status);

    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
    {
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);
    }
        /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype == IMAGE_HDU)
    {
        ffpmsg("Can only delete rows in TABLE or BINTABLE extension (ffdrow)");
        return(*status = NOT_TABLE);
    }

    if (nrows < 0 )
        return(*status = NEG_BYTES);
    else if (nrows == 0)
        return(*status);   /* no op, so just return */

    ffgkyj(fptr, "NAXIS1", &naxis1, comm, status); /* get the current   */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* size of the table */

    if (firstrow > naxis2)
    {
        ffpmsg(
   "Delete position greater than the number of rows in the table (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow < 1)
    {
        ffpmsg("Delete position is less than 1 (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow + nrows - 1 > naxis2)
    {
        ffpmsg("No. of rows to delete exceeds size of table (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }

    nshift = naxis1 * nrows;           /* no. of bytes to delete from table */
    datasize = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize;    /* current size of data */
    firstbyte = naxis1 * (firstrow + nrows - 1); /* relative delete position */
    nbytes = datasize - firstbyte;    /* no. of bytes to shift up */
    firstbyte += ((fptr->Fptr)->datastart);   /* absolute delete position */
    ffshft(fptr, firstbyte, nbytes, (-1) * nshift, status); /* shift data */

    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nblock = (nshift + freespace) / 2880;   /* number of blocks */

    if (nblock > 0) 
        ffdblk(fptr, nblock, status);  /* delete integral number blocks */

    /* update the heap starting address */
    (fptr->Fptr)->heapstart -= nshift;

    /* update the THEAP keyword if it exists */
    tstatus = 0;
    ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);

    /* update the NAXIS2 keyword */
    ffmkyj(fptr, "NAXIS2", naxis2 - nrows, "&", status);
    ((fptr->Fptr)->numrows) -= nrows;
    ((fptr->Fptr)->origrows) -= nrows;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdrws(fitsfile *fptr,  /* I - FITS file pointer                        */
           long *rownum,    /* I - list of rows to delete (1 = first)       */
           long nrows,      /* I - number of rows to delete                 */
           int *status)     /* IO - error status                            */
/*
 delete the list of rows from the table (1 = first row of table).
*/
{
    long naxis1, naxis2, ii, insertpos, nextrow, nextrowpos;
    char comm[FLEN_COMMENT];
    unsigned char *buffer;

    if (*status > 0)
        return(*status);

    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);

    /* rescan header if data structure is undefined */
    if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype == IMAGE_HDU)
    {
        ffpmsg("Can only delete rows in TABLE or BINTABLE extension (ffdrws)");
        return(*status = NOT_TABLE);
    }

    if (nrows < 0 )
        return(*status = NEG_BYTES);
    else if (nrows == 0)
        return(*status);   /* no op, so just return */

    ffgkyj(fptr, "NAXIS1", &naxis1, comm, status); /* row width   */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* number of rows */

    /* check that input row list is in ascending order */
    for (ii = 1; ii < nrows; ii++)
    {
        if (rownum[ii - 1] >= rownum[ii])
        {
            ffpmsg("row numbers are not in increasing order (ffdrws)");
            return(*status = BAD_ROW_NUM);
        }
    }

    if (rownum[0] < 1)
    {
        ffpmsg("first row to delete is less than 1 (ffdrws)");
        return(*status = BAD_ROW_NUM);
    }
    else if (rownum[nrows - 1] > naxis2)
    {
        ffpmsg("last row to delete exceeds size of table (ffdrws)");
        return(*status = BAD_ROW_NUM);
    }

    buffer = (unsigned char *) malloc(naxis1);  /* buffer for one row */

    if (!buffer)
    {
        ffpmsg("malloc failed (ffdrws)");
        return(*status = MEMORY_ALLOCATION);
    }

    /* byte location to start of first row to delete, and the next row */
    insertpos = (fptr->Fptr)->datastart + ((rownum[0] - 1) * naxis1);
    nextrowpos = insertpos + naxis1;
    nextrow = rownum[0] + 1;

    /* work through the list of rows to delete */
    for (ii = 1; ii < nrows; nextrow++, nextrowpos += naxis1)
    {
        if (nextrow < rownum[ii])  
        {   /* keep this row, so copy it to the new position */

            ffmbyt(fptr, nextrowpos, REPORT_EOF, status);
            ffgbyt(fptr, naxis1, buffer, status);  /* read the bytes */

            ffmbyt(fptr, insertpos, IGNORE_EOF, status);
            ffpbyt(fptr, naxis1, buffer, status);  /* write the bytes */

            if (*status > 0)
            {
                ffpmsg("error while copying good rows in table (ffdrws)");
                free(buffer);
                return(*status);
            }
            insertpos += naxis1;
        }
        else
        {   /* skip over this row since it is in the list */
            ii++;
        }
    }

    /* finished with all the rows to delete; copy remaining rows */
    while(nextrow <= naxis2)
    {
        ffmbyt(fptr, nextrowpos, REPORT_EOF, status);
        ffgbyt(fptr, naxis1, buffer, status);  /* read the bytes */

        ffmbyt(fptr, insertpos, IGNORE_EOF, status);
        ffpbyt(fptr, naxis1, buffer, status);  /* write the bytes */

        if (*status > 0)
        {
            ffpmsg("failed to copy remaining rows in table (ffdrws)");
            free(buffer);
            return(*status);
        }
        insertpos  += naxis1;
        nextrowpos += naxis1;
        nextrow++; 
    }
    free(buffer);
    
    /* now delete the empty rows at the end of the table */
    ffdrow(fptr, naxis2 - nrows + 1, nrows, status);
    
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fficol(fitsfile *fptr,  /* I - FITS file pointer                        */
           int numcol,      /* I - position for new col. (1 = 1st)          */
           char *ttype,     /* I - name of column (TTYPE keyword)           */
           char *tform,     /* I - format of column (TFORM keyword)         */
           int *status)     /* IO - error status                            */
/*
 Insert a new column into an existing table at position numcol.  If
 numcol is greater than the number of existing columns in the table
 then the new column will be appended as the last column in the table.
*/
{
    char *name, *format;

    name = ttype;
    format = tform;

    fficls(fptr, numcol, 1, &name, &format, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fficls(fitsfile *fptr,  /* I - FITS file pointer                        */
           int fstcol,      /* I - position for first new col. (1 = 1st)    */
           int ncols,       /* I - number of columns to insert              */
           char **ttype,    /* I - array of column names(TTYPE keywords)    */
           char **tform,    /* I - array of formats of column (TFORM)       */
           int *status)     /* IO - error status                            */
/*
 Insert 1 or more new columns into an existing table at position numcol.  If
 fstcol is greater than the number of existing columns in the table
 then the new column will be appended as the last column in the table.
*/
{
    int colnum, datacode, decims, tfields, tstatus, ii;
    long width, delbyte, repeat, naxis1, naxis2, datasize, freespace, nadd;
    long nblock, firstbyte, nbytes, tbcol;
    char tfm[FLEN_VALUE], keyname[FLEN_KEYWORD], comm[FLEN_COMMENT], *cptr;
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
    {
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);
    }
        /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype == IMAGE_HDU)
    {
       ffpmsg("Can only add columns to TABLE or BINTABLE extension (fficol)");
       return(*status = NOT_TABLE);
    }

    /*  is the column number valid?  */
    tfields = (fptr->Fptr)->tfield;
    if (fstcol < 1 )
        return(*status = BAD_COL_NUM);
    else if (fstcol > tfields)
        colnum = tfields + 1;   /* append as last column */
    else
        colnum = fstcol;

    /* parse the tform value and calc number of bytes to add to each row */
    delbyte = 0;
    for (ii = 0; ii < ncols; ii++)
    {
        strcpy(tfm, tform[ii]);
        ffupch(tfm);         /* make sure format is in upper case */

        if ((fptr->Fptr)->hdutype == ASCII_TBL)
        {
            ffasfm(tfm, &datacode, &width, &decims, status);
            delbyte += width + 1;  /*  add one space between the columns */
        }
        else
        {
            ffbnfm(tfm, &datacode, &repeat, &width, status);

            if (datacode < 0)         /* variable length array column */
                delbyte += 8;
            else if (datacode == 1)          /* bit column; round up  */
                delbyte += (repeat + 7) / 8; /* to multiple of 8 bits */
            else if (datacode == 16)  /* ASCII string column */
                delbyte += repeat;
            else                      /* numerical data type */
                delbyte += (datacode / 10) * repeat;
        }
    }

    if (*status > 0) 
        return(*status);

    naxis1 = (fptr->Fptr)->rowlength;          /* current width of the table */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* number of rows */

    /* current size of data */
    datasize = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize;
    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nadd = delbyte * naxis2;                /* no. of bytes to add to table */

    if ( (freespace - nadd) < 0)   /* not enough existing space? */
    {
        nblock = (nadd - freespace + 2879) / 2880;     /* number of blocks  */
        if (ffiblk(fptr, nblock, 1, status) > 0)       /* insert the blocks */
            return(*status);
    }

    /* shift heap down (if it exists) */
    if ((fptr->Fptr)->heapsize > 0)
    {
        nbytes = (fptr->Fptr)->heapsize;    /* no. of bytes to shift down */

        /* absolute heap pos */
        firstbyte = (fptr->Fptr)->datastart + (fptr->Fptr)->heapstart;

        if (ffshft(fptr, firstbyte, nbytes, nadd, status) > 0) /* move heap */
            return(*status);

        /* update the heap starting address */
        (fptr->Fptr)->heapstart += nadd;

        /* update the THEAP keyword if it exists */
        tstatus = 0;
        ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);
    }

    /* calculate byte position in the row where to insert the new column */
    if (colnum > tfields)
        firstbyte = naxis1;
    else
    {
        colptr = (fptr->Fptr)->tableptr;
        colptr += (colnum - 1);
        firstbyte = colptr->tbcol;
    }

    /* insert delbyte bytes in every row, at byte position firstbyte */
    ffcins(fptr, naxis1, naxis2, delbyte, firstbyte, status);

    if ((fptr->Fptr)->hdutype == ASCII_TBL)
    {
        /* adjust the TBCOL values of the existing columns */
        for(ii = 0; ii < tfields; ii++)
        {
            ffkeyn("TBCOL", ii + 1, keyname, status);
            ffgkyj(fptr, keyname, &tbcol, comm, status);
            if (tbcol > firstbyte)
            {
                tbcol += delbyte;
                ffmkyj(fptr, keyname, tbcol, "&", status);
            }
        }
    }

    /* update the mandatory keywords */
    ffmkyj(fptr, "TFIELDS", tfields + ncols, "&", status);
    ffmkyj(fptr, "NAXIS1", naxis1 + delbyte, "&", status);

    /* increment the index value on any existing column keywords */
    if(colnum <= tfields)
        ffkshf(fptr, colnum, tfields, ncols, status);

    /* add the required keywords for the new columns */
    for (ii = 0; ii < ncols; ii++, colnum++)
    {
        strcpy(comm, "label for field");
        ffkeyn("TTYPE", colnum, keyname, status);
        ffpkys(fptr, keyname, ttype[ii], comm, status);

        strcpy(comm, "format of field");
        strcpy(tfm, tform[ii]);
        ffupch(tfm);         /* make sure format is in upper case */
        ffkeyn("TFORM", colnum, keyname, status);

        if (datacode == TUSHORT) 
        {
           /* Replace the 'U' with an 'I' in the TFORMn code */
           cptr = tfm;
           while (*cptr != 'U') 
              cptr++;

           *cptr = 'I';
           ffpkys(fptr, keyname, tfm, comm, status);

           /* write the TZEROn and TSCALn keywords */
           ffkeyn("TZERO", colnum, keyname, status);
           strcpy(comm, "offset for unsigned integers");

           ffpkyg(fptr, keyname, 32768., 0, comm, status);

           ffkeyn("TSCAL", colnum, keyname, status);
           strcpy(comm, "data are not scaled");
           ffpkyg(fptr, keyname, 1., 0, comm, status);
        }
        else if (datacode == TULONG) 
        {
           /* Replace the 'V' with an 'J' in the TFORMn code */
           cptr = tfm;
           while (*cptr != 'V') 
              cptr++;

           *cptr = 'J';
           ffpkys(fptr, keyname, tfm, comm, status);

           /* write the TZEROn and TSCALn keywords */
           ffkeyn("TZERO", colnum, keyname, status);
           strcpy(comm, "offset for unsigned integers");

           ffpkyg(fptr, keyname, 2147483648., 0, comm, status);

           ffkeyn("TSCAL", colnum, keyname, status);
           strcpy(comm, "data are not scaled");
           ffpkyg(fptr, keyname, 1., 0, comm, status);
        }
        else
        {
           ffpkys(fptr, keyname, tfm, comm, status);
        }

        if ((fptr->Fptr)->hdutype == ASCII_TBL)   /* write the TBCOL keyword */
        {
            if (colnum == tfields + 1)
                tbcol = firstbyte + 2;  /* allow space between preceding col */
            else
                tbcol = firstbyte + 1;

            strcpy(comm, "beginning column of field");
            ffkeyn("TBCOL", colnum, keyname, status);
            ffpkyj(fptr, keyname, tbcol, comm, status);

            /* increment the column starting position for the next column */
            ffasfm(tfm, &datacode, &width, &decims, status);
            firstbyte += width + 1;  /*  add one space between the columns */
        }
    }

    ffrdef(fptr, status); /* initialize the new table structure */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffmvec(fitsfile *fptr,  /* I - FITS file pointer                        */
           int colnum,      /* I - position of col to be modified           */
           long newveclen,  /* I - new vector length of column (TFORM)       */
           int *status)     /* IO - error status                            */
/*
  Modify the vector length of a column in a binary table, larger or smaller.
  E.g., change a column from TFORMn = '1E' to '20E'.
*/
{
    int datacode, tfields, tstatus;
    long width, delbyte, repeat, naxis1, naxis2, datasize, freespace, nadd;
    long nblock, firstbyte, nbytes, size, ndelete;
    char tfm[FLEN_VALUE], keyname[FLEN_KEYWORD], tcode[2];
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
    {
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);
    }
        /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype != BINARY_TBL)
    {
       ffpmsg(
  "Can only change vector length of a column in BINTABLE extension (ffmvec)");
       return(*status = NOT_TABLE);
    }

    /*  is the column number valid?  */
    tfields = (fptr->Fptr)->tfield;
    if (colnum < 1 || colnum > tfields)
        return(*status = BAD_COL_NUM);

    /* look up the current vector length and element width */

    colptr = (fptr->Fptr)->tableptr;
    colptr += (colnum - 1);

    datacode = colptr->tdatatype; /* datatype of the column */
    repeat = colptr->trepeat;  /* field repeat count  */
    width =  colptr->twidth;   /*  width of a single element in chars */

    if (datacode < 0)
    {
        ffpmsg(
        "Can't modify vector length of variable length column (ffmvec)");
        return(*status = BAD_TFORM);
    }

    if (repeat == newveclen)
        return(*status);  /* column already has the desired vector length */

    if (datacode == TSTRING)
        width = 1;      /* width was equal to width of unit string */

    naxis1 = (fptr->Fptr)->rowlength;          /* current width of the table */
    ffgkyj(fptr, "NAXIS2", &naxis2, NULL, status); /* number of rows */

    delbyte = (newveclen - repeat) * width;    /* no. of bytes to insert */
    if (datacode == TBIT)  /* BIT column is a special case */
       delbyte = ((newveclen + 1) / 8) - ((repeat + 1) / 8);

    if (delbyte > 0)  /* insert space for more elements */
    {
      /* current size of data */
      datasize = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize;
      freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;

      nadd = delbyte * naxis2;             /* no. of bytes to add to table */

      if ( (freespace - nadd) < 0)   /* not enough existing space? */
      {
        nblock = (nadd - freespace + 2879) / 2880;    /* number of blocks  */
        if (ffiblk(fptr, nblock, 1, status) > 0)      /* insert the blocks */
          return(*status);
      }

      /* shift heap down (if it exists) */
      if ((fptr->Fptr)->heapsize > 0)
      {
        nbytes = (fptr->Fptr)->heapsize;    /* no. of bytes to shift down */

        /* absolute heap pos */
        firstbyte = (fptr->Fptr)->datastart + (fptr->Fptr)->heapstart;

        if (ffshft(fptr, firstbyte, nbytes, nadd, status) > 0) /* move heap */
            return(*status);

        /* update the heap starting address */
        (fptr->Fptr)->heapstart += nadd;

        /* update the THEAP keyword if it exists */
        tstatus = 0;
        ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);
      }

      firstbyte = colptr->tbcol + (repeat * width);  /* insert position */

      /* insert delbyte bytes in every row, at byte position firstbyte */
      ffcins(fptr, naxis1, naxis2, delbyte, firstbyte, status);
    }
    else if (delbyte < 0)
    {
      /* current size of table */
      size = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize;
      freespace = ((size + 2879) / 2880) * 2880 - size - (delbyte * naxis2);
      nblock = freespace / 2880;   /* number of empty blocks to delete */
      firstbyte = colptr->tbcol + (repeat * width);  /* delete position */

      /* delete elements from the vector */
      ffcdel(fptr, naxis1, naxis2, -delbyte, firstbyte, status);
 
      /* shift heap up (if it exists) */
      if ((fptr->Fptr)->heapsize > 0)
      {
        nbytes = (fptr->Fptr)->heapsize;    /* no. of bytes to shift up */
        /* abs heap pos */
        firstbyte = (fptr->Fptr)->datastart + (fptr->Fptr)->heapstart;
        ndelete = delbyte * naxis2; /* size of shift (negative) */

        if (ffshft(fptr, firstbyte, nbytes, ndelete, status) > 0)
          return(*status);

        /* update the heap starting address */
        (fptr->Fptr)->heapstart += ndelete;  /* ndelete is negative */

        /* update the THEAP keyword if it exists */
        tstatus = 0;
        ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);
      }

      /* delete the empty  blocks at the end of the HDU */
      if (nblock > 0)
        ffdblk(fptr, nblock, status);
    }

    /* construct the new TFORM keyword for the column */
    if (datacode == TBIT)
      strcpy(tcode,"X");
    else if (datacode == TBYTE)
      strcpy(tcode,"B");
    else if (datacode == TLOGICAL)
      strcpy(tcode,"L");
    else if (datacode == TSTRING)
      strcpy(tcode,"A");
    else if (datacode == TSHORT)
      strcpy(tcode,"I");
    else if (datacode == TLONG)
      strcpy(tcode,"J");
    else if (datacode == TFLOAT)
      strcpy(tcode,"E");
    else if (datacode == TDOUBLE)
      strcpy(tcode,"D");
    else if (datacode == TCOMPLEX)
      strcpy(tcode,"C");
    else if (datacode == TDBLCOMPLEX)
      strcpy(tcode,"M");

    sprintf(tfm,"%ld%s",newveclen,tcode);      /* TFORM value */
    ffkeyn("TFORM", colnum, keyname, status);  /* Keyword name */
    ffmkys(fptr, keyname, tfm, "&", status);   /* modify TFORM keyword */

    ffmkyj(fptr, "NAXIS1", naxis1 + delbyte, "&", status); /* modify NAXIS1 */

    ffrdef(fptr, status); /* reinitialize the new table structure */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcpcl(fitsfile *infptr,    /* I - FITS file pointer to input file  */
           fitsfile *outfptr,   /* I - FITS file pointer to output file */
           int incol,           /* I - number of input column   */
           int outcol,          /* I - number for output column  */
           int create_col,      /* I - create new col if TRUE, else overwrite */
           int *status)         /* IO - error status     */
/*
  copy a column from infptr and insert it in the outfptr table.
*/
{
    int tstatus, colnum, typecode, anynull;
    long tfields, repeat, width, nrows, outrows;
    long inloop, outloop, maxloop, ndone, ntodo, npixels;
    long firstrow, firstelem, ii;
    char keyname[FLEN_KEYWORD], ttype[FLEN_VALUE], tform[FLEN_VALUE];
    char *lvalues = 0, nullflag, **strarray = 0;
    char nulstr[] = {'\5', '\0'};  /* unique null string value */
    double dnull = 0.l, *dvalues = 0;
    float fnull = 0., *fvalues = 0;

    if (*status > 0)
        return(*status);

    if (infptr->HDUposition != (infptr->Fptr)->curhdu)
    {
        ffmahd(infptr, (infptr->HDUposition) + 1, NULL, status);
    }
    else if ((infptr->Fptr)->datastart == DATA_UNDEFINED)
        ffrdef(infptr, status);                /* rescan header */

    if (outfptr->HDUposition != (outfptr->Fptr)->curhdu)
    {
        ffmahd(outfptr, (outfptr->HDUposition) + 1, NULL, status);
    }
    else if ((outfptr->Fptr)->datastart == DATA_UNDEFINED)
        ffrdef(outfptr, status);               /* rescan header */

    if (*status > 0)
        return(*status);

    if ((infptr->Fptr)->hdutype == IMAGE_HDU || (outfptr->Fptr)->hdutype == IMAGE_HDU)
    {
       ffpmsg
       ("Can not copy columns to or from IMAGE HDUs (ffcpcl)");
       return(*status = NOT_TABLE);
    }

    if ( (infptr->Fptr)->hdutype == BINARY_TBL &&  (outfptr->Fptr)->hdutype == ASCII_TBL)
    {
       ffpmsg
       ("Copying from Binary table to ASCII table is not supported (ffcpcl)");
       return(*status = NOT_BTABLE);
    }

    /* get the datatype and vector repeat length of the column */
    ffgtcl(infptr, incol, &typecode, &repeat, &width, status);

    if (typecode < 0)
    {
        ffpmsg("Variable-length columns are not supported (ffcpcl)");
        return(*status = BAD_TFORM);
    }

    if (create_col)    /* insert new column in output table? */
    {
        tstatus = 0;
        ffkeyn("TTYPE", incol, keyname, &tstatus);
        ffgkys(infptr, keyname, ttype, 0, &tstatus);
        ffkeyn("TFORM", incol, keyname, &tstatus);
    
        if (ffgkys(infptr, keyname, tform, 0, &tstatus) )
        {
          ffpmsg
          ("Could not find TTYPE and TFORM keywords in input table (ffcpcl)");
          return(*status = NO_TFORM);
        }

        if ((infptr->Fptr)->hdutype == ASCII_TBL && (outfptr->Fptr)->hdutype == BINARY_TBL)
        {
            /* convert from ASCII table to BINARY table format string */
            if (typecode == TSTRING)
                ffnkey(width, "A", tform, status);

            else if (typecode == TLONG)
                strcpy(tform, "1J");

            else if (typecode == TSHORT)
                strcpy(tform, "1I");

            else if (typecode == TFLOAT)
                strcpy(tform,"1E");

            else if (typecode == TDOUBLE)
                strcpy(tform,"1D");
        }

        if (ffgkyj(outfptr, "TFIELDS", &tfields, 0, &tstatus))
        {
           ffpmsg
           ("Could not read TFIELDS keyword in output table (ffcpcl)");
           return(*status = NO_TFIELDS);
        }

        colnum = minvalue((int) tfields + 1, outcol); /* output col. number */

        /* create the empty column */
        if (fficol(outfptr, colnum, ttype, tform, status) > 0)
        {
           ffpmsg
           ("Could not append new column to output file (ffcpcl)");
           return(*status);
        }

        /* copy other column-related keywords if they exist */

        ffcpky(infptr, outfptr, incol, colnum, "TUNIT", status);
        ffcpky(infptr, outfptr, incol, colnum, "TSCAL", status);
        ffcpky(infptr, outfptr, incol, colnum, "TZERO", status);
        ffcpky(infptr, outfptr, incol, colnum, "TDISP", status);
        ffcpky(infptr, outfptr, incol, colnum, "TLMIN", status);
        ffcpky(infptr, outfptr, incol, colnum, "TLMAX", status);
        ffcpky(infptr, outfptr, incol, colnum, "TDIM", status);

        if ((infptr->Fptr)->hdutype == ASCII_TBL && (outfptr->Fptr)->hdutype == BINARY_TBL)
        {
            /* binary tables only have TNULLn keyword for integer columns */
            if (typecode == TLONG || typecode == TSHORT)
            {
                /* check if null string is defined; replace with integer */
                ffkeyn("TNULL", incol, keyname, &tstatus);
                if (ffgkys(infptr, keyname, ttype, 0, &tstatus) <= 0)
                {
                   ffkeyn("TNULL", colnum, keyname, &tstatus);
                   if (typecode == TLONG)
                      ffpkyj(outfptr, keyname, -9999999L, "Null value", status);
                   else
                      ffpkyj(outfptr, keyname, -32768L, "Null value", status);
                }
            }
        }
        else
        {
            ffcpky(infptr, outfptr, incol, colnum, "TNULL", status);
        }

        /* rescan header to recognize the new keywords */
        if (ffrdef(outfptr, status) )
            return(*status);
    }
    else
    {
        colnum = outcol;
    }

    ffgkyj(infptr,  "NAXIS2", &nrows,   0, status);  /* no. of input rows */
    ffgkyj(outfptr, "NAXIS2", &outrows, 0, status);  /* no. of output rows */
    nrows = minvalue(nrows, outrows);

    if (typecode == TBIT)
        repeat = (repeat - 1) / 8 + 1;  /* convert from bits to bytes */
    else if (typecode == TSTRING && (infptr->Fptr)->hdutype == BINARY_TBL)
        repeat = repeat / width;  /* convert from chars to unit strings */

    /* get optimum number of rows to copy at one time */
    ffgrsz(infptr,  &inloop,  status);
    ffgrsz(outfptr, &outloop, status);

    /* adjust optimum number, since 2 tables are open at once */
    maxloop = minvalue(inloop, outloop); /* smallest of the 2 tables */
    maxloop = maxvalue(1, maxloop / 2);  /* at least 1 row */
    maxloop = minvalue(maxloop, nrows);  /* max = nrows to be copied */
    maxloop *= repeat;                   /* mult by no of elements in a row */

    /* allocate memory for arrays */
    if (typecode == TLOGICAL)
    {
       lvalues   = (char *) calloc(maxloop, sizeof(char) );
       if (!lvalues)
       {
         ffpmsg
         ("malloc failed to get memory for logicals (ffcpcl)");
         return(*status = ARRAY_TOO_BIG);
       }
    }
    else if (typecode == TSTRING)
    {
       /* allocate array of pointers */
       strarray = (char **) calloc(maxloop, sizeof(strarray));

       /* allocate space for each string */
       for (ii = 0; ii < maxloop; ii++)
          strarray[ii] = (char *) calloc(width+1, sizeof(char));
    }
    else if (typecode == TCOMPLEX)
    {
       fvalues = (float *) calloc(maxloop * 2, sizeof(float) );
       if (!fvalues)
       {
         ffpmsg
         ("malloc failed to get memory for complex (ffcpcl)");
         return(*status = ARRAY_TOO_BIG);
       }
       fnull = 0.;
    }
    else if (typecode == TDBLCOMPLEX)
    {
       dvalues = (double *) calloc(maxloop * 2, sizeof(double) );
       if (!dvalues)
       {
         ffpmsg
         ("malloc failed to get memory for dbl complex (ffcpcl)");
         return(*status = ARRAY_TOO_BIG);
       }
       dnull = 0.;
    }
    else    /* numerical datatype; read them all as doubles */
    {
       dvalues = (double *) calloc(maxloop, sizeof(double) );
       if (!dvalues)
       {
         ffpmsg
         ("malloc failed to get memory for doubles (ffcpcl)");
         return(*status = ARRAY_TOO_BIG);
       }
         dnull = -9.99991999E31;  /* use an unlikely value for nulls */
    }

    npixels = nrows * repeat;          /* total no. of pixels to copy */
    ntodo = minvalue(npixels, maxloop);   /* no. to copy per iteration */
    ndone = 0;             /* total no. of pixels that have been copied */

    while (ntodo)      /* iterate through the table */
    {
        firstrow = ndone / repeat + 1;
        firstelem = ndone - ((firstrow - 1) * repeat) + 1;

        /* read from input table */
        if (typecode == TLOGICAL)
            ffgcl(infptr, incol, firstrow, firstelem, ntodo, 
                       lvalues, status);
        else if (typecode == TSTRING)
            ffgcvs(infptr, incol, firstrow, firstelem, ntodo,
                       nulstr, strarray, &anynull, status);

        else if (typecode == TCOMPLEX)  
            ffgcvc(infptr, incol, firstrow, firstelem, ntodo, fnull, 
                   fvalues, &anynull, status);

        else if (typecode == TDBLCOMPLEX)
            ffgcvm(infptr, incol, firstrow, firstelem, ntodo, dnull, 
                   dvalues, &anynull, status);

        else       /* all numerical types */
            ffgcvd(infptr, incol, firstrow, firstelem, ntodo, dnull, 
                   dvalues, &anynull, status);

        if (*status > 0)
        {
            ffpmsg("Error reading input copy of column (ffcpcl)");
            break;
        }

        /* write to output table */
        if (typecode == TLOGICAL)
        {
            nullflag = 2;
            ffpcnl(outfptr, colnum, firstrow, firstelem, ntodo, 
                       lvalues, nullflag, status);
        }

        else if (typecode == TSTRING)
        {
            if (anynull)
                ffpcns(outfptr, colnum, firstrow, firstelem, ntodo,
                       strarray, nulstr, status);
            else
                ffpcls(outfptr, colnum, firstrow, firstelem, ntodo,
                       strarray, status);
        }

        else if (typecode == TCOMPLEX)  
        {                      /* doesn't support writing nulls */
            ffpclc(outfptr, colnum, firstrow, firstelem, ntodo, 
                       fvalues, status);
        }

        else if (typecode == TDBLCOMPLEX)  
        {                      /* doesn't support writing nulls */
            ffpclm(outfptr, colnum, firstrow, firstelem, ntodo, 
                       dvalues, status);
        }

        else  /* all other numerical types */
        {
            if (anynull)
                ffpcnd(outfptr, colnum, firstrow, firstelem, ntodo, 
                       dvalues, dnull, status);
            else
                ffpcld(outfptr, colnum, firstrow, firstelem, ntodo, 
                       dvalues, status);
        }

        if (*status > 0)
        {
            ffpmsg("Error writing output copy of column (ffcpcl)");
            break;
        }

        npixels -= ntodo;
        ndone += ntodo;
        ntodo = minvalue(npixels, maxloop);
    }

    /* free the previously allocated memory */
    if (typecode == TLOGICAL)
    {
        free(lvalues);
    }
    else if (typecode == TSTRING)
    {
         for (ii = 0; ii < maxloop; ii++)
             free(strarray[ii]);

         free(strarray);
    }
    else
    {
        free(dvalues);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcpky(fitsfile *infptr,    /* I - FITS file pointer to input file  */
           fitsfile *outfptr,   /* I - FITS file pointer to output file */
           int incol,           /* I - input index number   */
           int outcol,          /* I - output index number  */
           char *rootname,      /* I - root name of the keyword to be copied */
           int *status)         /* IO - error status     */
/*
  copy an indexed keyword from infptr to outfptr.
*/
{
    int tstatus = 0;
    char keyname[FLEN_KEYWORD];
    char value[FLEN_VALUE], comment[FLEN_COMMENT], card[FLEN_CARD];

    ffkeyn(rootname, incol, keyname, &tstatus);
    if (ffgkey(infptr, keyname, value, comment, &tstatus) <= 0)
    {
        ffkeyn(rootname, outcol, keyname, &tstatus);
        ffmkky(keyname, value, comment, card, status);
        ffprec(outfptr, card, status);
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdcol(fitsfile *fptr,  /* I - FITS file pointer                        */
           int colnum,      /* I - column to delete (1 = 1st)               */
           int *status)     /* IO - error status                            */
/*
  Delete a column from a table.
*/
{
    int ii, tstatus;
    long firstbyte, delbyte, nspace, naxis1, naxis2;
    long size, freespace, nblock, ndelete, nbytes, tbcol;
    char keyname[FLEN_KEYWORD], comm[FLEN_COMMENT];
    tcolumn *colptr, *nextcol;

    if (*status > 0)
        return(*status);

    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
    {
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);
    }
    /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    if ((fptr->Fptr)->hdutype == IMAGE_HDU)
    {
       ffpmsg
       ("Can only delete column from TABLE or BINTABLE extension (ffdcol)");
       return(*status = NOT_TABLE);
    }

    if (colnum < 1 || colnum > (fptr->Fptr)->tfield )
        return(*status = BAD_COL_NUM);

    colptr = (fptr->Fptr)->tableptr;
    colptr += (colnum - 1);
    firstbyte = colptr->tbcol;  /* starting byte position of the column */

    /* use column width to determine how many bytes to delete in each row */
    if ((fptr->Fptr)->hdutype == ASCII_TBL)
    {
      delbyte = colptr->twidth;  /* width of ASCII column */

      if (colnum < (fptr->Fptr)->tfield) /* check for space between next column */
      {
        nextcol = colptr + 1;
        nspace = (nextcol->tbcol) - (colptr->tbcol) - delbyte;
        if (nspace > 0)
            delbyte++;
      }
      else if (colnum > 1)   /* check for space between last 2 columns */
      {
        nextcol = colptr - 1;
        nspace = (colptr->tbcol) - (nextcol->tbcol) - (nextcol->twidth);
        if (nspace > 0)
        {
           delbyte++;
           firstbyte--;  /* delete the leading space */
        }
      }
    }
    else   /* a binary table */
    {
      if (colnum < (fptr->Fptr)->tfield)
      {
         nextcol = colptr + 1;
         delbyte = (nextcol->tbcol) - (colptr->tbcol);
      }
      else
      {
         delbyte = ((fptr->Fptr)->rowlength) - (colptr->tbcol);
      }
    }

    naxis1 = (fptr->Fptr)->rowlength;          /* current width of the table */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* number of rows */

    /* current size of table */
    size = (fptr->Fptr)->heapstart + (fptr->Fptr)->heapsize;
    freespace = (delbyte * naxis2) + ((size + 2879) / 2880) * 2880 - size;
    nblock = freespace / 2880;   /* number of empty blocks to delete */

    ffcdel(fptr, naxis1, naxis2, delbyte, firstbyte, status); /* delete col */

    /* shift heap up (if it exists) */
    if ((fptr->Fptr)->heapsize > 0)
    {
      nbytes = (fptr->Fptr)->heapsize;    /* no. of bytes to shift up */

      /* absolute heap pos */
      firstbyte = (fptr->Fptr)->datastart + (fptr->Fptr)->heapstart;
      ndelete = delbyte * naxis2; /* size of shift */

      if (ffshft(fptr, firstbyte, nbytes, -ndelete, status) > 0) /* mv heap */
          return(*status);

      /* update the heap starting address */
      (fptr->Fptr)->heapstart -= ndelete;

      /* update the THEAP keyword if it exists */
      tstatus = 0;
      ffmkyj(fptr, "THEAP", (fptr->Fptr)->heapstart, "&", &tstatus);
    }

    /* delete the empty  blocks at the end of the HDU */
    if (nblock > 0)
        ffdblk(fptr, nblock, status);

    if ((fptr->Fptr)->hdutype == ASCII_TBL)
    {
      /* adjust the TBCOL values of the remaining columns */
      for (ii = 1; ii <= (fptr->Fptr)->tfield; ii++)
      {
        ffkeyn("TBCOL", ii, keyname, status);
        ffgkyj(fptr, keyname, &tbcol, comm, status);
        if (tbcol > firstbyte)
        {
          tbcol = tbcol - delbyte;
          ffmkyj(fptr, keyname, tbcol, "&", status);
        }
      }
    }

    /* update the mandatory keywords */
    ffmkyj(fptr, "TFIELDS", ((fptr->Fptr)->tfield) - 1, "&", status);        
    ffmkyj(fptr,  "NAXIS1",   naxis1 - delbyte, "&", status);

    /*
      delete the index keywords starting with 'T' associated with the 
      deleted column and subtract 1 from index of all higher keywords
    */
    ffkshf(fptr, colnum, (fptr->Fptr)->tfield, -1, status);

    ffrdef(fptr, status);  /* initialize the new table structure */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcins(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis1,     /* I - width of the table, in bytes             */
           long naxis2,     /* I - number of rows in the table              */
           long ninsert,    /* I - number of bytes to insert in each row    */
           long bytepos,    /* I - rel. position in row to insert bytes     */
           int *status)     /* IO - error status                            */
/*
 Insert 'ninsert' bytes into each row of the table at position 'bytepos'.
*/
{
    unsigned char buffer[10000], cfill;
    long newlen, fbyte, nbytes, ii, irow, nseg;

    if (*status > 0)
        return(*status);

    if (naxis2 == 0)
        return(*status);  /* just return if there are 0 rows in the table */

    /* select appropriate fill value */
    if ((fptr->Fptr)->hdutype == ASCII_TBL)
        cfill = 32;                     /* ASCII tables use blank fill */
    else
        cfill = 0;    /* primary array and binary tables use zero fill */

    newlen = naxis1 + ninsert;

    if (newlen <= 10000)
    {
       /*******************************************************************
       CASE #1: optimal case where whole new row fits in the work buffer
       *******************************************************************/

        for (ii = 0; ii < ninsert; ii++)
            buffer[ii] = cfill;      /* initialize buffer with fill value */

        /* first move the trailing bytes (if any) in the last row */
        fbyte = bytepos + 1;
        nbytes = naxis1 - bytepos;
        ffgtbb(fptr, naxis2, fbyte, nbytes, &buffer[ninsert], status);
        (fptr->Fptr)->rowlength = newlen;  /* set row length to its new value */

        /* write the row (with leading fill bytes) in the new place */
        nbytes += ninsert;
        ffptbb(fptr, naxis2, fbyte, nbytes, buffer, status);
        (fptr->Fptr)->rowlength = naxis1;  /* reset row length to orig. value */

        /*  now move the rest of the rows */
        for (irow = naxis2 - 1; irow > 0; irow--)
        {
            /* read the row to be shifted (work backwards thru the table) */
            ffgtbb(fptr, irow, fbyte, naxis1, &buffer[ninsert], status);
            (fptr->Fptr)->rowlength = newlen; /* set row length to new value */

            /* write the row (with the leading fill bytes) in the new place */
            ffptbb(fptr, irow, fbyte, newlen, buffer, status);
            (fptr->Fptr)->rowlength = naxis1; /* reset row len to orig value */
        }
    }
    else
    {
        /*****************************************************************
        CASE #2:  whole row doesn't fit in work buffer; move row in pieces
        ******************************************************************
        first copy the data, then go back and write fill into the new column
        start by copying the trailing bytes (if any) in the last row.     */

        nbytes = naxis1 - bytepos;
        nseg = (nbytes + 9999) / 10000;
        fbyte = (nseg - 1) * 10000 + bytepos + 1;
        nbytes = naxis1 - fbyte + 1;

        for (ii = 0; ii < nseg; ii++)
        {
            ffgtbb(fptr, naxis2, fbyte, nbytes, buffer, status);
            (fptr->Fptr)->rowlength = newlen;  /* set row length to new value */

            ffptbb(fptr, naxis2, fbyte + ninsert, nbytes, buffer, status);
            (fptr->Fptr)->rowlength = naxis1; /* reset row len to orig value */

            fbyte -= 10000;
            nbytes = 10000;
        }

        /* now move the rest of the rows */
        nseg = (naxis1 + 9999) / 10000;
        for (irow = naxis2 - 1; irow > 0; irow--)
        {
          fbyte = (nseg - 1) * 10000 + bytepos + 1;
          nbytes = naxis1 - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            /* read the row to be shifted (work backwards thru the table) */
            ffgtbb(fptr, irow, fbyte, nbytes, buffer, status);
            (fptr->Fptr)->rowlength = newlen;  /* set row length to new value */

            /* write the row in the new place */
            ffptbb(fptr, irow, fbyte + ninsert, nbytes, buffer, status);
            (fptr->Fptr)->rowlength = naxis1; /* reset row len to orig value */

            fbyte -= 10000;
            nbytes = 10000;
          }
        }

        /* now write the fill values into the new column */
        nbytes = minvalue(ninsert, 10000);
        memset(buffer, cfill, nbytes); /* initialize with fill value */

        nseg = (ninsert + 9999) / 10000;
        (fptr->Fptr)->rowlength = newlen;  /* set row length to its new value */

        for (irow = 1; irow <= naxis2; irow++)
        {
          fbyte = bytepos + 1;
          nbytes = ninsert - ((nseg - 1) * 10000);
          for (ii = 0; ii < nseg; ii++)
          {
            ffptbb(fptr, irow, fbyte, nbytes, buffer, status);
            fbyte += nbytes;
            nbytes = 10000;
          }
        }
        (fptr->Fptr)->rowlength = naxis1;  /* reset row length to orig value */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcdel(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis1,     /* I - width of the table, in bytes             */
           long naxis2,     /* I - number of rows in the table              */
           long ndelete,    /* I - number of bytes to delete in each row    */
           long bytepos,    /* I - rel. position in row to delete bytes     */
           int *status)     /* IO - error status                            */
/*
 delete 'ndelete' bytes from each row of the table at position 'bytepos'.
*/
{
    unsigned char buffer[10000];
    long newlen, i1, i2, ii, remain, nbytes, irow, nseg;

    if (*status > 0)
        return(*status);

    if (naxis2 == 0)
        return(*status);  /* just return if there are 0 rows in the table */

    newlen = naxis1 - ndelete;

    if (newlen <= 10000)
    {
      /*******************************************************************
      CASE #1: optimal case where whole new row fits in the work buffer
      *******************************************************************/
      i1 = bytepos + 1;
      i2 = i1 + ndelete;
      for (irow = 1; irow < naxis2; irow++)
      {
        ffgtbb(fptr, irow, i2, newlen, buffer, status); /* read row */
        (fptr->Fptr)->rowlength = newlen;  /* set row length to its new value */

        ffptbb(fptr, irow, i1, newlen, buffer, status); /* write row */
        (fptr->Fptr)->rowlength = naxis1;  /* reset row length to orig value */
      }

      /* now do the last row */
      remain = naxis1 - (bytepos + ndelete);

      if (remain > 0)
      {
        ffgtbb(fptr, naxis2, i2, remain, buffer, status); /* read row */
        (fptr->Fptr)->rowlength = newlen;  /* set row length to its new value */

        ffptbb(fptr, naxis2, i1, remain, buffer, status); /* write row */
        (fptr->Fptr)->rowlength = naxis1;  /* reset row length to orig value */
      }
    }
    else
    {
        /*****************************************************************
        CASE #2:  whole row doesn't fit in work buffer; move row in pieces
        ******************************************************************/

        nseg = (newlen + 9999) / 10000;
        for (irow = 1; irow < naxis2; irow++)
        {
          i1 = bytepos + 1;
          i2 = i1 + ndelete;

          nbytes = newlen - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            ffgtbb(fptr, irow, i2, nbytes, buffer, status); /* read bytes */
            (fptr->Fptr)->rowlength = newlen;  /* set row length to new value */

            ffptbb(fptr, irow, i1, nbytes, buffer, status); /* rewrite bytes */
            (fptr->Fptr)->rowlength = naxis1; /* reset row len to orig value */

            i1 += nbytes;
            i2 += nbytes;
            nbytes = 10000;
          }
        }

        /* now do the last row */
        remain = naxis1 - (bytepos + ndelete);

        if (remain > 0)
        {
          nseg = (remain + 9999) / 10000;
          i1 = bytepos + 1;
          i2 = i1 + ndelete;
          nbytes = remain - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            ffgtbb(fptr, naxis2, i2, nbytes, buffer, status);
            (fptr->Fptr)->rowlength = newlen;  /* set row length to new value */

            ffptbb(fptr, naxis2, i1, nbytes, buffer, status); /* write row */
            (fptr->Fptr)->rowlength = naxis1;  /* reset row len to orig value */

            i1 += nbytes;
            i2 += nbytes;
            nbytes = 10000;
          }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffkshf(fitsfile *fptr,  /* I - FITS file pointer                        */
           int colmin,      /* I - starting col. to be incremented; 1 = 1st */
           int colmax,      /* I - last column to be incremented            */
           int incre,       /* I - shift index number by this amount        */
           int *status)     /* IO - error status                            */
/*
  shift the index value on any existing column keywords
  This routine will modify the name of any keyword that begins with 'T'
  and has an index number in the range COLMIN - COLMAX, inclusive.

  if incre is positive, then the index values will be incremented.
  if incre is negative, then the kewords with index = COLMIN
  will be deleted and the index of higher numbered keywords will
  be decremented.
*/
{
    int nkeys, nmore, nrec, tstatus, i1;
    long ivalue;
    char rec[FLEN_CARD], q[FLEN_KEYWORD], newkey[FLEN_KEYWORD];

    ffghsp(fptr, &nkeys, &nmore, status);  /* get number of keywords */

    /* go thru header starting with the 9th keyword looking for 'TxxxxNNN' */

    for (nrec = 9; nrec <= nkeys; nrec++)
    {     
        ffgrec(fptr, nrec, rec, status);

        if (rec[0] == 'T')
        {
            i1 = 0;
            strncpy(q, &rec[1], 4);
            if (!strncmp(q, "BCOL", 4) || !strncmp(q, "FORM", 4) ||
                !strncmp(q, "TYPE", 4) || !strncmp(q, "SCAL", 4) ||
                !strncmp(q, "UNIT", 4) || !strncmp(q, "NULL", 4) ||
                !strncmp(q, "ZERO", 4) || !strncmp(q, "DISP", 4) ||
                !strncmp(q, "LMIN", 4) || !strncmp(q, "LMAX", 4) ||
                !strncmp(q, "DMIN", 4) || !strncmp(q, "DMAX", 4) ||
                !strncmp(q, "CTYP", 4) || !strncmp(q, "CRPX", 4) ||
                !strncmp(q, "CRVL", 4) || !strncmp(q, "CDLT", 4) ||
                !strncmp(q, "CROT", 4) || !strncmp(q, "CUNI", 4) )
              i1 = 5;
            else if (!strncmp(rec, "TDIM", 4) )
              i1 = 4;

            if (i1)
            {
              /* try reading the index number suffix */
              q[0] = '\0';
              strncat(q, &rec[i1], 8 - i1);

              tstatus = 0;
              ffc2ii(q, &ivalue, &tstatus);

              if (tstatus == 0 && ivalue >= colmin && ivalue <= colmax)
              {
                if (incre <= 0 && ivalue == colmin)       
                {
                  ffdrec(fptr, nrec, status); /* delete keyword */
                  nkeys = nkeys - 1;
                  nrec = nrec - 1;
                }
                else
                {
                  ivalue = ivalue + incre;
                  q[0] = '\0';
                  strncat(q, rec, i1);
     
                  ffkeyn(q, ivalue, newkey, status);
                  strncpy(rec, "        ", 8);    /* erase old keyword name */
                  i1 = strlen(newkey);
                  strncpy(rec, newkey, i1);   /* overwrite new keyword name */
                  ffmrec(fptr, nrec, rec, status);  /* modify the record */
                }
              }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffshft(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstbyte,  /* I - position of first byte in block to shift */
           long nbytes,     /* I - size of block of bytes to shift          */
           long nshift,     /* I - size of shift in bytes (+ or -)          */
           int *status)     /* IO - error status                            */
/*
    Shift block of bytes by nshift bytes (positive or negative).
    A positive nshift value moves the block down further in the file, while a
    negative value shifts the block towards the beginning of the file.
*/
{
    long ntodo, ntomov, ptr;
    char buffer[10000];

    if (*status > 0)
        return(*status);

    ntodo = nbytes;   /* total number of bytes to shift */

    if (nshift > 0)
            /* start at the end of the block and work backwards */
            ptr = firstbyte + nbytes;
    else
            /* start at the beginning of the block working forwards */
            ptr = firstbyte;

    while (ntodo)
    {
        /* number of bytes to move at one time */
        ntomov = minvalue(ntodo, 10000);

        if (nshift > 0)     /* if moving block down ... */
            ptr -= ntomov;

        /* move to position and read the bytes to be moved */
        ffmbyt(fptr, ptr, REPORT_EOF, status);
        ffgbyt(fptr, ntomov, buffer, status);

        /* move by shift amount and write the bytes */
        ffmbyt(fptr, ptr + nshift, IGNORE_EOF, status);
        if (ffpbyt(fptr, ntomov, buffer, status) > 0)
        {
           ffpmsg("Error while shifting block (ffshft)");
           return(*status);
        }

        ntodo -= ntomov;
        if (nshift < 0)     /* if moving block up ... */
            ptr += ntomov;
    }

    /* now overwrite the old data with fill */
    if ((fptr->Fptr)->hdutype == ASCII_TBL)
       memset(buffer, 32, 10000); /* fill ASCII tables with spaces */
    else
       memset(buffer,  0, 10000); /* fill other HDUs with zeros */


    if (nshift < 0)
    {
        ntodo = -nshift;
        /* point to the end of the shifted block */
        ptr = firstbyte + nbytes + nshift;
    }
    else
    {
        ntodo = nshift;
        /* point to original beginning of the block */
        ptr = firstbyte;
    }

    ffmbyt(fptr, ptr, REPORT_EOF, status);

    while (ntodo)
    {
        ntomov = minvalue(ntodo, 10000);
        ffpbyt(fptr, ntomov, buffer, status);
        ntodo -= ntomov;
    }
    return(*status);
}
