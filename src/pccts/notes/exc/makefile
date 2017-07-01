GFILE=extestc

.SUFFIXES:
.SUFFIXES:	.o .cpp .c .MM .h .g .i .dlg

#CCCP			= cccp
#CCCPOPT 		=-lang-c++

CCCP			= cpp
CCCPOPT			=-B

CCC			= cc
CPLUSPLUS_INCLUDE	= /CenterLine/clc++/sparc-solaris2/incl

TOKENS = tokens.h
DLG_FILE = parser.dlg
ERR = err
HDR_FILE =
MOD_FILE = mode.h
SCAN = scan

PCCTS = /pccts
ANTLR_H = $(PCCTS)/h
BIN = $(PCCTS)/bin
ANTLR = $(BIN)/antlr
DLG = $(BIN)/dlg

INCLUDE = -I$(CPLUSPLUS_INCLUDE) \
           -I$(ANTLR_H)

CFLAGS = $(INCLUDE) $(COTHER) -g
AFLAGS = -gs $(AOTHER)
DFLAGS = -C2 -i $(DOTHER)

MYFILES =  

SRC = $(GFILE).c
      $(MYFILES)

OBJ = $(GFILE).o \
      $(SCAN).o \
      $(ERR).o \
      $(MYFILES:.c=.o)

ANTLR_SPAWN = $(GFILE).c $(DLG_FILE) $(TOKENS) $(ERR).c

DLG_SPAWN = $(SCAN).c $(MOD_FILE)

CLEAN_FILES = $(DLG_SPAWN) $(ANTLR_SPAWN) *.o *.MM *.bak a.* \
		depend proxy.dat extestc *.tmp zzz_*

.cpp.o :
	$(CCC) -c $(CFLAGS) $<
 
.c.o :
	$(CCC) -c $(CFLAGS) $<
 
.cpp.i :
	$(CCC) -c -E $(CFLAGS) $< >$*.i
 
.c.i :
	$(CCC) -c -E $(CFLAGS) $< >$*.i
 
.cpp.MM :
	$(CCCP) $(CCCPTOP) -MM $(INCLUDE) $< $*.MM
 
.c.MM :
	$(CCCP) $(CCCPOPT) -MM $(INCLUDE) $< $*.MM
 
$(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

$(ANTLR_SPAWN) : $(GFILE).g
	@ echo "*********************************************************"
	@ echo "***                                                   ***"
	@ echo "***    Requires version 133MR7 (or later) of PCCTS    ***"
	@ echo "***                                                   ***"
	@ echo "*********************************************************"
	$(ANTLR) $(AFLAGS) $(GFILE).g

$(DLG_SPAWN) : $(DLG_FILE)
	$(DLG) $(DFLAGS) $(DLG_FILE) $(SCAN).c

$(ERR).o : $(ERR).c $(TOKENS)
	$(CC) -c $(CFLAGS) -o $(ERR).o $(ERR).c

depend : $(SRC:.c=.MM)
	cat *.MM > a.a
	sed -f ../fixup_cpp.sed <a.a >depend
	echo " "
	echo "Dependency list in file \"depend\""
	echo " "

oc :	
	#suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 593
	#suppress 768
	#load $(CFLAGS) $(LIB) $(SRC) $(ERR).c $(SCAN).c

verify :
	../compareOutput ./extestc

clean :
	- $(RM) $(CLEAN_FILES)

##############################################################################
extestc.o: extestc.c
extestc.o: ./tokens.h
extestc.o: ./mode.h
