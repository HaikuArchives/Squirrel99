/*  This file, cfileio.c, contains the low-level file access routines.     */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.                                           */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stddef.h>  /* apparently needed to define size_t */
#include "fitsio2.h"
#include "group.h"

#define MAX_PREFIX_LEN 20  /* max length of file type prefix (e.g. 'http://') */
#define MAX_DRIVERS 20     /* max number of file I/O drivers */

typedef struct    /* structure containing pointers to I/O driver functions */ 
{   char prefix[MAX_PREFIX_LEN];
    int (*init)(void);
    int (*shutdown)(void);
    int (*setoptions)(int option);
    int (*getoptions)(int *options);
    int (*getversion)(int *version);
    int (*checkfile)(char *urltype, char *infile, char *outfile);
    int (*open)(char *filename, int rwmode, int *driverhandle);
    int (*create)(char *filename, int *drivehandle);
    int (*truncate)(int drivehandle, OFF_T size);
    int (*close)(int drivehandle);
    int (*remove)(char *filename);
    int (*size)(int drivehandle, OFF_T *size);
    int (*flush)(int drivehandle);
    int (*seek)(int drivehandle, OFF_T offset);
    int (*read)(int drivehandle, void *buffer, long nbytes);
    int (*write)(int drivehandle, void *buffer, long nbytes);
} fitsdriver;

fitsdriver driverTable[MAX_DRIVERS];  /* allocate driver tables */

int need_to_initialize = 1;    /* true if CFITSIO has not been initialized */
int no_of_drivers = 0;         /* number of currently defined I/O drivers */

/*--------------------------------------------------------------------------*/
int ffomem(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           const char *name,     /* I - name of file to open                */
           int mode,             /* I - 0 = open readonly; 1 = read/write   */
           void **buffptr,       /* I - address of memory pointer           */
           size_t *buffsize,     /* I - size of buffer, in bytes            */
           size_t deltasize,     /* I - increment for future realloc's      */
           void *(*mem_realloc)(void *p, size_t newsize), /* function       */
           int *status)          /* IO - error status                       */
/*
  Open an existing FITS file in core memory.  This is a specialized version
  of ffopen.
*/
{
    int driver, handle, hdutyp, slen, movetotype, extvers, extnum;
    char extname[FLEN_VALUE];
    OFF_T filesize;
    char urltype[MAX_PREFIX_LEN], infile[FLEN_FILENAME], outfile[FLEN_FILENAME];
    char extspec[FLEN_FILENAME], rowfilter[FLEN_FILENAME];
    char binspec[FLEN_FILENAME], colspec[FLEN_FILENAME];
    char imagecolname[FLEN_VALUE], rowexpress[FLEN_FILENAME];
    char *url, errmsg[FLEN_ERRMSG];
    char *hdtype[3] = {"IMAGE", "TABLE", "BINTABLE"};

    if (*status > 0)
        return(*status);

    *fptr = 0;                   /* initialize null file pointer */

    if (need_to_initialize)           /* this is called only once */
    {
        *status = fits_init_cfitsio();

        if (*status > 0)
            return(*status);
    }

    url = (char *) name;
    while (*url == ' ')  /* ignore leading spaces in the file spec */
        url++;

        /* parse the input file specification */
    ffiurl(url, urltype, infile, outfile, extspec,
              rowfilter, binspec, colspec, status);

    strcpy(urltype, "memkeep://");   /* URL type for pre-existing memory file */

    *status = urltype2driver(urltype, &driver);

    if (*status > 0)
    {
        ffpmsg("could not find driver for pre-existing memory file: (ffomem)");
        return(*status);
    }

    /* call driver routine to open the memory file */
    *status =   mem_openmem( buffptr, buffsize,deltasize,
                            mem_realloc,  &handle);

    if (*status > 0)
    {
         ffpmsg("failed to open pre-existing memory file: (ffomem)");
         return(*status);
    }

        /* get initial file size */
    *status = (*driverTable[driver].size)(handle, &filesize);

    if (*status > 0)
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed get the size of the memory file: (ffomem)");
        return(*status);
    }

        /* allocate fitsfile structure and initialize = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

    if (!(*fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        return(*status = MEMORY_ALLOCATION);
    }

        /* allocate FITSfile structure and initialize = 0 */
    (*fptr)->Fptr = (FITSfile *) calloc(1, sizeof(FITSfile));

    if (!((*fptr)->Fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        free(*fptr);
        *fptr = 0;       
        return(*status = MEMORY_ALLOCATION);
    }

    slen = strlen(url) + 1;
    slen = maxvalue(slen, 32); /* reserve at least 32 chars */ 
    ((*fptr)->Fptr)->filename = (char *) malloc(slen); /* mem for file name */

    if ( !(((*fptr)->Fptr)->filename) )
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate memory for filename: (ffopen)");
        ffpmsg(url);
        free((*fptr)->Fptr);
        free(*fptr);
        *fptr = 0;              /* return null file pointer */
        return(*status = MEMORY_ALLOCATION);
    }

        /* store the parameters describing the file */
    ((*fptr)->Fptr)->filehandle = handle;        /* file handle */
    ((*fptr)->Fptr)->driver = driver;            /* driver number */
    strcpy(((*fptr)->Fptr)->filename, url);      /* full input filename */
    ((*fptr)->Fptr)->filesize = filesize;        /* physical file size */
    ((*fptr)->Fptr)->logfilesize = filesize;     /* logical file size */
    ((*fptr)->Fptr)->writemode = mode;      /* read-write mode    */
    ((*fptr)->Fptr)->datastart = DATA_UNDEFINED; /* unknown start of data */
    ((*fptr)->Fptr)->curbuf = -1;             /* undefined current IO buffer */
    ((*fptr)->Fptr)->open_count = 1;     /* structure is currently used once */
    ((*fptr)->Fptr)->validcode = VALIDSTRUC; /* flag denoting valid structure */

    ffldrc(*fptr, 0, REPORT_EOF, status);     /* load first record */

    if (ffrhdu(*fptr, &hdutyp, status) > 0)  /* determine HDU structure */
    {
        ffpmsg(
          "ffopen could not interpret primary array header of file: (ffomem)");
        ffpmsg(url);

        if (*status == UNKNOWN_REC)
           ffpmsg("This does not look like a FITS file.");

        ffclos(*fptr, status);
        *fptr = 0;              /* return null file pointer */
    }

    /* ---------------------------------------------------------- */
    /* move to desired extension, if specified as part of the URL */
    /* ---------------------------------------------------------- */

    imagecolname[0] = '\0';
    rowexpress[0] = '\0';

    if (*extspec)
    {
       /* parse the extension specifier into individual parameters */
       ffexts(extspec, &extnum, 
         extname, &extvers, &movetotype, imagecolname, rowexpress, status);


      if (*status > 0)
          return(*status);

      if (extnum)
      {
        ffmahd(*fptr, extnum + 1, &hdutyp, status);
      }
      else if (*extname) /* move to named extension, if specified */
      {
        ffmnhd(*fptr, movetotype, extname, extvers, status);
      }

      if (*status > 0)
      {
        ffpmsg("ffopen could not move to the specified extension:");
        if (extnum > 0)
        {
          sprintf(errmsg,
          " extension number %d doesn't exist or couldn't be opened.",extnum);
          ffpmsg(errmsg);
        }
        else
        {
          sprintf(errmsg,
          " extension with EXTNAME = %s,", extname);
          ffpmsg(errmsg);

          if (extvers)
          {
             sprintf(errmsg,
             "           and with EXTVERS = %d,", extvers);
             ffpmsg(errmsg);
          }

          if (movetotype != ANY_HDU)
          {
             sprintf(errmsg,
             "           and with XTENSION = %s,", hdtype[movetotype]);
             ffpmsg(errmsg);
          }

          ffpmsg(" doesn't exist or couldn't be opened.");
        }
        return(*status);
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffopen(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           const char *name,     /* I - full name of file to open           */
           int mode,             /* I - 0 = open readonly; 1 = read/write   */
           int *status)          /* IO - error status                       */
/*
  Open an existing FITS file with either readonly or read/write access.
*/
{
    int  driver, hdutyp, slen, writecopy, isopen;
    OFF_T filesize;
    long rownum, nrows, goodrows;
    int extnum, extvers, handle, movetotype;
    char urltype[MAX_PREFIX_LEN], infile[FLEN_FILENAME], outfile[FLEN_FILENAME];
    char origurltype[MAX_PREFIX_LEN], extspec[FLEN_FILENAME];
    char extname[FLEN_VALUE], rowfilter[FLEN_FILENAME];
    char imagecolname[FLEN_VALUE], rowexpress[FLEN_FILENAME];
    char binspec[FLEN_FILENAME], colspec[FLEN_FILENAME];
    char histfilename[FLEN_FILENAME];
    char filtfilename[FLEN_FILENAME];
    char wtcol[FLEN_VALUE];
    char minname[4][FLEN_VALUE], maxname[4][FLEN_VALUE];
    char binname[4][FLEN_VALUE];
    char card[FLEN_CARD];

    char *url;
    double minin[4], maxin[4], binsizein[4], weight;
    int imagetype, haxis, recip;
    char colname[4][FLEN_VALUE];
    char errmsg[FLEN_ERRMSG];
    char *hdtype[3] = {"IMAGE", "TABLE", "BINTABLE"};
    char *rowselect = 0;

    if (*status > 0)
        return(*status);

    *fptr = 0;              /* initialize null file pointer */
    writecopy = 0;  /* have we made a write-able copy of the input file? */

    if (need_to_initialize)           /* this is called only once */
       *status = fits_init_cfitsio();

    if (*status > 0)
        return(*status);

    url = (char *) name;
    while (*url == ' ')  /* ignore leading spaces in the filename */
        url++;

    if (*url == '\0')
    {
        ffpmsg("Name of file to open is blank. (ffopen)");
        return(*status = FILE_NOT_OPENED);
    }

        /* parse the input file specification */
    ffiurl(url, urltype, infile, outfile, extspec,
              rowfilter, binspec, colspec, status);

    if (*status > 0)
    {
        ffpmsg("could not parse the input filename: (ffopen)");
        ffpmsg(url);
        return(*status);
    }

    imagecolname[0] = '\0';
    rowexpress[0] = '\0';

    if (*extspec)
    {
       /* parse the extension specifier into individual parameters */
       ffexts(extspec, &extnum, 
         extname, &extvers, &movetotype, imagecolname, rowexpress, status);

      if (*status > 0)
          return(*status);
    }

    /*-------------------------------------------------------------------*/
    /* special cases:                                                    */
    /*-------------------------------------------------------------------*/

    histfilename[0] = '\0';
    filtfilename[0] = '\0';
    if (*outfile && (*binspec || *imagecolname))
    {
        /* if binspec or imagecolumn are specified, then the  */
        /* output file name is intended for the final image,  */
        /* and not a copy of the input file.                  */
        strcpy(histfilename, outfile);
        outfile[0] = '\0';
    }
    else if (*outfile && (*rowfilter || *colspec))
    {
        /* if rowfilter or colspece are specified, then the    */
        /* output file name is intended for the filtered file  */
        /* and not a copy of the input file.                   */
        strcpy(filtfilename, outfile);
        outfile[0] = '\0';
    }

    /*-------------------------------------------------------------------*/
    /* check if this same file is already open, and if so, attach to it  */
    /*-------------------------------------------------------------------*/

    if (fits_already_open(fptr, url, urltype, infile, extspec, rowfilter,
            binspec, colspec, mode, &isopen, status) > 0)
    {
        return(*status);
    }

    if (isopen)
       goto move2hdu;  

    /* get the driver number corresponding to this urltype */
    *status = urltype2driver(urltype, &driver);

    if (*status > 0)
    {
        ffpmsg("could not find driver for this file: (ffopen)");
        ffpmsg(urltype);
        ffpmsg(url);
        return(*status);
    }

    /*-------------------------------------------------------------------
        deal with all those messy special cases which may require that
        a different driver be used:
            - is disk file compressed?
            - are ftp: or http: files compressed?
            - has user requested that a local copy be made of
              the ftp or http file?
      -------------------------------------------------------------------*/

    if (driverTable[driver].checkfile)
    {
        strcpy(origurltype,urltype);  /* Save the urltype */

        /* 'checkfile' may modify the urltype, infile and outfile strings */
        *status =  (*driverTable[driver].checkfile)(urltype, infile, outfile);

        if (*status)
        {
            ffpmsg("checkfile failed for this file: (ffopen)");
            ffpmsg(url);
            return(*status);
        }

        if (strcmp(origurltype, urltype))  /* did driver changed on us? */
        {
            *status = urltype2driver(urltype, &driver);
            if (*status > 0)
            {
                ffpmsg("could not change driver for this file: (ffopen)");
                ffpmsg(url);
                ffpmsg(urltype);
                return(*status);
            }
        }
    }

    /* call appropriate driver to open the file */
    if (driverTable[driver].open)
    {
        *status =  (*driverTable[driver].open)(infile, mode, &handle);
        if (*status > 0)
        {
            ffpmsg("failed to find or open the following file: (ffopen)");
            ffpmsg(url);
            return(*status);
       }
    }
    else
    {
        ffpmsg("cannot open an existing file of this type: (ffopen)");
        ffpmsg(url);
        return(*status = FILE_NOT_OPENED);
    }

        /* get initial file size */
    *status = (*driverTable[driver].size)(handle, &filesize);
    if (*status > 0)
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed get the size of the following file: (ffopen)");
        ffpmsg(url);
        return(*status);
    }

        /* allocate fitsfile structure and initialize = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

    if (!(*fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        return(*status = MEMORY_ALLOCATION);
    }

        /* allocate FITSfile structure and initialize = 0 */
    (*fptr)->Fptr = (FITSfile *) calloc(1, sizeof(FITSfile));

    if (!((*fptr)->Fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        free(*fptr);
        *fptr = 0;       
        return(*status = MEMORY_ALLOCATION);
    }

    slen = strlen(url) + 1;
    slen = maxvalue(slen, 32); /* reserve at least 32 chars */ 
    ((*fptr)->Fptr)->filename = (char *) malloc(slen); /* mem for file name */

    if ( !(((*fptr)->Fptr)->filename) )
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate memory for filename: (ffopen)");
        ffpmsg(url);
        free((*fptr)->Fptr);
        free(*fptr);
        *fptr = 0;              /* return null file pointer */
        return(*status = MEMORY_ALLOCATION);
    }

        /* store the parameters describing the file */
    ((*fptr)->Fptr)->filehandle = handle;        /* file handle */
    ((*fptr)->Fptr)->driver = driver;            /* driver number */
    strcpy(((*fptr)->Fptr)->filename, url);      /* full input filename */
    ((*fptr)->Fptr)->filesize = filesize;        /* physical file size */
    ((*fptr)->Fptr)->logfilesize = filesize;     /* logical file size */
    ((*fptr)->Fptr)->writemode = mode;           /* read-write mode    */
    ((*fptr)->Fptr)->datastart = DATA_UNDEFINED; /* unknown start of data */
    ((*fptr)->Fptr)->curbuf = -1;            /* undefined current IO buffer */
    ((*fptr)->Fptr)->open_count = 1;      /* structure is currently used once */
    ((*fptr)->Fptr)->validcode = VALIDSTRUC; /* flag denoting valid structure */

    ffldrc(*fptr, 0, REPORT_EOF, status);     /* load first record */

    if (ffrhdu(*fptr, &hdutyp, status) > 0)  /* determine HDU structure */
    {
        ffpmsg(
          "ffopen could not interpret primary array header of file: ");
        ffpmsg(url);

        if (*status == UNKNOWN_REC)
           ffpmsg("This does not look like a FITS file.");

        ffclos(*fptr, status);
        *fptr = 0;              /* return null file pointer */
        return(*status);
    }

    /* ------------------------------------------------------------- */
    /* At this point, the input file has been opened. If outfile was */
    /* specified, then we have opened a copy of the file, not the    */
    /* original file so it is safe to modify it if necessary         */
    /* ------------------------------------------------------------- */

    if (*outfile)
        writecopy = 1;  

move2hdu:

    /* ---------------------------------------------------------- */
    /* move to desired extension, if specified as part of the URL */
    /* ---------------------------------------------------------- */

    if (*extspec)
    {
      if (extnum)  /* extension number was specified */
      {
        ffmahd(*fptr, extnum + 1, &hdutyp, status);
      }
      else if (*extname) /* move to named extension, if specified */
      {
        ffmnhd(*fptr, movetotype, extname, extvers, status);
      }

      if (*status > 0)  /* clean up after error */
      {
        ffpmsg("ffopen could not move to the specified extension:");
        if (extnum > 0)
        {
          sprintf(errmsg,
          " extension number %d doesn't exist or couldn't be opened.",extnum);
          ffpmsg(errmsg);
        }
        else
        {
          sprintf(errmsg,
          " extension with EXTNAME = %s,", extname);
          ffpmsg(errmsg);

          if (extvers)
          {
             sprintf(errmsg,
             "           and with EXTVERS = %d,", extvers);
             ffpmsg(errmsg);
          }

          if (movetotype != ANY_HDU)
          {
             sprintf(errmsg,
             "           and with XTENSION = %s,", hdtype[movetotype]);
             ffpmsg(errmsg);
          }

          ffpmsg(" doesn't exist or couldn't be opened.");
        }
        return(*status);
      }
    }

    if (*imagecolname)
    {
       /* we need to open an image contained in a single table cell */
       /* First, determine which row of the table to use. */

       if (isdigit((int) *rowexpress))  /* is the row specification a number? */
       {
          sscanf(rowexpress, "%ld", &rownum);
          if (rownum < 1)
          {
             ffpmsg("illegal rownum for image cell:");
             ffpmsg(rowexpress);
             ffpmsg("Could not open the following image in a table cell:");
             ffpmsg(extspec);
             return(*status = BAD_ROW_NUM);
          }
       }
       else if (fits_find_first_row(*fptr, rowexpress, &rownum, status) > 0)
       {
          ffpmsg("Failed to find row matching this expression:");
          ffpmsg(rowexpress);
          ffpmsg("Could not open the following image in a table cell:");
          ffpmsg(extspec);
          return(*status);
       }

       if (rownum == 0)
       {
          ffpmsg("row statisfying this expression doesn't exist::");
          ffpmsg(rowexpress);
          ffpmsg("Could not open the following image in a table cell:");
          ffpmsg(extspec);
          return(*status = BAD_ROW_NUM);
       }

       /* determine the name of the new file to contain copy of the image */
       if (*histfilename)
           strcpy(outfile, histfilename); /* the original outfile name */
       else
           strcpy(outfile, "mem://_1");  /* create image file in memory */

       /* Copy the image into new primary array and open it as the current */
       /* fptr.  This will close the table that contains the original image. */

       if (fits_copy_image_cell(fptr, outfile, imagecolname, rownum,
                                status) > 0)
       {
          ffpmsg("Failed to copy table cell to new primary array:");
          ffpmsg(extspec);
          return(*status);
       }

       writecopy = 1;  /* we are now dealing with a copy of the original file */

       /* add some HISTORY; fits_copy_image_cell also wrote HISTORY keywords */
       
       if (*extname)
         sprintf(card,"HISTORY  in HDU '%.16s' of file '%.36s'", extname, infile);
       else
         sprintf(card,"HISTORY  in HDU %d of file '%.45s'", extnum, infile);

       ffprec(*fptr, card, status);
    }

    /* --------------------------------------------------------------------- */
    /* edit columns (and/or keywords) in the table, if specified in the URL  */
    /* --------------------------------------------------------------------- */
 
    if (*colspec)
    {
       /* the column specifier will modify the file, so make sure */
       /* we are already dealing with a copy, or else make a new copy */

       if (!writecopy)  /* Is the current file already a copy? */
           writecopy = fits_is_this_a_copy(urltype);

       if (!writecopy)
       {
           if (*filtfilename && *outfile == '\0')
               strcpy(outfile, filtfilename); /* the original outfile name */
           else
               strcpy(outfile, "mem://_1");   /* will create copy in memory */

           writecopy = 1;
       }
       else
       {
           ((*fptr)->Fptr)->writemode = READWRITE; /* we have write access */
           outfile[0] = '\0';
       }

       if (ffedit_columns(fptr, outfile, colspec, status) > 0)
       {
           ffpmsg("editing columns in input table failed (ffopen)");
           ffpmsg(" while trying to perform the following operation:");
           ffpmsg(colspec);
           return(*status);
       }
    }

    /* ------------------------------------------------------------------- */
    /* select rows from the table, if specified in the URL                 */
    /* or select a subimage (if this is an image HDU and not a table)      */
    /* ------------------------------------------------------------------- */
 
    if (*rowfilter)
    {

     fits_get_hdu_type(*fptr, &hdutyp, status);  /* get type of HDU */
     if (hdutyp == IMAGE_HDU)
     {
        /* this is an image so 'rowfilter' is an image section specification */

        if (*filtfilename && *outfile == '\0')
            strcpy(outfile, filtfilename); /* the original outfile name */
        else if (*outfile == '\0') /* output file name not already defined? */
            strcpy(outfile, "mem://_2");  /* will create file in memory */

        /* create new file containing the image section, plus a copy of */
        /* any other HDUs that exist in the input file.  This routine   */
        /* will close the original image file and return a pointer      */
        /* to the new file. */

        if (fits_select_image_section(fptr, outfile, rowfilter, status) > 0)
        {
           ffpmsg("on-the-fly selection of image section failed (ffopen)");
           ffpmsg(" while trying to use the following section filter:");
           ffpmsg(rowfilter);
           return(*status);
        }
        writecopy = 1;
     }
     else
     {
       /* this is a table HDU, so the rowfilter is really a row filter */

      if (*binspec)
      {
        /*  since we are going to make a histogram of the selected rows,   */
        /*  it would be a waste of time and memory to make a whole copy of */
        /*  the selected rows.  Instead, just construct an array of TRUE   */
        /*  or FALSE values that indicate which rows are to be included    */
        /*  in the histogram and pass that to the histogram generating     */
        /*  routine                                                        */

        fits_get_num_rows(*fptr, &nrows, status);  /* get no. of rows */

        rowselect = (char *) calloc(nrows, 1);
        if (!rowselect)
        {
           ffpmsg(
           "failed to allocate memory for selected columns array (ffopen)");
           ffpmsg(" while trying to select rows with the following filter:");
           ffpmsg(rowfilter);
           return(*status = MEMORY_ALLOCATION);
        }

        if (fits_find_rows(*fptr, rowfilter, 1L, nrows, &goodrows,
            rowselect, status) > 0)
        {
           ffpmsg("selection of rows in input table failed (ffopen)");
           ffpmsg(" while trying to select rows with the following filter:");
           ffpmsg(rowfilter);
           return(*status);
        }
      }
      else
      {
        if (!writecopy)  /* Is the current file already a copy? */
           writecopy = fits_is_this_a_copy(urltype);

        if (!writecopy)
        {
           if (*filtfilename && *outfile == '\0')
               strcpy(outfile, filtfilename); /* the original outfile name */
           else if (*outfile == '\0') /* output file name not already defined? */
             strcpy(outfile, "mem://_2");  /* will create copy in memory */
        }
        else
        {
           ((*fptr)->Fptr)->writemode = READWRITE; /* we have write access */
           outfile[0] = '\0';
        }

        /* select rows in the table.  If a copy of the input file has */
        /* not already been made, then this routine will make a copy */
        /* and then close the input file, so that the modifications will */
        /* only be made on the copy, not the original */

        if (ffselect_table(fptr, outfile, rowfilter, status) > 0)
        {
          ffpmsg("on-the-fly selection of rows in input table failed (ffopen)");
           ffpmsg(" while trying to select rows with the following filter:");
           ffpmsg(rowfilter);
           return(*status);
        }

      }    /* end of no binspec case */
     }   /* end of table HDU case */
    }  /* end of rowfilter exists case */

    /* ------------------------------------------------------------------- */
    /* make an image histogram by binning columns, if specified in the URL */
    /* ------------------------------------------------------------------- */
 
    if (*binspec)
    {
       if (*histfilename)
           strcpy(outfile, histfilename); /* the original outfile name */
       else
           strcpy(outfile, "mem://_3");  /* create histogram in memory */
                                         /* if not already copied the file */ 

       /* parse the binning specifier into individual parameters */
       ffbins(binspec, &imagetype, &haxis, colname, 
                          minin, maxin, binsizein, 
                          minname, maxname, binname,
                          &weight, wtcol, &recip, status);

       /* Create the histogram primary array and open it as the current fptr.  */
       /* This will close the table that was used to create the histogram. */
       ffhist(fptr, outfile, imagetype, haxis, colname, minin, maxin,
              binsizein, minname, maxname, binname,
              weight, wtcol, recip, rowselect, status);

       if (*status > 0)
       {
           ffpmsg("on-the-fly histogramming of input table failed (ffopen)");
           ffpmsg(" while trying to execute the following histogram specification:");
           ffpmsg(binspec);
           return(*status);
       }

       if (rowselect)
          free(rowselect);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffreopen(fitsfile *openfptr, /* I - FITS file pointer to open file  */ 
             fitsfile **newfptr,  /* O - pointer to new re opened file   */
             int *status)        /* IO - error status                   */
/*
  Reopen an existing FITS file with either readonly or read/write access.
  The reopened file shares the same FITSfile structure but may point to a
  different HDU within the file.
*/
{
    if (*status > 0)
        return(*status);

    /* check that the open file pointer is valid */
    if (!openfptr)
        return(*status = NULL_INPUT_PTR);
    else if ((openfptr->Fptr)->validcode != VALIDSTRUC) /* check magic value */
        return(*status = BAD_FILEPTR); 

        /* allocate fitsfile structure and initialize = 0 */
    *newfptr = (fitsfile *) calloc(1, sizeof(fitsfile));

    (*newfptr)->Fptr = openfptr->Fptr; /* both point to the same structure */
    (*newfptr)->HDUposition = 0;  /* set initial position to primary array */
    (((*newfptr)->Fptr)->open_count)++;   /* increment the file usage counter */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_already_open(fitsfile **fptr, /* I/O - FITS file pointer       */ 
           char *url, 
           char *urltype, 
           char *infile, 
           char *extspec, 
           char *rowfilter,
           char *binspec, 
           char *colspec, 
           int  mode,             /* I - 0 = open readonly; 1 = read/write   */
           int  *isopen,          /* O - 1 = file is already open            */
           int  *status)          /* IO - error status                       */
/*
  Check if the file to be opened is already open.  If so, then attach to it.
*/
     /*
       this function was changed so that for files of access method FILE://
       the file paths are compared using standard URL syntax and absolute
       paths (as opposed to relative paths). This eliminates some instances
       where a file is already opened but it is not realized because it
       was opened with another file path. For instance, if the CWD is
       /a/b/c and I open /a/b/c/foo.fits then open ./foo.fits the previous
       version of this function would not have reconized that the two files
       were the same. This version does reconize that the two files are
       the same.
     */
{

    FITSfile *oldFptr;
    int ii;
    char oldurltype[MAX_PREFIX_LEN], oldinfile[FLEN_FILENAME];
    char oldextspec[FLEN_FILENAME], oldoutfile[FLEN_FILENAME];
    char oldrowfilter[FLEN_FILENAME];
    char oldbinspec[FLEN_FILENAME], oldcolspec[FLEN_FILENAME];
    char cwd[FLEN_FILENAME];
    char tmpStr[FLEN_FILENAME];
    char tmpinfile[FLEN_FILENAME];

    *isopen = 0;

    if(strcasecmp(urltype,"FILE://") == 0)
      {
        fits_path2url(infile,tmpinfile,status);

        if(tmpinfile[0] != '/')
          {
            fits_get_cwd(cwd,status);
            strcat(cwd,"/");
            strcat(cwd,tmpinfile);
            fits_clean_url(cwd,tmpinfile,status);
          }
      }
    else
      strcpy(tmpinfile,infile);

    for (ii = 0; ii < NIOBUF; ii++)   /* check every buffer */
    {
        ffcurbuf(ii, &oldFptr);  
        if (oldFptr)            /* this is the current buffer of a file */
        {
          ffiurl(oldFptr->filename, oldurltype, 
                    oldinfile, oldoutfile, oldextspec, oldrowfilter, 
                    oldbinspec, oldcolspec, status);

          if (*status > 0)
          {
            ffpmsg("could not parse the previously opened filename: (ffopen)");
            ffpmsg(oldFptr->filename);
            return(*status);
          }

          if(strcasecmp(oldurltype,"FILE://") == 0)
            {
              fits_path2url(oldinfile,tmpStr,status);
              
              if(tmpStr[0] != '/')
                {
                  fits_get_cwd(cwd,status);
                  strcat(cwd,"/");
                  strcat(cwd,tmpStr);
                  fits_clean_url(cwd,tmpStr,status);
                }

              strcpy(oldinfile,tmpStr);
            }

          if (!strcmp(urltype, oldurltype) && !strcmp(tmpinfile, oldinfile) )
          {
              /* identical type of file and root file name */

              if ( (!rowfilter[0] && !oldrowfilter[0] &&
                    !binspec[0]   && !oldbinspec[0] &&
                    !colspec[0]   && !oldcolspec[0])

                  /* no filtering or binning specs for either file, so */
                  /* this is a case where the same file is being reopened. */
                  /* It doesn't matter if the extensions are different */

                      ||   /* or */

                  (!strcmp(rowfilter, oldrowfilter) &&
                   !strcmp(binspec, oldbinspec)     &&
                   !strcmp(colspec, oldcolspec)     &&
                   !strcmp(extspec, oldextspec) ) )

                  /* filtering specs are given and are identical, and */
                  /* the same extension is specified */

              {
                  if (mode == READWRITE && oldFptr->writemode == READONLY)
                  {
                    /*
                      cannot assume that a file previously opened with READONLY
                      can now be written to (e.g., files on CDROM, or over the
                      the network, or STDIN), so return with an error.
                    */

                    ffpmsg(
                "cannot reopen file READWRITE when previously opened READONLY");
                    ffpmsg(url);
                    return(*status = FILE_NOT_OPENED);
                  }

                  *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

                  if (!(*fptr))
                  {
                     ffpmsg(
                   "failed to allocate structure for following file: (ffopen)");
                     ffpmsg(url);
                     return(*status = MEMORY_ALLOCATION);
                  }

                  (*fptr)->Fptr = oldFptr; /* point to the structure */
                  (*fptr)->HDUposition = 0;     /* set initial position */
                (((*fptr)->Fptr)->open_count)++;  /* increment usage counter */

                  if (binspec[0])  /* if binning specified, don't move */
                      extspec[0] = '\0';

                  /* all the filtering has already been applied, so ignore */
                  rowfilter[0] = '\0';
                  binspec[0] = '\0';
                  colspec[0] = '\0';

                  *isopen = 1;
              }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_is_this_a_copy(char *urltype) /* I - type of file */
/*
  specialized routine that returns 1 if the file is known to be a temporary
  copy of the originally opened file.  Otherwise it returns 0.
*/
{
  int iscopy;

  if (!strncmp(urltype, "mem", 3) )
     iscopy = 1;    /* file copy is in memory */
  else if (!strncmp(urltype, "compress", 8) )
     iscopy = 1;    /* compressed diskfile that is uncompressed in memory */
  else if (!strncmp(urltype, "http", 4) )
     iscopy = 1;    /* copied file using http protocol */
  else if (!strncmp(urltype, "ftp", 3) )
     iscopy = 1;    /* copied file using ftp protocol */
  else if (!strncpy(urltype, "stdin", 5) )
     iscopy = 1;    /* piped stdin has been copied to memory */
  else
     iscopy = 0;    /* file is not known to be a copy */
 
    return(iscopy);
}
/*--------------------------------------------------------------------------*/
int ffedit_columns(
           fitsfile **fptr,  /* IO - pointer to input table; on output it  */
                             /*      points to the new selected rows table */
           char *outfile,    /* I - name for output file */
           char *expr,       /* I - column edit expression    */
           int *status)
/*
   modify columns in a table and/or header keywords in the HDU
*/
{
    fitsfile *newptr;
    int ii, hdunum, slen, colnum;
    char *cptr, *cptr2, *cptr3, clause[FLEN_FILENAME], keyname[FLEN_KEYWORD];
    char colname[FLEN_VALUE], oldname[FLEN_VALUE], colformat[FLEN_VALUE];
    char *file_expr = NULL;

    if (*outfile)
    {
      /* create new empty file in to hold the selected rows */
      if (ffinit(&newptr, outfile, status) > 0)
      {
        ffpmsg("failed to create file for copy (ffedit_columns)");
        return(*status);
      }

      fits_get_hdu_num(*fptr, &hdunum);  /* current HDU number in input file */

      /* copy all HDUs to the output copy */

      for (ii = 1; 1; ii++)
      {
        if (fits_movabs_hdu(*fptr, ii, NULL, status) > 0)
            break;

        fits_copy_hdu(*fptr, newptr, 0, status);
      }

      if (*status == END_OF_FILE)
      {
        *status = 0;              /* got the expected EOF error; reset = 0  */
        ffxmsg(-2, NULL);         /* remove extraneous error message */
      }
      else if (*status > 0)
      {
        ffclos(newptr, status);
        ffpmsg("failed to copy all HDUs from input file (ffedit_columns)");
        return(*status);
      }

      /* close the original file and return ptr to the new image */
      ffclos(*fptr, status);

      *fptr = newptr; /* reset the pointer to the new table */

      /* move back to the selected table HDU */
      if (fits_movabs_hdu(*fptr, hdunum, NULL, status) > 0)
      {
         ffpmsg("failed to copy the input file (ffedit_columns)");
         return(*status);
      }
    }

    /* remove the "col " from the beginning of the column edit expression */
    cptr = expr + 4;

    while (*cptr == ' ')
         cptr++;         /* skip leading white space */
   
    /* Check if need to import expression from a file */

    if( *cptr=='@' ) {
       if( ffimport_file( cptr+1, &file_expr, status ) ) return(*status);
       cptr = file_expr;
       while (*cptr == ' ')
          cptr++;         /* skip leading white space... again */
    }

    /* parse expression and get first clause, if more than 1 */

    while ((slen = fits_get_token(&cptr, ";", clause, NULL)) > 0 )
    {
        if( *cptr==';' ) cptr++;
        clause[slen] = '\0';

        if (clause[0] == '!' || clause[0] == '-')
        {
            /* delete this column or keyword */

            if (ffgcno(*fptr, CASEINSEN, &clause[1], &colnum, status) <= 0)
            {
                /* a column with this name exists, so try to delete it */
                if (ffdcol(*fptr, colnum, status) > 0)
                {
                    ffpmsg("failed to delete column in input file:");
                    ffpmsg(clause);
                    if( file_expr ) free( file_expr );
                    return(*status);
                }
            }
            else
            {
                /* try deleting a keyword with this name */
                *status = 0;
                if (ffdkey(*fptr, &clause[1], status) > 0)
                {
                    ffpmsg("column or keyword to be deleted does not exist:");
                    ffpmsg(clause);
                    if( file_expr ) free( file_expr );
                    return(*status);
                }
            }
        }
        else
        {
            /*
               this is either a column or keyword name followed by a 
               single "=" and a calculation expression, or
               a column name followed by double = ("==") followed
               by the new name to which it should be renamed.
            */

            cptr2 = clause;
            slen = fits_get_token(&cptr2, " =", colname, NULL);
            if (slen == 0)
            {
                ffpmsg("error: column or keyword name is blank:");
                ffpmsg(clause);
                if( file_expr ) free( file_expr );
                return(*status= URL_PARSE_ERROR);
            }

            while (*cptr2 == ' ')
                 cptr2++;         /* skip white space */

            if (*cptr2 != '=')
            {
               ffpmsg("Syntax error in columns specifier in input URL:");
               ffpmsg(cptr2);
               if( file_expr ) free( file_expr );
               return(*status = URL_PARSE_ERROR);
            }

            cptr2++;   /* skip over the first '=' */

            if (*cptr2 == '=')
            {
                /*
                    Case 1:  rename a column or keyword;  syntax is
                    "new_name == old_name"
                */

                cptr2++;  /* skip the 2nd '=' */
                while (*cptr2 == ' ')
                      cptr2++;       /* skip white space */

                fits_get_token(&cptr2, " ", oldname, NULL);

                /* get column number of the existing column */
                if (ffgcno(*fptr, CASEINSEN, oldname, &colnum, status) <= 0)
                {
                    /* modify the TTYPEn keyword value with the new name */
                    ffkeyn("TTYPE", colnum, keyname, status);

                    if (ffmkys(*fptr, keyname, colname, NULL, status) > 0)
                    {
                      ffpmsg("failed to rename column in input file");
                      ffpmsg(" oldname =");
                      ffpmsg(oldname);
                      ffpmsg(" newname =");
                      ffpmsg(colname);
                      if( file_expr ) free( file_expr );
                      return(*status);
                    }
                }
                else
                {
                    /* try renaming a keyword */
                    *status = 0;
                    if (ffmnam(*fptr, oldname, colname, status) > 0)
                    {
                      ffpmsg("column or keyword to be renamed does not exist:");
                        ffpmsg(clause);
                        if( file_expr ) free( file_expr );
                        return(*status);
                    }
                }
            }  
            else
            {
                /* this must be a general column/keyword calc expression */
                /* "name = expression" or "colname(TFORM) = expression" */

                /* parse the name and TFORM values, if present */
                colformat[0] = '\0';
                cptr3 = colname;

                fits_get_token(&cptr3, "(", oldname, NULL);

                if (cptr3[0] == '(' )
                {
                   cptr3++;  /* skip the '(' */
                   fits_get_token(&cptr3, ")", colformat, NULL);
                }

                /* calculate values for the column or keyword */ 
                fits_calculator(*fptr, cptr2, *fptr, oldname, colformat,
       	                        status);
            }
        }
    }

    if( file_expr ) free( file_expr );
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_copy_image_cell(
           fitsfile **fptr,  /* IO - pointer to input table; on output it  */
                             /*      points to the new image primary array */
           char *outfile,    /* I - name for output file                   */
           char *colname,    /* I - name of column containing the image    */
           long rownum,      /* I - number of the row containing the image */
           int *status)
{
    fitsfile *newptr;
    unsigned char buffer[30000];
    int ii, hdutype, colnum, typecode, bitpix, naxis, maxelem, tstatus;
    long naxes[9], nbytes, firstbyte, twidth;
    OFF_T repeat, startpos, elemnum, rowlen;
    long incre, tnull, ntodo;
    double scale, zero;
    char tform[20];
    char keyname[FLEN_KEYWORD], card[FLEN_CARD];
    char axisnum[10], root[9];

    if (*status > 0)
        return(*status);

    /* get column number */
    if (ffgcno(*fptr, CASEINSEN, colname, &colnum, status) > 0)
    {
        ffpmsg("column containing image in table cell does not exist:");
        ffpmsg(colname);
        return(*status);
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr(*fptr, colnum, rownum, 1L, 1L, 0, &scale, &zero,
         tform, &twidth, &typecode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, (char *) buffer, status) > 0 )
         return(*status);

    if (hdutype != BINARY_TBL)
    {
        ffpmsg("This extension is not a binary table.");
        ffpmsg(" Cannot open the image in a binary table cell.");
        return(*status = NOT_BTABLE);
    }

    if (typecode < 0)
    {
        /* variable length array */
        typecode *= -1;  

        /* variable length arrays are 1-dimensional by default */
        naxis = 1;
        naxes[0] = (long) repeat;
    }
    else
    {
        /* get the dimensions of the image */
        ffgtdm(*fptr, colnum, 9, &naxis, naxes, status);
    }

    if (*status > 0)
    {
        ffpmsg("Error getting the dimensions of the image");
        return(*status);
    }

    /* determine BITPIX value for the image */
    if (typecode == TBYTE)
    {
        bitpix = BYTE_IMG;
        nbytes = (long) repeat;
    }
    else if (typecode == TSHORT)
    {
        bitpix = SHORT_IMG;
        nbytes = (long) repeat * 2;
    }
    else if (typecode == TLONG)
    {
        bitpix = LONG_IMG;
        nbytes = (long) repeat * 4;
    }
    else if (typecode == TFLOAT)
    {
        bitpix = FLOAT_IMG;
        nbytes = (long) repeat * 4;
    }
    else if (typecode == TDOUBLE)
    {
        bitpix = DOUBLE_IMG;
        nbytes = (long) repeat * 8;
    }
    else
    {
        ffpmsg("Error: the following image column has invalid datatype:");
        ffpmsg(colname);
        ffpmsg(tform);
        ffpmsg("Cannot open an image in a single row of this column.");
        return(*status = BAD_TFORM);
    }

    /* create new empty file to hold copy of the image */
    if (ffinit(&newptr, outfile, status) > 0)
    {
        ffpmsg("failed to create file for copy of image in table cell:");
        ffpmsg(outfile);
        return(*status);
    }

    if (ffcrim(newptr, bitpix, naxis, naxes, status) > 0)
    {
        ffpmsg("failed to write required primary array keywords in this file:");
        ffpmsg(outfile);
        return(*status);
    }

    /* write the BSCAL and BZERO keywords, if needed */
    if (scale != 1.0)
        ffpky(newptr, TDOUBLE, "BSCALE", &scale, "Array scaling factor",
              status);

    if (zero != 0.0)
        ffpky(newptr, TDOUBLE, "BZERO", &zero, "Array scaling zero point",
              status);

    ffkeyn("TUNIT", colnum, keyname, status);
    tstatus = 0;
    if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
    {
        strncpy(card, "BUNIT   ", 8);
        ffprec(newptr, card, status);
    }

    ffkeyn("TNULL", colnum, keyname, status);
    tstatus = 0;
    if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
    {
        strncpy(card, "BLANK   ", 8);
        ffprec(newptr, card, status);
    }

    /* convert the nominal WCS keywords, if present */
    strcpy(axisnum,"123456789");
    for (ii = 0; ii < naxis; ii++)
    {
      strcpy(root, "1CTYP");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CTYPE1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }

      strcpy(root, "1CUNI");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CUNIT1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }

      strcpy(root, "1CRPX");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CRPIX1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }

      strcpy(root, "1CRVL");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CRVAL1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }

      strcpy(root, "1CDLT");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CDELT1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }

      strcpy(root, "1CROT");
      root[0] = axisnum[ii];
      ffkeyn(root, colnum, keyname, status);
      tstatus = 0;
      if (ffgcrd(*fptr, keyname, card, &tstatus) == 0)
      {
        strncpy(card, "CROTA1  ", 8);
        card[5] = axisnum[ii];
        ffprec(newptr, card, status);
      }
    }

    /* copy all other relevant keywords */
    fits_copy_image_keywords(*fptr, newptr, status);

    /* add some HISTORY  */
    sprintf(card,"HISTORY  This image was copied from row %ld of column '%s',",
            rownum, colname);
    ffprec(newptr, card, status);

    /* finally, copy the data, one buffer size at a time */
    ffmbyt(*fptr, startpos, TRUE, status);
    firstbyte = 1; 
    while (nbytes && (*status <= 0) )
    {
     /* the upper limit on the number of bytes must match the declaration */
        ntodo = minvalue(30000L, nbytes);
        ffgbyt(*fptr, ntodo, buffer, status);
        ffptbb(newptr, 1, firstbyte, ntodo, buffer, status);
        nbytes    -= ntodo;
        firstbyte += ntodo;
    }

    /* close the original file and return ptr to the new image */
    ffclos(*fptr, status);

    *fptr = newptr; /* reset the pointer to the new table */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_copy_image_keywords(
           fitsfile *infptr,   /* I - pointer to input table */
           fitsfile *outfptr,  /* I - pointer to input table */
           int *status)
/*
     Copy relevant keywords from the table header into the newly created 
     primary array header.  Convert names of keywords where appropriate.
*/
{
    int nrec, nkeys, nmore;
    char rec[FLEN_CARD], *root;

    if (*status > 0)
        return(*status);

    ffghsp(infptr, &nkeys, &nmore, status);  /* get number of keywords */
    root = rec + 1;

    for (nrec = 9; nrec <= nkeys; nrec++)
    {     
        ffgrec(infptr, nrec, rec, status);

        if (*rec == 'T')
        {
            if (!strncmp(root, "FORM", 4) || !strncmp(root, "HEAP", 4) ||
                !strncmp(root, "TYPE", 4) || !strncmp(root, "SCAL", 4) ||
                !strncmp(root, "ZERO", 4) || !strncmp(root, "DISP", 4) ||
                !strncmp(root, "LMIN", 4) || !strncmp(root, "LMAX", 4) ||
                !strncmp(root, "DMIN", 4) || !strncmp(root, "DMAX", 4) ||
                !strncmp(root, "CTYP", 4) || !strncmp(root, "CRPX", 4) ||
                !strncmp(root, "CRVL", 4) || !strncmp(root, "CDLT", 4) ||
                !strncmp(root, "CROT", 4) || !strncmp(root, "CUNI", 4) ||
                !strncmp(root, "UNIT", 4) || !strncmp(root, "NULL", 4) ||
                !strncmp(root, "DIM" , 3) || !strncmp(root, "BCOL", 4) )

                /* will have to deal with the WCS keywords separately */
            {
               continue;   /* ignore these keywords */
            }
            else
            {
               ffprec(outfptr, rec, status); /* copy the keyword */
            }
        }
        else if (isdigit((int) *rec) )
        {
           if ( !strncmp(root, "CTYP", 4) || !strncmp(root, "CRPX", 4) ||
                !strncmp(root, "CRVL", 4) || !strncmp(root, "CDLT", 4) ||
                !strncmp(root, "CROT", 4) || !strncmp(root, "CUNI", 4) )

                /* will have to deal with the WCS keywords separately */
            {
               continue;   /* ignore these keywords */
            }
            else
            {
               ffprec(outfptr, rec, status); /* copy the keyword */
            }
        }
        else if (*rec == 'E' && *root == 'X')
        {
            if (!strncmp(root, "XTNAME", 6) || !strncmp(root, "XTVER", 5) ||
                !strncmp(root, "XTLEVEL", 7)  )
            {
               continue;
            }
            else
            {
               ffprec(outfptr, rec, status); /* copy the keyword */
            }
        }
        else
        {
           ffprec(outfptr, rec, status); /* copy the keyword */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_select_image_section(
           fitsfile **fptr,  /* IO - pointer to input image; on output it  */
                             /*      points to the new subimage */
           char *outfile,    /* I - name for output file        */
           char *expr,       /* I - Image section expression    */
           int *status)
{
  /*
     copies an image section from the input file to an output file
  */

    fitsfile *newptr;
    int ii, hdunum, naxis, bitpix, tstatus, anynull, nkey, numkeys;
    long naxes[9], smin, smax, sinc, fpixels[9], lpixels[9], incs[9];
    long outnaxes[9], outsize, buffsize, dummy[2];
    char *cptr, keyname[FLEN_KEYWORD], card[FLEN_CARD];
    double *buffer = 0, crpix, cdelt;

    /* create new empty file to hold the image section */
    if (ffinit(&newptr, outfile, status) > 0)
    {
        ffpmsg(
         "failed to create output file for image section:");
        ffpmsg(outfile);
        return(*status);
    }

    fits_get_hdu_num(*fptr, &hdunum);  /* current HDU number in input file */

    /* copy all preceding extensions to the output file */
    for (ii = 1; ii < hdunum; ii++)
    {
        fits_movabs_hdu(*fptr, ii, NULL, status);
        if (fits_copy_hdu(*fptr, newptr, 0, status) > 0)
        {
            ffclos(newptr, status);
            return(*status);
        }
    }

    /* move back to the original HDU position */
    fits_movabs_hdu(*fptr, hdunum, NULL, status);

    /* get the size of the input image */
    fits_get_img_type(*fptr, &bitpix, status);
    fits_get_img_dim(*fptr, &naxis, status);
    if (fits_get_img_size(*fptr, naxis, naxes, status) > 0)
    {
        ffclos(newptr, status);
        return(*status);
    }

    if (naxis < 1 || naxis > 9)
    {
        ffpmsg(
        "Input image either had NAXIS = 0 (NULL image) or has > 9 dimensions");
        ffclos(newptr, status);
        return(*status = BAD_NAXIS);
    }

    /* create output image with same size and type as the input image */
    /*  Will update the size later */
    fits_create_img(newptr, bitpix, naxis, naxes, status);

    /* copy all other non-structural keywords from the input to output file */
    fits_get_hdrspace(*fptr, &numkeys, NULL, status);

    for (nkey = 4; nkey <= numkeys; nkey++) /* skip the first few keywords */
    {
        fits_read_record(*fptr, nkey, card, status);

        if (fits_get_keyclass(card) > TYP_CMPRS_KEY)
        {
            /* write the record to the output file */
            fits_write_record(newptr, card, status);
        }
    }

    if (*status > 0)
    {
         ffpmsg("error copying header from input image to output image");
         return(*status);
    }

    /* parse the section specifier to get min, max, and inc for each axis */
    /* and the size of each output image axis */

    outsize = 1;
    cptr = expr;
    for (ii=0; ii < naxis; ii++)
    {

       if (fits_get_section_range(&cptr, &smin, &smax, &sinc, status) > 0)
       {
          ffpmsg("error parsing the following image section specifier:");
          ffpmsg(expr);
          ffclos(newptr, status);
          return(*status);
       }

       if (smax == 0)
          smax = naxes[ii];   /* use whole axis  by default */
       else if (smin == 0)
          smin = naxes[ii];   /* use inverted whole axis */

       if (smin > naxes[ii] || smax > naxes[ii])
       {
          ffpmsg("image section exceeds dimensions of input image:");
          ffpmsg(expr);
          ffclos(newptr, status);
          return(*status = BAD_NAXIS);
       }

       fpixels[ii] = smin;
       lpixels[ii] = smax;
       incs[ii] = sinc;

       if (smin <= smax)
           outnaxes[ii] = (smax - smin + sinc) / sinc;
       else
           outnaxes[ii] = (smin - smax + sinc) / sinc;

       outsize = outsize * outnaxes[ii];

       /* modify the NAXISn keyword */
       fits_make_keyn("NAXIS", ii + 1, keyname, status);
       fits_modify_key_lng(newptr, keyname, outnaxes[ii], NULL, status);

       /* modify the WCS keywords if necessary */

       if (fpixels[ii] != 1 || incs[ii] != 1)
       {
         /* read the CRPIXn keyword if it exists in the input file */
         fits_make_keyn("CRPIX", ii + 1, keyname, status);
         tstatus = 0;

         if (fits_read_key(*fptr, TDOUBLE, keyname, 
             &crpix, NULL, &tstatus) == 0)
         {
           /* calculate the new CRPIXn value */
           if (fpixels[ii] <= lpixels[ii])
             crpix = (crpix - (fpixels[ii] - 1.0) - .5) / incs[ii] + 0.5;
           else
             crpix = (fpixels[ii] - (crpix - 1.0) - .5) / incs[ii] + 0.5;
            
           /* modify the value in the output file */
           fits_modify_key_dbl(newptr, keyname, crpix, 15, NULL, status);

           if (incs[ii] != 1 || fpixels[ii] > lpixels[ii])
           {
             /* read the CDELTn keyword if it exists in the input file */
             fits_make_keyn("CDELT", ii + 1, keyname, status);
             tstatus = 0;

             if (fits_read_key(*fptr, TDOUBLE, keyname, 
                 &cdelt, NULL, &tstatus) == 0)
             {
               /* calculate the new CDELTn value */
               if (fpixels[ii] <= lpixels[ii])
                 cdelt = cdelt * incs[ii];
               else
                 cdelt = cdelt * (-incs[ii]);
              
               /* modify the value in the output file */
               fits_modify_key_dbl(newptr, keyname, cdelt, 15, NULL, status);
             }

             /* modify the CDi_j keywords if thet exist in the input file */

             fits_make_keyn("CD1_", ii + 1, keyname, status);
             tstatus = 0;
             if (fits_read_key(*fptr, TDOUBLE, keyname, 
               &cdelt, NULL, &tstatus) == 0)
             {
                 /* calculate the new CDi_j value */
                 if (fpixels[ii] <= lpixels[ii])
                   cdelt = cdelt * incs[ii];
                 else
                   cdelt = cdelt * (-incs[ii]);
              
                 /* modify the value in the output file */
                 fits_modify_key_dbl(newptr, keyname, cdelt, 15, NULL, status);
             }

             fits_make_keyn("CD2_", ii + 1, keyname, status);
             tstatus = 0;
             if (fits_read_key(*fptr, TDOUBLE, keyname, 
               &cdelt, NULL, &tstatus) == 0)
             {
                 /* calculate the new CDi_j value */
                 if (fpixels[ii] <= lpixels[ii])
                   cdelt = cdelt * incs[ii];
                 else
                   cdelt = cdelt * (-incs[ii]);
              
                 /* modify the value in the output file */
                 fits_modify_key_dbl(newptr, keyname, cdelt, 15, NULL, status);
             }
           }
         }
       }
    }  /* end of main NAXIS loop */

    if (ffrdef(newptr, status) > 0)  /* force the header to be scanned */
    {
        ffclos(newptr, status);
        return(*status);
    }

    /* write a dummy value to the last pixel in the output section */
    /* This will force memory to be allocated for the FITS files if it */
    /* is being written in memory, before we allocate some more memory */
    /* below.  Hopefully this leads to better memory management and */
    /* reduces the probability that the memory for the FITS file will have */
    /* to be reallocated to a new location later. */

    /* turn off any scaling of the pixel values */
    fits_set_bscale(*fptr,  1.0, 0.0, status);
    fits_set_bscale(newptr, 1.0, 0.0, status);

    dummy[0] = 0;
    if (fits_write_img(newptr, TLONG, outsize, 1, dummy, status) > 0)
    {
        ffpmsg("error trying to write dummy value to the last image pixel");
        ffclos(newptr, status);
        return(*status);
    }

    /* allocate memory for the entire image section */
    buffsize = (abs(bitpix) / 8) * outsize;

    buffer = (double *) malloc(buffsize);
    if (!buffer)
    {
        ffpmsg("error allocating memory for image section");
        ffclos(newptr, status);
        return(*status = MEMORY_ALLOCATION);
    }

    /* read the image section then write it to the output file */

    if (bitpix == 8)
    {
        ffgsvb(*fptr, 1, naxis, naxes, fpixels, lpixels, incs, 0,
            (unsigned char *) buffer, &anynull, status);

        ffpprb(newptr, 1, 1, outsize, (unsigned char *) buffer, status);
    }
    else if (bitpix == 16)
    {
        ffgsvi(*fptr, 1, naxis, naxes, fpixels, lpixels, incs, 0,
            (short *) buffer, &anynull, status);

        ffppri(newptr, 1, 1, outsize, (short *) buffer, status);
    }
    else if (bitpix == 32)
    {
        ffgsvk(*fptr, 1, naxis, naxes, fpixels, lpixels, incs, 0,
            (int *) buffer, &anynull, status);

        ffpprk(newptr, 1, 1, outsize, (int *) buffer, status);
    }
    else if (bitpix == -32)
    {
        ffgsve(*fptr, 1, naxis, naxes, fpixels, lpixels, incs, FLOATNULLVALUE,
            (float *) buffer, &anynull, status);

        ffppne(newptr, 1, 1, outsize, (float *) buffer, FLOATNULLVALUE, status);
    }
    else if (bitpix == -64)
    {
        ffgsvd(*fptr, 1, naxis, naxes, fpixels, lpixels, incs, DOUBLENULLVALUE,
             buffer, &anynull, status);

        ffppnd(newptr, 1, 1, outsize, buffer, DOUBLENULLVALUE,
               status);
    }

    free(buffer);  /* finished with the memory */

    if (*status > 0)
    {
        ffpmsg("error copying image section from input to output file");
        ffclos(newptr, status);
        return(*status);
    }

    /* copy any remaining HDUs to the output file */

    for (ii = hdunum + 1; 1; ii++)
    {
        if (fits_movabs_hdu(*fptr, ii, NULL, status) > 0)
            break;

        fits_copy_hdu(*fptr, newptr, 0, status);
    }

    if (*status == END_OF_FILE)   
        *status = 0;              /* got the expected EOF error; reset = 0  */
    else if (*status > 0)
    {
        ffclos(newptr, status);
        return(*status);
    }

    /* close the original file and return ptr to the new image */
    ffclos(*fptr, status);

    *fptr = newptr; /* reset the pointer to the new table */

    /* move back to the image subsection */
    fits_movabs_hdu(*fptr, hdunum, NULL, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_section_range(char **ptr, 
                   long *secmin,
                   long *secmax, 
                   long *incre,
                   int *status)
/*
   Parse the input image section specification string, returning 
   the  min, max and increment values.
   Typical string =   "1:512:2"  or "1:512"
*/
{
    int slen, isanumber;
    char token[FLEN_VALUE];

    if (*status > 0)
        return(*status);

    slen = fits_get_token(ptr, " ,:", token, &isanumber); /* get 1st token */

    if (*token == '*')  /* wild card means to use the whole range */
    {
       *secmin = 1;
       *secmax = 0;
    }
    else if (*token == '-' && *(token+1) == '*' )  /* invert the whole range */
    {
       *secmin = 0;
       *secmax = 1;
    }
    else
    {
      if (slen == 0 || !isanumber || **ptr != ':')
        return(*status = URL_PARSE_ERROR);   

      /* the token contains the min value */
      *secmin = atol(token);

      (*ptr)++;  /* skip the colon between the min and max values */
      slen = fits_get_token(ptr, " ,:", token, &isanumber); /* get token */

      if (slen == 0 || !isanumber)
        return(*status = URL_PARSE_ERROR);   

      /* the token contains the max value */
      *secmax = atol(token);
    }

    if (**ptr == ':')
    {
        (*ptr)++;  /* skip the colon between the max and incre values */
        slen = fits_get_token(ptr, " ,", token, &isanumber); /* get token */

        if (slen == 0 || !isanumber)
            return(*status = URL_PARSE_ERROR);   

        *incre = atol(token);
    }
    else
        *incre = 1;  /* default increment if none is supplied */

    if (**ptr == ',')
        (*ptr)++;

    while (**ptr == ' ')   /* skip any trailing blanks */
         (*ptr)++;

    if (*secmin < 0 || *secmax < 0 || *incre < 1)
        *status = URL_PARSE_ERROR;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffselect_table(
           fitsfile **fptr,  /* IO - pointer to input table; on output it  */
                             /*      points to the new selected rows table */
           char *outfile,    /* I - name for output file */
           char *expr,       /* I - Boolean expression    */
           int *status)
{
    fitsfile *newptr;
    int ii, hdunum;

    if (*outfile)
    {
      /* create new empty file in to hold the selected rows */
      if (ffinit(&newptr, outfile, status) > 0)
      {
        ffpmsg(
         "failed to create file for selected rows from input table");
        ffpmsg(outfile);
        return(*status);
      }

      fits_get_hdu_num(*fptr, &hdunum);  /* current HDU number in input file */

      /* copy all preceding extensions to the output file */
      for (ii = 1; ii < hdunum; ii++)
      {
        fits_movabs_hdu(*fptr, ii, NULL, status);
        if (fits_copy_hdu(*fptr, newptr, 0, status) > 0)
        {
            ffclos(newptr, status);
            return(*status);
        }
      }

      /* copy all the header keywords from the input to output file */
      fits_movabs_hdu(*fptr, hdunum, NULL, status);
      if (fits_copy_header(*fptr, newptr, status) > 0)
      {
        ffclos(newptr, status);
        return(*status);
      }

      /* set number of rows = 0 */
      fits_modify_key_lng(newptr, "NAXIS2", 0, NULL,status);
      (newptr->Fptr)->numrows = 0;
      (newptr->Fptr)->origrows = 0;

      if (ffrdef(newptr, status) > 0)  /* force the header to be scanned */
      {
        ffclos(newptr, status);
        return(*status);
      }
    }
    else
        newptr = *fptr;  /* will delete rows in place in the table */

    /* copy rows which satisfy the selection expression to the output table */
    /* or delete the nonqualifying rows if *fptr = newptr.                  */
    if (fits_select_rows(*fptr, newptr, expr, status) > 0)
    {
        if (*outfile)
            ffclos(newptr, status);

        return(*status);
    }

    if (*outfile)
    {
      /* copy any remaining HDUs to the output copy */

      for (ii = hdunum + 1; 1; ii++)
      {
        if (fits_movabs_hdu(*fptr, ii, NULL, status) > 0)
            break;

        fits_copy_hdu(*fptr, newptr, 0, status);
      }

      if (*status == END_OF_FILE)   
        *status = 0;              /* got the expected EOF error; reset = 0  */
      else if (*status > 0)
      {
        ffclos(newptr, status);
        return(*status);
      }

      /* close the original file and return ptr to the new image */
      ffclos(*fptr, status);

      *fptr = newptr; /* reset the pointer to the new table */

      /* move back to the selected table HDU */
      fits_movabs_hdu(*fptr, hdunum, NULL, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffinit(fitsfile **fptr,      /* O - FITS file pointer                   */
           const char *name,     /* I - name of file to create              */
           int *status)          /* IO - error status                       */
/*
  Create and initialize a new FITS file.
*/
{
    int driver, slen, clobber;
    char *url;
    char urltype[MAX_PREFIX_LEN], outfile[FLEN_FILENAME];
    char tmplfile[FLEN_FILENAME];
    int handle;

    if (*status > 0)
        return(*status);

    *fptr = 0;              /* initialize null file pointer */

    if (need_to_initialize)            /* this is called only once */
       *status = fits_init_cfitsio();

    if (*status > 0)
        return(*status);

    url = (char *) name;
    while (*url == ' ')  /* ignore leading spaces in the filename */
        url++;

    if (*url == '\0')
    {
        ffpmsg("Name of file to create is blank. (ffinit)");
        return(*status = FILE_NOT_CREATED);
    }

    /* check for clobber symbol, i.e,  overwrite existing file */
    if (*url == '!')
    {
        clobber = TRUE;
        url++;
    }
    else
        clobber = FALSE;

        /* parse the output file specification */
    ffourl(url, urltype, outfile, tmplfile, status);

    if (*status > 0)
    {
        ffpmsg("could not parse the output filename: (ffinit)");
        ffpmsg(url);
        return(*status);
    }

        /* find which driver corresponds to the urltype */
    *status = urltype2driver(urltype, &driver);

    if (*status)
    {
        ffpmsg("could not find driver for this file: (ffinit)");
        ffpmsg(url);
        return(*status);
    }

        /* delete pre-existing file, if asked to do so */
    if (clobber)
    {
        if (driverTable[driver].remove)
             (*driverTable[driver].remove)(outfile);
    }

        /* call appropriate driver to create the file */
    if (driverTable[driver].create)
    {
        *status = (*driverTable[driver].create)(outfile, &handle);
        if (*status)
        {
            ffpmsg("failed to create the following file: (ffinit)");
            ffpmsg(url);
            return(*status);
       }
    }
    else
    {
        ffpmsg("cannot create a new file of this type: (ffinit)");
        ffpmsg(url);
        return(*status = FILE_NOT_CREATED);
    }

        /* allocate fitsfile structure and initialize = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

    if (!(*fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        return(*status = MEMORY_ALLOCATION);
    }

        /* allocate FITSfile structure and initialize = 0 */
    (*fptr)->Fptr = (FITSfile *) calloc(1, sizeof(FITSfile));

    if (!((*fptr)->Fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for following file: (ffopen)");
        ffpmsg(url);
        free(*fptr);
        *fptr = 0;       
        return(*status = MEMORY_ALLOCATION);
    }

    slen = strlen(url) + 1;
    slen = maxvalue(slen, 32); /* reserve at least 32 chars */ 
    ((*fptr)->Fptr)->filename = (char *) malloc(slen); /* mem for file name */

    if ( !(((*fptr)->Fptr)->filename) )
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate memory for filename: (ffinit)");
        ffpmsg(url);
        free((*fptr)->Fptr);
        free(*fptr);
        *fptr = 0;              /* return null file pointer */
        return(*status = FILE_NOT_CREATED);
    }

        /* store the parameters describing the file */
    ((*fptr)->Fptr)->filehandle = handle;        /* store the file pointer */
    ((*fptr)->Fptr)->driver = driver;            /*  driver number         */
    strcpy(((*fptr)->Fptr)->filename, url);      /* full input filename    */
    ((*fptr)->Fptr)->filesize = 0;               /* physical file size     */
    ((*fptr)->Fptr)->logfilesize = 0;            /* logical file size      */
    ((*fptr)->Fptr)->writemode = 1;              /* read-write mode        */
    ((*fptr)->Fptr)->datastart = DATA_UNDEFINED; /* unknown start of data  */
    ((*fptr)->Fptr)->curbuf = -1;         /* undefined current IO buffer   */
    ((*fptr)->Fptr)->open_count = 1;      /* structure is currently used once */
    ((*fptr)->Fptr)->validcode = VALIDSTRUC; /* flag denoting valid structure */

    ffldrc(*fptr, 0, IGNORE_EOF, status);     /* initialize first record */

    /* if template file was given, use it to define structure of new file */
    if (tmplfile[0])
        ffoptplt(*fptr, tmplfile, status);

    return(*status);                       /* successful return */
}
/*--------------------------------------------------------------------------*/
/* ffimem == fits_create_memfile */

int ffimem(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           void **buffptr,       /* I - address of memory pointer           */
           size_t *buffsize,     /* I - size of buffer, in bytes            */
           size_t deltasize,     /* I - increment for future realloc's      */
           void *(*mem_realloc)(void *p, size_t newsize), /* function       */
           int *status)          /* IO - error status                       */

/*
  Create and initialize a new FITS file in memory
*/
{
    int driver, slen;
    char urltype[MAX_PREFIX_LEN];
    int handle;

    if (*status > 0)
        return(*status);

    *fptr = 0;              /* initialize null file pointer */

    if (need_to_initialize)            /* this is called only once */
       *status = fits_init_cfitsio();

    if (*status > 0)
        return(*status);

    strcpy(urltype, "memkeep://"); /* URL type for pre-existing memory file */

    *status = urltype2driver(urltype, &driver);

    if (*status > 0)
    {
        ffpmsg("could not find driver for pre-existing memory file: (ffimem)");
        return(*status);
    }

    /* call driver routine to "open" the memory file */
    *status =   mem_openmem( buffptr, buffsize, deltasize,
                            mem_realloc,  &handle);

    if (*status > 0)
    {
         ffpmsg("failed to open pre-existing memory file: (ffimem)");
         return(*status);
    }

        /* allocate fitsfile structure and initialize = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

    if (!(*fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for memory file: (ffimem)");
        return(*status = MEMORY_ALLOCATION);
    }

        /* allocate FITSfile structure and initialize = 0 */
    (*fptr)->Fptr = (FITSfile *) calloc(1, sizeof(FITSfile));

    if (!((*fptr)->Fptr))
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate structure for memory file: (ffimem)");
        free(*fptr);
        *fptr = 0;       
        return(*status = MEMORY_ALLOCATION);
    }

    slen = 32; /* reserve at least 32 chars */ 
    ((*fptr)->Fptr)->filename = (char *) malloc(slen); /* mem for file name */

    if ( !(((*fptr)->Fptr)->filename) )
    {
        (*driverTable[driver].close)(handle);  /* close the file */
        ffpmsg("failed to allocate memory for filename: (ffimem)");
        free((*fptr)->Fptr);
        free(*fptr);
        *fptr = 0;              /* return null file pointer */
        return(*status = MEMORY_ALLOCATION);
    }

        /* store the parameters describing the file */
    ((*fptr)->Fptr)->filehandle = handle;        /* file handle */
    ((*fptr)->Fptr)->driver = driver;            /* driver number */
    strcpy(((*fptr)->Fptr)->filename, "memfile"); /* dummy filename */
    ((*fptr)->Fptr)->filesize = *buffsize;        /* physical file size */
    ((*fptr)->Fptr)->logfilesize = *buffsize;     /* logical file size */
    ((*fptr)->Fptr)->writemode = 1;               /* read-write mode    */
    ((*fptr)->Fptr)->datastart = DATA_UNDEFINED;  /* unknown start of data */
    ((*fptr)->Fptr)->curbuf = -1;             /* undefined current IO buffer */
    ((*fptr)->Fptr)->open_count = 1;     /* structure is currently used once */
    ((*fptr)->Fptr)->validcode = VALIDSTRUC; /* flag denoting valid structure */

    ffldrc(*fptr, 0, IGNORE_EOF, status);     /* initialize first record */
    return(*status); 
}
/*--------------------------------------------------------------------------*/
int fits_init_cfitsio(void)
/*
  initialize anything that is required before using the CFITSIO routines
*/
{
    int status;

    union u_tag {
      short ival;
      char cval[2];
    } u;

    need_to_initialize = 0;

    /*   test for correct byteswapping.   */

    u.ival = 1;
    if  ((BYTESWAPPED && u.cval[0] != 1) ||
         (BYTESWAPPED == FALSE && u.cval[1] != 1) )
    {
      printf ("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf(" Byteswapping is not being done correctly on this system.\n");
      printf(" Check the MACHINE and BYTESWAPPED definitions in fitsio2.h\n");
      printf(" Please report this problem to the author at\n");
      printf("     pence@tetra.gsfc.nasa.gov\n");
      printf(  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(1);
    }

    /* register the standard I/O drivers that are always available */

    /*--------------------disk file driver-----------------------*/
    status = fits_register_driver("file://", 
            file_init,
            file_shutdown,
            file_setoptions,
            file_getoptions, 
            file_getversion,
	    file_checkfile,
            file_open,
            file_create,
#ifdef HAVE_FTRUNCATE
            file_truncate,
#else
            NULL,   /* no file truncate function */
#endif
            file_close,
            file_remove,
            file_size,
            file_flush,
            file_seek,
            file_read,
            file_write);

    if (status)
    {
        ffpmsg("failed to register the file:// driver (init_cfitsio)");
        return(status);
    }

    /*------------ output temporary memory file driver -----------------------*/
    status = fits_register_driver("mem://", 
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */
            NULL,            /* open function not allowed */
            mem_create, 
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);


    if (status)
    {
        ffpmsg("failed to register the mem:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------input pre-existing memory file driver------------------*/
    status = fits_register_driver("memkeep://", 
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */
            NULL,            /* file open driver function is not used */
            NULL,            /* create function not allowed */
            mem_truncate,
            mem_close_keep,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);


    if (status)
    {
        ffpmsg("failed to register the memkeep:// driver (init_cfitsio)");
        return(status);
    }

   /*-------------------stdin stream driver----------------------*/
   /*  the stdin stream is copied to memory then opened in memory */

    status = fits_register_driver("stdin://", 
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            stdin_checkfile, 
            stdin_open,
            NULL,            /* create function not allowed */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the stdin:// driver (init_cfitsio)");
        return(status);
    }

   /*-------------------stdin file stream driver----------------------*/
   /*  the stdin stream is copied to a disk file, then the disk file is opened */

    status = fits_register_driver("stdinfile://", 
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            stdin_open,
            NULL,            /* create function not allowed */
#ifdef HAVE_FTRUNCATE
            file_truncate,
#else
            NULL,   /* no file truncate function */
#endif
            file_close,
            file_remove,
            file_size,
            file_flush,
            file_seek,
            file_read,
            file_write);

    if (status)
    {
        ffpmsg("failed to register the stdin:// driver (init_cfitsio)");
        return(status);
    }


    /*-----------------------stdout stream driver------------------*/
    status = fits_register_driver("stdout://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            NULL,            /* open function not required */
            mem_create, 
            mem_truncate,
            stdout_close,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the stdout:// driver (init_cfitsio)");
        return(status);
    }

    /*------------------iraf disk file to memory driver -----------*/
    status = fits_register_driver("irafmem://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            mem_iraf_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the irafmem:// driver (init_cfitsio)");
        return(status);
    }

    /*------------------raw binary file to memory driver -----------*/
    status = fits_register_driver("rawfile://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            mem_rawfile_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the irafmem:// driver (init_cfitsio)");
        return(status);
    }

    /*------------------compressed disk file to memory driver -----------*/
    status = fits_register_driver("compress://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            mem_compress_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the compress:// driver (init_cfitsio)");
        return(status);
    }

    /*------------------compressed disk file to disk file driver -------*/
    status = fits_register_driver("compressfile://",
            file_init,
            file_shutdown,
            file_setoptions,
            file_getoptions, 
            file_getversion,
            NULL,            /* checkfile not needed */ 
            file_compress_open,
            file_create,
#ifdef HAVE_FTRUNCATE
            file_truncate,
#else
            NULL,   /* no file truncate function */
#endif
            file_close,
            file_remove,
            file_size,
            file_flush,
            file_seek,
            file_read,
            file_write);

    if (status)
    {
        ffpmsg("failed to register the compressfile:// driver (init_cfitsio)");
        return(status);
    }

    /* Register Optional drivers */

#ifdef HAVE_NET_SERVICES

    /*--------------------root driver-----------------------*/

    status = fits_register_driver("root://",
				  root_init,
				  root_shutdown,
				  root_setoptions,
				  root_getoptions, 
				  root_getversion,
				  NULL,            /* checkfile not needed */ 
				  root_open,
				  root_create,
				  NULL,  /* No truncate possible */
				  root_close,
				  NULL,  /* No remove possible */
				  root_size,  /* no size possible */
				  root_flush,
				  root_seek, /* Though will always succeed */
				  root_read,
				  root_write);

    if (status)
    {
        ffpmsg("failed to register the root:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------------http  driver-----------------------*/
    status = fits_register_driver("http://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            http_checkfile,
            http_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the http:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------------http file driver-----------------------*/

    status = fits_register_driver("httpfile://",
            file_init,
            file_shutdown,
            file_setoptions,
            file_getoptions, 
            file_getversion,
            NULL,            /* checkfile not needed */ 
            http_file_open,
            file_create,
#ifdef HAVE_FTRUNCATE
            file_truncate,
#else
            NULL,   /* no file truncate function */
#endif
            file_close,
            file_remove,
            file_size,
            file_flush,
            file_seek,
            file_read,
            file_write);

    if (status)
    {
        ffpmsg("failed to register the httpfile:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------------httpcompress file driver-----------------------*/

    status = fits_register_driver("httpcompress://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            http_compress_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the httpcompress:// driver (init_cfitsio)");
        return(status);
    }


    /*--------------------ftp driver-----------------------*/
    status = fits_register_driver("ftp://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            ftp_checkfile,
            ftp_open,
            NULL,            /* create function not required */
            mem_truncate,
            mem_close_free,
            NULL,            /* remove function not required */
            mem_size,
            NULL,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the ftp:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------------ftp file driver-----------------------*/
    status = fits_register_driver("ftpfile://",
            file_init,
            file_shutdown,
            file_setoptions,
            file_getoptions, 
            file_getversion,
            NULL,            /* checkfile not needed */ 
            ftp_file_open,
            file_create,
#ifdef HAVE_FTRUNCATE
            file_truncate,
#else
            NULL,   /* no file truncate function */
#endif
            file_close,
            file_remove,
            file_size,
            file_flush,
            file_seek,
            file_read,
            file_write);

    if (status)
    {
        ffpmsg("failed to register the ftpfile:// driver (init_cfitsio)");
        return(status);
    }

    /*--------------------ftp compressed file driver------------------*/
    status = fits_register_driver("ftpcompress://",
            mem_init,
            mem_shutdown,
            mem_setoptions,
            mem_getoptions, 
            mem_getversion,
            NULL,            /* checkfile not needed */ 
            ftp_compress_open,
            0,            /* create function not required */
            mem_truncate,
            mem_close_free,
            0,            /* remove function not required */
            mem_size,
            0,            /* flush function not required */
            mem_seek,
            mem_read,
            mem_write);

    if (status)
    {
        ffpmsg("failed to register the ftpcompress:// driver (init_cfitsio)");
        return(status);
    }
      /* === End of net drivers section === */  
#endif

/* ==================== SHARED MEMORY DRIVER SECTION ======================= */

#ifdef HAVE_SHMEM_SERVICES

    /*--------------------shared memory driver-----------------------*/
    status = fits_register_driver("shmem://", 
            smem_init,
            smem_shutdown,
            smem_setoptions,
            smem_getoptions, 
            smem_getversion,
            NULL,            /* checkfile not needed */ 
            smem_open,
            smem_create,
            NULL,            /* truncate file not supported yet */ 
            smem_close,
            smem_remove,
            smem_size,
            smem_flush,
            smem_seek,
            smem_read,
            smem_write );

    if (status)
    {
        ffpmsg("failed to register the shmem:// driver (init_cfitsio)");
        return(status);
    }

#endif

/* ==================== END OF SHARED MEMORY DRIVER SECTION ================ */



    return(status);
}
/*--------------------------------------------------------------------------*/
int fits_register_driver(char *prefix,
	int (*init)(void),
	int (*shutdown)(void),
	int (*setoptions)(int option),
	int (*getoptions)(int *options),
	int (*getversion)(int *version),
	int (*checkfile) (char *urltype, char *infile, char *outfile),
	int (*open)(char *filename, int rwmode, int *driverhandle),
	int (*create)(char *filename, int *driverhandle),
	int (*truncate)(int driverhandle, OFF_T filesize),
	int (*close)(int driverhandle),
	int (*fremove)(char *filename),
        int (*size)(int driverhandle, OFF_T *size),
	int (*flush)(int driverhandle),
	int (*seek)(int driverhandle, OFF_T offset),
	int (*read) (int driverhandle, void *buffer, long nbytes),
	int (*write)(int driverhandle, void *buffer, long nbytes) )
/*
  register all the functions needed to support an I/O driver
*/
{
    int status;

    if (no_of_drivers + 1 == MAX_DRIVERS)
        return(TOO_MANY_DRIVERS);

    if (prefix  == NULL)
        return(BAD_URL_PREFIX);
   

    if (init != NULL)		
    { 
        status = (*init)();
        if (status)
            return(status);
    }

    	/*  fill in data in table */
    strncpy(driverTable[no_of_drivers].prefix, prefix, MAX_PREFIX_LEN);
    driverTable[no_of_drivers].prefix[MAX_PREFIX_LEN - 1] = 0;
    driverTable[no_of_drivers].init = init;
    driverTable[no_of_drivers].shutdown = shutdown;
    driverTable[no_of_drivers].setoptions = setoptions;
    driverTable[no_of_drivers].getoptions = getoptions;
    driverTable[no_of_drivers].getversion = getversion;
    driverTable[no_of_drivers].checkfile = checkfile;
    driverTable[no_of_drivers].open = open;
    driverTable[no_of_drivers].create = create;
    driverTable[no_of_drivers].truncate = truncate;
    driverTable[no_of_drivers].close = close;
    driverTable[no_of_drivers].remove = fremove;
    driverTable[no_of_drivers].size = size;
    driverTable[no_of_drivers].flush = flush;
    driverTable[no_of_drivers].seek = seek;
    driverTable[no_of_drivers].read = read;
    driverTable[no_of_drivers].write = write;

    no_of_drivers++;      /* increment the number of drivers */
    return(0);
 }
/*--------------------------------------------------------------------------*/
int ffiurl(char *url, 
                    char *urltype,
                    char *infilex,
                    char *outfile, 
                    char *extspec,
                    char *rowfilterx,
                    char *binspec,
                    char *colspec,
                    int *status)
/*
   parse the input URL into its basic components.
*/

{ 
    int ii, jj, slen, infilelen, plus_ext = 0, collen;
    char *ptr1, *ptr2, *ptr3, *tmptr;

    /* must have temporary variable for these, in case inputs are NULL */
    char *infile;
    char *rowfilter;
    char *tmpstr;

    if (*status > 0)
        return(*status);

    if (infilex)
        *infilex  = '\0';
    if (rowfilterx)
        *rowfilterx = '\0';

    if (urltype)
        *urltype = '\0';
    if (outfile)
        *outfile = '\0';
    if (extspec)
        *extspec = '\0';
    if (binspec)
        *binspec = '\0';
    if (colspec)
        *colspec = '\0';

    slen = strlen(url);

    if (slen == 0)       /* blank filename ?? */
        return(*status);

    /* allocate memory for 3 strings, each as long as the input url */
    infile = (char *) malloc(3 * (slen + 1) );
    if (!infile)
       return(*status = MEMORY_ALLOCATION);

    rowfilter = &infile[slen + 1];
    tmpstr = &rowfilter[slen + 1];

    *infile = '\0';
    *rowfilter = '\0';
    *tmpstr = '\0';

    ptr1 = url;

    /* -------------------------------------------------------- */
    /*  get urltype (e.g., file://, ftp://, http://, etc.)  */
    /* --------------------------------------------------------- */

    if (*ptr1 == '-' && 
                     ( *(ptr1 +1) ==  0   || *(ptr1 +1) == ' ' || 
                       *(ptr1 +1) == '['  || *(ptr1 +1) == '(' ) )

         /* "-" means read file from stdin */
         /* also support "- ", "-[extname]" and '-(outfile.fits)"    */
         /* but exclude disk file names that begin with a minus sign */
         /* e.g., "-55d33m.fits"   */
    {
        if (urltype)
            strcat(urltype, "stdin://");
        ptr1++;
    }
    else
    {
        ptr2 = strstr(ptr1, "://");
        if (ptr2)                  /* copy the explicit urltype string */ 
        {
            if (urltype)
                 strncat(urltype, ptr1, ptr2 - ptr1 + 3);
            ptr1 = ptr2 + 3;
        }
        else if (!strncmp(ptr1, "ftp:", 4) )
        {                              /* the 2 //'s are optional */
            if (urltype)
                strcat(urltype, "ftp://");
            ptr1 += 4;
        }
        else if (!strncmp(ptr1, "http:", 5) )
        {                              /* the 2 //'s are optional */
            if (urltype)
                strcat(urltype, "http://");
            ptr1 += 5;
        }
        else if (!strncmp(ptr1, "mem:", 4) )
        {                              /* the 2 //'s are optional */
            if (urltype)
                strcat(urltype, "mem://");
            ptr1 += 4;
        }
        else if (!strncmp(ptr1, "shmem:", 6) )
        {                              /* the 2 //'s are optional */
            if (urltype)
                strcat(urltype, "shmem://");
            ptr1 += 6;
        }
        else if (!strncmp(ptr1, "file:", 5) )
        {                              /* the 2 //'s are optional */
            if (urltype)
                strcat(urltype, "file://");
            ptr1 += 5;
        }
        else                       /* assume file driver    */
        {
            if (urltype)
                strcat(urltype, "file://");
        }
    }


    /* ----------------------------------------------------------    
       Look for VMS style filenames like: 
            disk:[directory.subdirectory]filename.ext, or
                 [directory.subdirectory]filename.ext

       Check if the first character is a '[' and urltype != stdin
       or if there is a ':[' string in the remaining url string. If
       so, then need to move past this bracket character before
       search for the opening bracket of a filter specification.
     ----------------------------------------------------------- */

    tmptr = ptr1;
    if (*ptr1 == '[')
    {
      if (*url != '-') 
        tmptr = ptr1 + 1; /* this bracket encloses a VMS directory name */
    }
    else
    {
       tmptr = strstr(ptr1, ":[");
       if (tmptr) /* these 2 chars are part of the VMS disk and directory */
          tmptr += 2; 
       else
          tmptr = ptr1;
    }

    /* ------------------------ */
    /*  get the input file name */
    /* ------------------------ */

    ptr2 = strchr(tmptr, '(');   /* search for opening parenthesis ( */
    ptr3 = strchr(tmptr, '[');   /* search for opening bracket [ */

    if (ptr2 == ptr3)  /* simple case: no [ or ( in the file name */
    {
        strcat(infile, ptr1);
    }
    else if (!ptr3)     /* no bracket, so () enclose output file name */
    {
        strncat(infile, ptr1, ptr2 - ptr1);
        ptr2++;

        ptr1 = strchr(ptr2, ')' );   /* search for closing ) */
        if (!ptr1)
        {
            free(infile);
            return(*status = URL_PARSE_ERROR);  /* error, no closing ) */
        }

        if (outfile)
            strncat(outfile, ptr2, ptr1 - ptr2);
    }
    else if (ptr2 && (ptr2 < ptr3)) /* () enclose output name before bracket */
    {
        strncat(infile, ptr1, ptr2 - ptr1);
        ptr2++;

        ptr1 = strchr(ptr2, ')' );   /* search for closing ) */
        if (!ptr1)
        {
            free(infile);
            return(*status = URL_PARSE_ERROR);  /* error, no closing ) */
        }

        if (outfile)
            strncat(outfile, ptr2, ptr1 - ptr2);
    }
    else    /*   bracket comes first, so there is no output name */
    {
        strncat(infile, ptr1, ptr3 - ptr1);
    }

   /* strip off any trailing blanks in the names */
    slen = strlen(infile);
    for (ii = slen - 1; ii > 0; ii--)   
    {
            if (infile[ii] == ' ')
                infile[ii] = '\0';
            else
                break;
    }

    if (outfile)
    {
        slen = strlen(outfile);
        for (ii = slen - 1; ii > 0; ii--)   
        {
            if (outfile[ii] == ' ')
                outfile[ii] = '\0';
            else
                break;
        }
    }

    /* --------------------------------------------- */
    /* check if this is an IRAF file (.imh extension */
    /* --------------------------------------------- */

    if (strstr(infile, ".imh"))
    {
        if (urltype)
            strcpy(urltype, "irafmem://");
    }

    /* --------------------------------------------- */
    /* check if the 'filename+n' convention has been */
    /* used to specifiy which HDU number to open     */ 
    /* --------------------------------------------- */

    jj = strlen(infile);

    for (ii = jj - 1; ii >= 0; ii--)
    {
        if (infile[ii] == '+')    /* search backwards for '+' sign */
            break;
    }

    if (ii > 0 && (jj - ii) < 5)  /* limit extension numbers to 4 digits */
    {
        infilelen = ii;
        ii++;
        ptr1 = infile+ii;   /* pointer to start of sequence */

        for (; ii < jj; ii++)
        {
            if (!isdigit((int) infile[ii] ) ) /* are all the chars digits? */
                break;
        }

        if (ii == jj)      
        {
             /* yes, the '+n' convention was used.  Copy */
             /* the digits to the output extspec string. */
             plus_ext = 1;

             if (extspec)
                 strncpy(extspec, ptr1, jj - infilelen);

             infile[infilelen] = '\0'; /* delete the extension number */
        }
    }

    /* if '*' was given for the output name expand it to the root file name */
    if (outfile && outfile[0] == '*')
    {
        /* scan input name backwards to the first '/' character */
        for (ii = jj - 1; ii >= 0; ii--)
        {
            if (infile[ii] == '/')
            {
                strcpy(outfile, &infile[ii + 1]);
                break;
            }
        }
    }

    /* copy strings from local copy to the output */
    if (infilex)
        strcpy(infilex, infile);

    if (!ptr3)     /* no [ character in the input string? Then we are done. */
    {
        free(infile);
        return(*status);
    }

    /* ------------------------------------------- */
    /* see if [ extension specification ] is given */
    /* ------------------------------------------- */

    if (!plus_ext) /* extension no. not already specified?  Then      */
                   /* first brackets must enclose extension name or # */
                   /* or it encloses a image subsection specification */
                   /* or a raw binary image specifier */
    {
       ptr1 = ptr3 + 1;    /* pointer to first char after the [ */

       ptr2 = strchr(ptr1, ']' );   /* search for closing ] */
       if (!ptr2)
       {
            ffpmsg("input file URL is missing closing bracket ']'");
            free(infile);
            return(*status = URL_PARSE_ERROR);  /* error, no closing ] */
       }

       /* ---------------------------------------------- */
       /* First, test if this is a rawfile specifier     */
       /* which looks something like: '[ib512,512:2880]' */
       /* ---------------------------------------------- */

       /* first character must be b,i,j,d,r,f, or u  */
       if (*ptr1 == 'b' || *ptr1 == 'B' || *ptr1 == 'i' || *ptr1 == 'I' ||
           *ptr1 == 'j' || *ptr1 == 'J' || *ptr1 == 'd' || *ptr1 == 'D' ||
           *ptr1 == 'r' || *ptr1 == 'R' || *ptr1 == 'f' || *ptr1 == 'F' ||
           *ptr1 == 'u' || *ptr1 == 'U')
       {
           /* next optional character may be a b or l (for Big or Little) */
           ptr1++;
           if (*ptr1 == 'b' || *ptr1 == 'B' || *ptr1 == 'l' || *ptr1 == 'L')
              ptr1++;

           if (isdigit((int) *ptr1))  /* must have at least 1 digit */
           {
             while (isdigit((int) *ptr1))
              ptr1++;             /* skip over digits */

             if (*ptr1 == ',' || *ptr1 == ':' || *ptr1 == ']' )
             {
               /* OK, this looks like a rawfile specifier */

               if (urltype)
               {
                 if (strstr(urltype, "stdin") )
                   strcpy(urltype, "rawstdin://");
                 else
                   strcpy(urltype, "rawfile://");
               }

               /* append the raw array specifier to infilex */
               if (infilex)
               {
                 strcat(infilex, ptr3);
                 ptr1 = strchr(infilex, ']'); /* find the closing ] char */
                 if (ptr1)
                   *(ptr1 + 1) = '\0';  /* terminate string after the ] */
               }

               if (extspec)
                  strcpy(extspec, "0"); /* the 0 ext number is implicit */

               tmptr = strchr(ptr2 + 1, '[' ); /* search for another [ char */ 

               /* copy any remaining characters into rowfilterx  */
               if (tmptr && rowfilterx)
               {
                 strcat(rowfilterx, tmptr + 1);

                 tmptr = strchr(rowfilterx, ']' );   /* search for closing ] */
                 if (tmptr)
                   *tmptr = '\0'; /* overwrite the ] with null terminator */
               }

               free(infile);        /* finished parsing, so return */
               return(*status);
             }
           }   
       }

       /* -------------------------------------------------------- */
       /* Not a rawfile, so next, test if this is an image section */
       /* i.e., an integer followed by a ':' or a '*' or '-*'      */
       /* -------------------------------------------------------- */
 
       ptr1 = ptr3 + 1;    /* reset pointer to first char after the [ */
       tmptr = ptr1;

       while (*tmptr == ' ')
          tmptr++;   /* skip leading blanks */

       while (isdigit((int) *tmptr))
          tmptr++;             /* skip over leading digits */

       if (*tmptr == ':' || *tmptr == '*' || *tmptr == '-')
       {
           /* this is an image section specifier */
           if (extspec)
              strcpy(extspec, "0"); /* the 0 extension number is implicit */

           strcat(rowfilter, ptr3);
       }
       else
       {
        /* -------------------------------------------------------------- */
        /* not an image section or rawfile spec so must be extension spec */
        /* -------------------------------------------------------------- */

           /* copy the extension specification */
           if (extspec)
               strncat(extspec, ptr1, ptr2 - ptr1);

           /* copy any remaining chars to filter spec string */
           strcat(rowfilter, ptr2 + 1);
       }
    }
    else   
    {
      /* ------------------------------------------------------------------ */
      /* already have extension, so this must be a filter spec of some sort */
      /* ------------------------------------------------------------------ */

        strcat(rowfilter, ptr3);
    }

    /* strip off any trailing blanks from filter */
    slen = strlen(rowfilter);
    for (ii = slen - 1; ii > 0; ii--)   
    {
        if (rowfilter[ii] == ' ')
            rowfilter[ii] = '\0';
        else
            break;
    }

    if (!rowfilter[0])
    {
        free(infile);
        return(*status);      /* nothing left to parse */
    }

    /* ------------------------------------------------ */
    /* does the filter contain a binning specification? */
    /* ------------------------------------------------ */

    ptr1 = strstr(rowfilter, "[bin");      /* search for "[bin" */
    if (!ptr1)
        ptr1 = strstr(rowfilter, "[BIN");      /* search for "[BIN" */
    if (!ptr1)
        ptr1 = strstr(rowfilter, "[Bin");      /* search for "[Bin" */

    if (ptr1)
    {
      ptr2 = ptr1 + 4;     /* end of the '[bin' string */
      if (*ptr2 == 'b' || *ptr2 == 'i' || *ptr2 == 'j' ||
          *ptr2 == 'r' || *ptr2 == 'd')
         ptr2++;  /* skip the datatype code letter */


      if ( *ptr2 != ' ' && *ptr2 != ']')
        ptr1 = NULL;   /* bin string must be followed by space or ] */
    }

    if (ptr1)
    {
        /* found the binning string */
        if (binspec)
        {
            strcpy(binspec, ptr1 + 1);       
            ptr2 = strchr(binspec, ']');

            if (ptr2)      /* terminate the binning filter */
            {
                *ptr2 = '\0';

                if ( *(--ptr2) == ' ')  /* delete trailing spaces */
                    *ptr2 = '\0';
            }
            else
            {
                ffpmsg("input file URL is missing closing bracket ']'");
                ffpmsg(rowfilter);
                free(infile);
                return(*status = URL_PARSE_ERROR);  /* error, no closing ] */
            }
        }

        /* delete the binning spec from the row filter string */
        ptr2 = strchr(ptr1, ']');
        strcpy(tmpstr, ptr2+1);  /* copy any chars after the binspec */
        strcpy(ptr1, tmpstr);    /* overwrite binspec */
    }

    /* --------------------------------------------------------- */
    /* does the filter contain a column selection specification? */
    /* --------------------------------------------------------- */

    ptr1 = strstr(rowfilter, "[col ");
    if (!ptr1)
    {
        ptr1 = strstr(rowfilter, "[COL ");

        if (!ptr1)
            ptr1 = strstr(rowfilter, "[Col ");
    }

    if (ptr1)
    {           /* find the end of the column specifier */
        ptr2 = ptr1 + 5;
        while (*ptr2 != ']')
        {
            if (*ptr2 == '\0')
            {
                ffpmsg("input file URL is missing closing bracket ']'");
                free(infile);
                return(*status = URL_PARSE_ERROR);  /* error, no closing ] */
            }

            if (*ptr2 == '\'')  /* start of a literal string */
            {
                ptr2 = strchr(ptr2 + 1, '\'');  /* find closing quote */
                if (!ptr2)
                {
                  ffpmsg
          ("literal string in input file URL is missing closing single quote");
                  free(infile);
                  return(*status = URL_PARSE_ERROR);  /* error, no closing ] */
                }
            }

            if (*ptr2 == '[')  /* set of nested square brackets */
            {
                ptr2 = strchr(ptr2 + 1, ']');  /* find closing bracket */
                if (!ptr2)
                {
                  ffpmsg
          ("nested brackets in input file URL is missing closing bracket");
                  free(infile);
                  return(*status = URL_PARSE_ERROR);  /* error, no closing ] */
                }
            }

            ptr2++;  /* continue search for the closing bracket character */
        } 

        collen = ptr2 - ptr1 - 1;

        if (colspec)    /* copy the column specifier to output string */
        {
            strncpy(colspec, ptr1 + 1, collen);       
            colspec[collen] = '\0';
 
            while (colspec[--collen] == ' ')
            {
                colspec[collen] = '\0';  /* strip trailing blanks */
            }
        }

        /* delete the column selection spec from the row filter string */
        strcpy(tmpstr, ptr2 + 1);  /* copy any chars after the colspec */
        strcpy(ptr1, tmpstr);      /* overwrite binspec */
    }

    /* copy the remaining string to the rowfilter output... should only */
    /* contain a rowfilter expression of the form "[expr]"              */

    if (rowfilterx && rowfilter[0]) {
       ptr2 = rowfilter + strlen(rowfilter) - 1;
       if( rowfilter[0]=='[' && *ptr2==']' ) {
          *ptr2 = '\0';
          strcpy(rowfilterx, rowfilter+1);
       } else {
          ffpmsg("input file URL lacks valid row filter expression");
          *status = URL_PARSE_ERROR;
       }
    }

    free(infile);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffrtnm(char *url, 
           char *rootname,
           int *status)
/*
   parse the input URL, returning the root name (filetype://basename).
*/

{ 
    int ii, jj, slen, infilelen;
    char *ptr1, *ptr2, *ptr3;
    char urltype[MAX_PREFIX_LEN];
    char infile[FLEN_FILENAME];

    if (*status > 0)
        return(*status);

    ptr1 = url;
    *rootname = '\0';
    *urltype = '\0';
    *infile  = '\0';

    /*  get urltype (e.g., file://, ftp://, http://, etc.)  */
    if (*ptr1 == '-')        /* "-" means read file from stdin */
    {
        strcat(urltype, "-");
        ptr1++;
    }
    else
    {
        ptr2 = strstr(ptr1, "://");
        if (ptr2)                  /* copy the explicit urltype string */ 
        {
            strncat(urltype, ptr1, ptr2 - ptr1 + 3);
            ptr1 = ptr2 + 3;
        }
        else if (!strncmp(ptr1, "ftp:", 4) )
        {                              /* the 2 //'s are optional */
            strcat(urltype, "ftp://");
            ptr1 += 4;
        }
        else if (!strncmp(ptr1, "http:", 5) )
        {                              /* the 2 //'s are optional */
            strcat(urltype, "http://");
            ptr1 += 5;
        }
        else if (!strncmp(ptr1, "mem:", 4) )
        {                              /* the 2 //'s are optional */
            strcat(urltype, "mem://");
            ptr1 += 4;
        }
        else if (!strncmp(ptr1, "shmem:", 6) )
        {                              /* the 2 //'s are optional */
            strcat(urltype, "shmem://");
            ptr1 += 6;
        }
        else if (!strncmp(ptr1, "file:", 5) )
        {                              /* the 2 //'s are optional */
            ptr1 += 5;
        }

        /* else assume file driver    */
    }
 
       /*  get the input file name  */
    ptr2 = strchr(ptr1, '(');   /* search for opening parenthesis ( */
    ptr3 = strchr(ptr1, '[');   /* search for opening bracket [ */

    if (ptr2 == ptr3)  /* simple case: no [ or ( in the file name */
    {
        strcat(infile, ptr1);
    }
    else if (!ptr3)     /* no bracket, so () enclose output file name */
    {
        strncat(infile, ptr1, ptr2 - ptr1);
        ptr2++;

        ptr1 = strchr(ptr2, ')' );   /* search for closing ) */
        if (!ptr1)
            return(*status = URL_PARSE_ERROR);  /* error, no closing ) */

    }
    else if (ptr2 && (ptr2 < ptr3)) /* () enclose output name before bracket */
    {
        strncat(infile, ptr1, ptr2 - ptr1);
        ptr2++;

        ptr1 = strchr(ptr2, ')' );   /* search for closing ) */
        if (!ptr1)
            return(*status = URL_PARSE_ERROR);  /* error, no closing ) */
    }
    else    /*   bracket comes first, so there is no output name */
    {
        strncat(infile, ptr1, ptr3 - ptr1);
    }

       /* strip off any trailing blanks in the names */
    slen = strlen(infile);
    for (ii = slen - 1; ii > 0; ii--)   
    {
        if (infile[ii] == ' ')
            infile[ii] = '\0';
        else
            break;
    }

    /* --------------------------------------------- */
    /* check if the 'filename+n' convention has been */
    /* used to specifiy which HDU number to open     */ 
    /* --------------------------------------------- */

    jj = strlen(infile);

    for (ii = jj - 1; ii >= 0; ii--)
    {
        if (infile[ii] == '+')    /* search backwards for '+' sign */
            break;
    }

    if (ii != 0 && (jj - ii) < 5)  /* limit extension numbers to 4 digits */
    {
        infilelen = ii;
        ii++;

        for (; ii < jj; ii++)
        {
            if (!isdigit((int) infile[ii] ) ) /* are all the chars digits? */
                break;
        }

        if (ii == jj)      
        {
             /* yes, the '+n' convention was used.  */

             infile[infilelen] = '\0'; /* delete the extension number */
        }
    }

    strcat(rootname, urltype);  /* construct the root name */
    strcat(rootname, infile);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffourl(char *url,             /* I - full input URL   */
           char *urltype,          /* O - url type         */
           char *outfile,          /* O - base file name   */
           char *tpltfile,         /* O - template file name, if any */
           int *status)
/*
   parse the output URL into its basic components.
*/

{ 
    char *ptr1, *ptr2;

    if (*status > 0)
        return(*status);

    ptr1 = url;
    if (urltype)
      *urltype = '\0';
    if (outfile)
      *outfile = '\0';
    if (tpltfile)
      *tpltfile = '\0';

    /*  get urltype (e.g., file://, ftp://, http://, etc.)  */
    if (*ptr1 == '-' && 
                     ( *(ptr1 +1) ==  0   || *(ptr1 +1) == ' ' || 
                       *(ptr1 +1) == '['  || *(ptr1 +1) == '(' ) )

         /* "-" means write to stdout */
         /* also support "- ", "-[extname]" and '-(outfile.fits)"    */
         /* but exclude disk file names that begin with a minus sign */
         /* e.g., "-55d33m.fits"   */
    {
      if (urltype)
        strcpy(urltype, "stdout://");
    }
    else
    {
        ptr2 = strstr(ptr1, "://");
        if (ptr2)                  /* copy the explicit urltype string */ 
        {
          if (urltype)
            strncat(urltype, ptr1, ptr2 - ptr1 + 3);

            ptr1 = ptr2 + 3;
        }
        else                       /* assume file driver    */
        {
          if (urltype)
             strcat(urltype, "file://");
        }

        /* look for template file name, enclosed in parenthesis */
        ptr2 = strchr(ptr1, '('); 

        if (ptr2)   /* template file was specified  */
        {
            if (outfile)
                strncat(outfile, ptr1, ptr2 - ptr1);

            ptr2++;

            ptr1 = strchr(ptr2, ')' );   /* search for closing ) */

            if (!ptr1)
            {
                return(*status = URL_PARSE_ERROR);  /* error, no closing ) */
            }

            if (tpltfile)
                strncat(tpltfile, ptr2, ptr1 - ptr2);
        }
        else  /* no template file */
        {
            if (outfile)
                strcpy(outfile, ptr1);
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffexts(char *extspec, 
                       int *extnum, 
                       char *extname,
                       int *extvers,
                       int *hdutype,
                       char *imagecolname,
                       char *rowexpress,
                       int *status)
{
/*
   Parse the input extension specification string, returning either the
   extension number or the values of the EXTNAME, EXTVERS, and XTENSION
   keywords in desired extension. Also return the name of the column containing
   an image, and an expression to be used to determine which row to use,
   if present.
*/
    char *ptr1, *ptr2;
    int slen, nvals;
    char tmpname[FLEN_VALUE];

    *extnum = 0;
    *extname = '\0';
    *extvers = 0;
    *hdutype = ANY_HDU;
    *imagecolname = '\0';
    *rowexpress = '\0';

    if (*status > 0)
        return(*status);

    ptr1 = extspec;       /* pointer to first char */

    while (*ptr1 == ' ')  /* skip over any leading blanks */
        ptr1++;

    if (isdigit((int) *ptr1))  /* is the extension specification a number? */
    {
        sscanf(ptr1, "%d", extnum);
        if (*extnum < 0 || *extnum > 9999)
        {
            *extnum = 0;   /* this is not a reasonable extension number */
            ffpmsg("specified extension number is out of range:");
            ffpmsg(extspec);
            return(*status = URL_PARSE_ERROR); 
        }
    }
    else
    {
           /* not a number, so EXTNAME must be specified, followed by */
           /* optional EXTVERS and XTENSION  values */

           /* don't use space char as end indicator, because there */
           /* may be imbedded spaces in the EXTNAME value */
           slen = strcspn(ptr1, ",:;");   /* length of EXTNAME */
           strncat(extname, ptr1, slen);  /* EXTNAME value */

           /* now remove any trailing blanks */
           while (slen > 0 && *(extname + slen -1) == ' ')
           {
               *(extname + slen -1) = '\0';
               slen--;
           }

           ptr1 += slen;
           slen = strspn(ptr1, " ,:");  /* skip delimiter characters */
           ptr1 += slen;

           slen = strcspn(ptr1, " ,:;");   /* length of EXTVERS */
           if (slen)
           {
               nvals = sscanf(ptr1, "%d", extvers);  /* EXTVERS value */
               if (nvals != 1)
               {
                   ffpmsg("illegal EXTVER value in input URL:");
                   ffpmsg(extspec);
                   return(*status = URL_PARSE_ERROR);
               }

               ptr1 += slen;
               slen = strspn(ptr1, " ,:");  /* skip delimiter characters */
               ptr1 += slen;

               slen = strcspn(ptr1, ";");   /* length of HDUTYPE */
               if (slen)
               {
                 if (*ptr1 == 'b' || *ptr1 == 'B')
                     *hdutype = BINARY_TBL;  
                 else if (*ptr1 == 't' || *ptr1 == 'T' ||
                          *ptr1 == 'a' || *ptr1 == 'A')
                     *hdutype = ASCII_TBL;
                 else if (*ptr1 == 'i' || *ptr1 == 'I')
                     *hdutype = IMAGE_HDU;
                 else
                 {
                     ffpmsg("unknown type of HDU in input URL:");
                     ffpmsg(extspec);
                     return(*status = URL_PARSE_ERROR);
                 }
               }
           }
           else
           {
                strcpy(tmpname, extname);
                ffupch(tmpname);
                if (!strcmp(tmpname, "PRIMARY") || !strcmp(tmpname, "P") )
                    *extname = '\0';  /* return extnum = 0 */
           }
    }

    ptr1 = strchr(ptr1, ';');
    if (ptr1)
    {
        /* an image is to be opened; the image is contained in a single */
        /* cell of a binary table.  A column name and an expression to  */
        /* determine which row to use has been entered.                 */

        ptr1++;  /* skip over the ';' delimiter */
        while (*ptr1 == ' ')  /* skip over any leading blanks */
            ptr1++;

        ptr2 = strchr(ptr1, '(');
        if (!ptr2)
        {
            ffpmsg("illegal specification of image in table cell in input URL:");
            ffpmsg(" did not find a row expression enclosed in ( )");
            ffpmsg(extspec);
            return(*status = URL_PARSE_ERROR);
        }

        strncat(imagecolname, ptr1, ptr2 - ptr1); /* copy column name */

        ptr2++;  /* skip over the '(' delimiter */
        while (*ptr2 == ' ')  /* skip over any leading blanks */
            ptr2++;


        ptr1 = strchr(ptr2, ')');
        if (!ptr2)
        {
            ffpmsg("illegal specification of image in table cell in input URL:");
            ffpmsg(" missing closing ')' character in row expression");
            ffpmsg(extspec);
            return(*status = URL_PARSE_ERROR);
        }

        strncat(rowexpress, ptr2, ptr1 - ptr2); /* row expression */
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffextn(char *url,           /* I - input filename/URL  */
           int *extension_num,  /* O - returned extension number */
           int *status)
{
/*
   Parse the input url string and return the number of the extension that
   CFITSIO would automatically move to if CFITSIO were to open this input URL.
   The extension numbers are one's based, so 1 = the primary array, 2 = the
   first extension, etc.

   The extension number that gets returned is determined by the following 
   algorithm:

   1. If the input URL includes a binning specification (e.g.
   'myfile.fits[3][bin X,Y]') then the returned extension number
   will always = 1, since CFITSIO would create a temporary primary
   image on the fly in this case.  The same is true if an image
   within a single cell of a binary table is opened.

   2.  Else if the input URL specifies an extension number (e.g.,
   'myfile.fits[3]' or 'myfile.fits+3') then the specified extension
   number (+ 1) is returned.  

   3.  Else if the extension name is specified in brackets
   (e.g., this 'myfile.fits[EVENTS]') then the file will be opened and searched
   for the extension number.  If the input URL is '-'  (reading from the stdin
   file stream) this is not possible and an error will be returned.

   4.  Else if the URL does not specify an extension (e.g. 'myfile.fits') then
   a special extension number = -99 will be returned to signal that no
   extension was specified.  This feature is mainly for compatibility with
   existing FTOOLS software.  CFITSIO would open the primary array by default
   (extension_num = 1) in this case.

*/
    fitsfile *fptr;
    char urltype[20];
    char infile[FLEN_FILENAME];
    char outfile[FLEN_FILENAME]; 
    char extspec[FLEN_FILENAME];
    char extname[FLEN_FILENAME];
    char rowfilter[FLEN_FILENAME];
    char binspec[FLEN_FILENAME];
    char colspec[FLEN_FILENAME];
    char imagecolname[FLEN_VALUE], rowexpress[FLEN_FILENAME];
    char *cptr;
    int extnum, extvers, hdutype, tstatus = 0;

    if (*status > 0)
        return(*status);

    /*  parse the input URL into its basic components  */
    ffiurl(url, urltype, infile, outfile,
             extspec, rowfilter,binspec, colspec, status);

    if (*status > 0)
        return(*status);

    if (*binspec)   /* is there a binning specification? */
    {
       *extension_num = 1; /* a temporary primary array image is created */
       return(*status);
    }

    if (*extspec)   /* is an extension specified? */
    {
       ffexts(extspec, &extnum, 
         extname, &extvers, &hdutype, imagecolname, rowexpress, status);

      if (*status > 0)
        return(*status);

      if (*imagecolname)   /* is an image within a table cell being opened? */
      {
         *extension_num = 1; /* a temporary primary array image is created */
         return(*status);
      }

      if (*extname)
      {
         /* have to open the file to search for the extension name (curses!) */

         if (!strcmp(urltype, "stdin://"))
            /* opening stdin would destroying it! */
            return(*status = URL_PARSE_ERROR); 

         /* First, strip off any filtering specification */
         strcpy(infile, url);
         cptr = strchr(infile, ']');  /* locate the closing bracket */
         if (!cptr)
         {
             return(*status = URL_PARSE_ERROR);
         }
         else
         {
             cptr++;
             *cptr = '\0'; /* terminate URl after the extension spec */
         }

         if (ffopen(&fptr, infile, READONLY, status) > 0) /* open the file */
         {
            ffclos(fptr, &tstatus);
            return(*status);
         }

         ffghdn(fptr, &extnum);    /* where am I in the file? */
         *extension_num = extnum;
         ffclos(fptr, status);

         return(*status);
      }
      else
      {
         *extension_num = extnum + 1;  /* return the specified number (+ 1) */
         return(*status);
      }
    }
    else
    {
         *extension_num = -99;  /* no specific extension was specified */
                                /* defaults to primary array */
         return(*status);
    }
}
/*--------------------------------------------------------------------------*/

int ffurlt(fitsfile *fptr, char *urlType, int *status)
/*
   return the prefix string associated with the driver in use by the
   fitsfile pointer fptr
*/

{ 
  strcpy(urlType, driverTable[fptr->Fptr->driver].prefix);
  return(*status);
}

/*--------------------------------------------------------------------------*/
int ffimport_file( char *filename,   /* Text file to read                   */
                   char **contents,  /* Pointer to pointer to hold file     */
                   int *status )     /* CFITSIO error code                  */
/*
   Read and concatenate all the lines from the given text file.  User
   must free the pointer returned in contents.  Pointer is guaranteed
   to hold 2 characters more than the length of the text... allows the
   calling routine to append (or prepend) a newline (or quotes?) without
   reallocating memory.
*/
{
   int allocLen, totalLen, lineLen;
   char *lines,line[256];
   FILE *aFile;

   if( *status > 0 ) return( *status );

   totalLen =    0;
   allocLen = 1024;
   lines    = (char *)malloc( (2+allocLen)*sizeof(char) );
   if( !lines ) {
      ffpmsg("Couldn't allocate memory to hold ASCII file contents.");
      return(*status = MEMORY_ALLOCATION );
   }
   lines[0] = '\0';

   if( (aFile = fopen( filename, "r" ))==NULL ) {
      sprintf(line,"Could not open ASCII file %s.",filename);
      ffpmsg(line);
      free( lines );
      return(*status = FILE_NOT_OPENED);
   }

   while( fgets(line,256,aFile)!=NULL ) {
      lineLen = strlen(line);
      
      if( line[lineLen-1]=='\n' ) line[--lineLen] = '\0';

      if( totalLen+lineLen>=allocLen ) {
         lines = (char *)realloc(lines, (2+(allocLen+=256))*sizeof(char) );
         if( ! lines ) {
            ffpmsg("Couldn't allocate memory to hold ASCII file contents.");
            *status = MEMORY_ALLOCATION;
            break;
         }
      }
      strcpy( lines+totalLen, line );
      totalLen += lineLen;
   }
   fclose(aFile);

   *contents = lines;
   return( *status );
}

/*--------------------------------------------------------------------------*/
int fits_get_token(char **ptr, 
                   char *delimiter,
                   char *token,
                   int *isanumber)   /* O - is this token a number? */
/*
   parse off the next token, delimited by a character in 'delimiter',
   from the input ptr string;  increment *ptr to the end of the token.
   Returns the length of the token, not including the delimiter char;
*/
{
    int slen, ii;

    *token = '\0';

    while (**ptr == ' ')  /* skip over leading blanks */
        (*ptr)++;

    slen = strcspn(*ptr, delimiter);  /* length of next token */
    if (slen)
    {
        strncat(token, *ptr, slen);       /* copy token */

        (*ptr) += slen;                   /* skip over the token */

        if (isanumber)
        {
            *isanumber = 1;
 
            for (ii = 0; ii < slen; ii++)
            {
                if ( !isdigit((int) token[ii]) && token[ii] != '.' && 
                     token[ii] != '-')
                {
                    *isanumber = 0;
                    break;
                }
            }
        }
    }

    return(slen);
}
/*--------------------------------------------------------------------------*/
int urltype2driver(char *urltype, int *driver)
/*
   compare input URL with list of known drivers, returning the
   matching driver numberL.
*/

{ 
    int ii;

       /* find matching driver; search most recent drivers first */

    for (ii=no_of_drivers - 1; ii >= 0; ii--)
    {
        if (0 == strcmp(driverTable[ii].prefix, urltype))
        { 
             *driver = ii;
             return(0);
        }
    }

    return(NO_MATCHING_DRIVER);   
}
/*--------------------------------------------------------------------------*/
int ffclos(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  close the FITS file by completing the current HDU, flushing it to disk,
  then calling the system dependent routine to physically close the FITS file
*/   
{
    if (!fptr)
        return(*status = NULL_INPUT_PTR);
    else if ((fptr->Fptr)->validcode != VALIDSTRUC) /* check for magic value */
        return(*status = BAD_FILEPTR); 

    ffchdu(fptr, status);         /* close and flush the current HDU   */
    ((fptr->Fptr)->open_count)--;           /* decrement usage counter */

    if ((fptr->Fptr)->open_count == 0)  /* if no other files use structure */
    {
        ffflsh(fptr, TRUE, status);   /* flush and disassociate IO buffers */

        /* call driver function to actually close the file */
        if (
   (*driverTable[(fptr->Fptr)->driver].close)((fptr->Fptr)->filehandle) )
        {
            if (*status <= 0)
            {
              *status = FILE_NOT_CLOSED;  /* report if no previous error */

              ffpmsg("failed to close the following file: (ffclos)");
              ffpmsg((fptr->Fptr)->filename);
            }
        }

        free((fptr->Fptr)->filename);     /* free memory for the filename */
        (fptr->Fptr)->filename = 0;
        (fptr->Fptr)->validcode = 0; /* magic value to indicate invalid fptr */
        free(fptr->Fptr);         /* free memory for the FITS file structure */
        free(fptr);               /* free memory for the FITS file structure */
    }
    else
    {
        ffflsh(fptr, FALSE, status); /* flush but don't disassociate buffers */
        free(fptr);               /* free memory for the FITS file structure */
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdelt(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  close and DELETE the FITS file. 
*/
{
    char *basename;
    int slen;

    if (!fptr)
        return(*status = NULL_INPUT_PTR);
    else if ((fptr->Fptr)->validcode != VALIDSTRUC) /* check for magic value */
        return(*status = BAD_FILEPTR); 

    ffchdu(fptr, status);    /* close the current HDU, ignore any errors */
    ffflsh(fptr, TRUE, status);     /* flush and disassociate IO buffers */

        /* call driver function to actually close the file */
    if ( (*driverTable[(fptr->Fptr)->driver].close)((fptr->Fptr)->filehandle) )
    {
        if (*status <= 0)
        {
            *status = FILE_NOT_CLOSED;  /* report error if no previous error */

            ffpmsg("failed to close the following file: (ffdelt)");
            ffpmsg((fptr->Fptr)->filename);
        }
    }

    /* call driver function to actually delete the file */
    if ( (driverTable[(fptr->Fptr)->driver].remove) )
    {
        /* parse the input URL to get the base filename */
        slen = strlen((fptr->Fptr)->filename);
        basename = (char *) malloc(slen +1);
        if (!basename)
            return(*status = MEMORY_ALLOCATION);
    
        ffiurl((fptr->Fptr)->filename, NULL, basename, NULL, NULL, NULL, NULL,
               NULL, status);

       if ((*driverTable[(fptr->Fptr)->driver].remove)(basename))
        {
            ffpmsg("failed to delete the following file: (ffdelt)");
            ffpmsg((fptr->Fptr)->filename);
            if (!(*status))
                *status = FILE_NOT_CLOSED;
        }
        free(basename);
    }

    free((fptr->Fptr)->filename);     /* free memory for the filename */
    (fptr->Fptr)->filename = 0;
    (fptr->Fptr)->validcode = 0;      /* magic value to indicate invalid fptr */
    free(fptr->Fptr);              /* free memory for the FITS file structure */
    free(fptr);                    /* free memory for the FITS file structure */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftrun( fitsfile *fptr,    /* I - FITS file pointer           */
             OFF_T filesize,   /* I - size to truncate the file   */
             int *status)      /* O - error status                */
/*
  low level routine to truncate a file to a new smaller size.
*/
{
  if (driverTable[(fptr->Fptr)->driver].truncate)
  {
    ffflsh(fptr, FALSE, status);  /* flush all the buffers first */
    (fptr->Fptr)->filesize = filesize;
    (fptr->Fptr)->logfilesize = filesize;
    (fptr->Fptr)->bytepos = filesize;
    ffbfeof(fptr, status);   /* eliminate any buffers beyond current EOF */
    return (
     (*driverTable[(fptr->Fptr)->driver].truncate)((fptr->Fptr)->filehandle,
     filesize) );
  }
  else
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffflushx( FITSfile *fptr)     /* I - FITS file pointer                  */
/*
  low level routine to flush internal file buffers to the file.
*/
{
    if (driverTable[fptr->driver].flush)
        return ( (*driverTable[fptr->driver].flush)(fptr->filehandle) );
    else
        return(0);    /* no flush function defined for this driver */
}
/*--------------------------------------------------------------------------*/
int ffseek( FITSfile *fptr,   /* I - FITS file pointer              */
            OFF_T position)   /* I - byte position to seek to       */
/*
  low level routine to seek to a position in a file.
*/
{
    return( (*driverTable[fptr->driver].seek)(fptr->filehandle, position) );
}
/*--------------------------------------------------------------------------*/
int ffwrite( FITSfile *fptr,   /* I - FITS file pointer              */
             long nbytes,      /* I - number of bytes to write       */
             void *buffer,     /* I - buffer to write                */
             int *status)      /* O - error status                   */
/*
  low level routine to write bytes to a file.
*/
{
    if ( (*driverTable[fptr->driver].write)(fptr->filehandle, buffer, nbytes) )
        *status = WRITE_ERROR;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffread( FITSfile *fptr,   /* I - FITS file pointer              */
            long nbytes,      /* I - number of bytes to read        */
            void *buffer,     /* O - buffer to read into            */
            int *status)      /* O - error status                   */
/*
  low level routine to read bytes from a file.
*/
{
    int readstatus;

    readstatus = (*driverTable[fptr->driver].read)(fptr->filehandle, 
        buffer, nbytes);

    if (readstatus == END_OF_FILE)
        *status = END_OF_FILE;
    else if (readstatus > 0)
        *status = READ_ERROR;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftplt(fitsfile **fptr,      /* O - FITS file pointer                   */
           const char *filename, /* I - name of file to create              */
           const char *tempname, /* I - name of template file               */
           int *status)          /* IO - error status                       */
/*
  Create and initialize a new FITS file  based on a template file.
  Uses C fopen and fgets functions.
*/
{
    if (*status > 0)
        return(*status);

    if ( ffinit(fptr, filename, status) )  /* create empty file */
        return(*status);

    ffoptplt(*fptr, tempname, status);  /* open and use template */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffoptplt(fitsfile *fptr,      /* O - FITS file pointer                   */
            const char *tempname, /* I - name of template file               */
            int *status)          /* IO - error status                       */
/*
  open template file and use it to create new file
*/
{
    fitsfile *tptr;
    int tstatus = 0, nkeys, nadd, ii;
    char card[FLEN_CARD];

    if (*status > 0)
        return(*status);

    if (tempname == NULL || *tempname == '\0')     /* no template file? */
        return(*status);

    /* try opening template */
    ffopen(&tptr, (char *) tempname, READONLY, &tstatus); 

    if (tstatus)  /* not a FITS file, so treat it as an ASCII template */
    {
        ffxmsg(-2, card);  /* clear the  error message */

        fits_execute_template(fptr, (char *) tempname, status);

        ffmahd(fptr, 1, 0, status);   /* move back to the primary array */
        return(*status);
    }
    else  /* template is a valid FITS file */
    {
        while (*status <= 0)
        {
           ffghsp(tptr, &nkeys, &nadd, status); /* get no. of keywords */

           for (ii = 1; ii <= nkeys; ii++)   /* copy keywords */
           {
              ffgrec(tptr,  ii, card, status);
              ffprec(fptr, card, status);
           }

           ffmrhd(tptr, 1, 0, status); /* move to next HDU until error */
           ffcrhd(fptr, status);  /* create empty new HDU in output file */
        }

        if (*status == END_OF_FILE)
        {
           ffxmsg(-2, card);  /* clear the end of file error message */
           *status = 0;              /* expected error condition */
        }
        ffclos(tptr, status);       /* close the template file */
    }

    ffmahd(fptr, 1, 0, status);   /* move to the primary array */
    return(*status);
}
/*--------------------------------------------------------------------------*/
void ffrprt( FILE *stream, int status)
/* 
   Print out report of cfitsio error status and messages on the error stack.
   Uses C FILE stream.
*/
{
    char status_str[FLEN_STATUS], errmsg[FLEN_ERRMSG];
  
    if (status)
    {

      fits_get_errstatus(status, status_str);  /* get the error description */
      fprintf(stream, "\nFITSIO status = %d: %s\n", status, status_str);

      while ( fits_read_errmsg(errmsg) )  /* get error stack messages */
             fprintf(stream, "%s\n", errmsg);
    }
    return; 
}
