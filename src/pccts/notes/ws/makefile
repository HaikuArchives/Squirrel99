###GFILE = $(g)
GFILE=wsex
PARSERCLASS = P

.SUFFIXES:
.SUFFIXES:	.o .cpp .c .MM .h .g .i .dlg

#CCCP			= cccp
#CCCPOPT 		=-lang-c++

CCCP			= cpp
CCCPOPT			=-B

CCC			= CC
CPLUSPLUS_INCLUDE	= /CenterLine/clc++/sparc-solaris2/incl

TOKENS = tokens.h
DLG_FILE = parser.dlg
ERR = err
HDR_FILE =
SCAN = DLGLexer
PCCTS = /pccts
ANTLR_H = $(PCCTS)/h
BIN = $(PCCTS)/bin
ANTLR = $(BIN)/antlr
DLG = $(BIN)/dlg

INCLUDE = -I$(CPLUSPLUS_INCLUDE) \
           -I$(ANTLR_H)

CFLAGS = $(INCLUDE) $(COTHER) -g
OCFLAGS = $(INCLUDE) $(COTHER) -g
AFLAGS = -CC -gt -gs $(AOTHER)
DFLAGS = -C2 -i -CC $(DOTHER)

MYFILES =   mySyn.cpp wsAST.cpp wsToken.cpp myPanic.cpp \
      wsDLG.cpp wstabadjust.cpp noleakAST.cpp simpleToken.cpp

SRC = $(GFILE).cpp \
      $(PARSERCLASS).cpp \
      $(ANTLR_H)/AParser.cpp \
      $(ANTLR_H)/DLexerBase.cpp \
      $(ANTLR_H)/ASTBase.cpp \
      $(ANTLR_H)/PCCTSAST.cpp \
      $(ANTLR_H)/ATokenBuffer.cpp \
      $(SCAN).cpp \
      $(MYFILES)

OBJ = $(GFILE).o \
      $(PARSERCLASS).o \
      AParser.o \
      DLexerBase.o \
      ASTBase.o \
      PCCTSAST.o \
      ATokenBuffer.o \
      $(SCAN).o \
      $(MYFILES:.cpp=.o)

ANTLR_SPAWN = $(GFILE).cpp $(PARSERCLASS).cpp \
	      $(PARSERCLASS).h $(DLG_FILE) $(TOKENS)

DLG_SPAWN = $(SCAN).cpp $(SCAN).h

CLEAN_FILES = *.o *.MM *.bak a.* depend proxy.dat wsex *.tmp zzz_* \
		$(ANTLR_SPAWN) $(DLG_SPAWN)

.cpp.o :
	$(CCC) -c $(CFLAGS) $<
 
.c.o :
	$(CCC) -c $(CFLAGS) $<
 
.cpp.i :
	$(CCC) -c -E $(CFLAGS) $< >$*.i
 
.c.i :
	$(CCC) -c -E $(CFLAGS) $< >$*.i
 
.cpp.MM :
	$(CCCP) $(CCCPOPT) -MM $(INCLUDE) $< $*.MM
 
.c.MM :
	$(CCCP) $(CCPOPT) -MM $(INCLUDE) $< $*.MM
 
$(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

$(ANTLR_SPAWN) : $(GFILE).g
	@ echo "*********************************************************"
	@ echo "***                                                   ***"
	@ echo "***    Requires version 133MR11 (or later) of PCCTS   ***"
	@ echo "***                                                   ***"
	@ echo "**********************************************************"
	$(ANTLR) $(AFLAGS) $(GFILE).g

$(DLG_SPAWN) : $(DLG_FILE)
	$(DLG) $(DFLAGS) $(DLG_FILE)

depend : $(SRC:.cpp=.MM)
	cat *.MM $(ANTLR_H)/*.MM > a.a
	sed -f ../fixup_cpp.sed <a.a >depend
	echo " "
	echo "Dependency list in file \"depend\""
	echo " "

AParser.o : $(ANTLR_H)/AParser.cpp
	$(CCC) -c $(CFLAGS) -o AParser.o $(ANTLR_H)/AParser.cpp
        
ATokenBuffer.o : $(ANTLR_H)/ATokenBuffer.cpp
	$(CCC) -c $(CFLAGS) -o ATokenBuffer.o $(ANTLR_H)/ATokenBuffer.cpp
        
DLexerBase.o : $(ANTLR_H)/DLexerBase.cpp
	$(CCC) -c $(CFLAGS) -o DLexerBase.o $(ANTLR_H)/DLexerBase.cpp
        
ASTBase.o : $(ANTLR_H)/ASTBase.cpp
	$(CCC) -c $(CFLAGS) -o ASTBase.o $(ANTLR_H)/ASTBase.cpp

PCCTSAST.o : $(ANTLR_H)/PCCTSAST.cpp
	$(CCC) -c $(CFLAGS) -o PCCTSAST.o $(ANTLR_H)/PCCTSAST.cpp
        
oc :    #suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 768
	#setopt mem_trace 10
	#load $(OCFLAGS) $(LIB) $(SRC)

verify :
	../compareOutput ./$(GFILE)

clean :
	- $(RM) $(CLEAN_FILES)

#############################################################################
DLGLexer.o: DLGLexer.cpp
DLGLexer.o: ./tokens.h
DLGLexer.o: ./DLGLexer.h
P.o: P.cpp
P.o: ./tokens.h
P.o: ./P.h
myPanic.o: myPanic.cpp
mySyn.o: mySyn.cpp
mySyn.o: ./tokens.h
mySyn.o: ./wsToken.h
mySyn.o: ./tokens.h
mySyn.o: ./P.h
noleakAST.o: noleakAST.cpp
noleakAST.o: ./noleakAST.h
simpleToken.o: simpleToken.cpp
simpleToken.o: ./simpleToken.h
simpleToken.o: ./tokens.h
wsAST.o: wsAST.cpp
wsAST.o: ./wsAST.h
wsAST.o: ./tokens.h
wsAST.o: ./noleakAST.h
wsAST.o: ./wsToken.h
wsAST.o: ./tokens.h
wsAST.o: ./P.h
wsDLG.o: wsDLG.cpp
wsDLG.o: ./wsToken.h
wsDLG.o: ./tokens.h
wsDLG.o: ./wsDLG.h
wsDLG.o: ./tokens.h
wsDLG.o: ./DLGLexer.h
wsToken.o: wsToken.cpp
wsToken.o: ./wsToken.h
wsToken.o: ./tokens.h
wsToken.o: ./myPanic.h
wsToken.o: ./P.h
wsex.o: wsex.cpp
wsex.o: ./tokens.h
wsex.o: ./P.h
wsex.o: ./wsDLG.h
wsex.o: ./tokens.h
wsex.o: ./DLGLexer.h
wsex.o: ./wsToken.h
wsex.o: ./tokens.h
wsex.o: ./myPanic.h
wsex.o: ./wsAST.h
wsex.o: ./tokens.h
wsex.o: ./noleakAST.h
wstabadjust.o: wstabadjust.cpp
wstabadjust.o: ./tokens.h
wstabadjust.o: ./DLGLexer.h
