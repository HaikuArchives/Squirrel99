####GFILE = $(g)
GFILE=diagram
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
AFLAGS = -CC -gt -gs $(AOTHER) -prc on
DFLAGS = -C2 -CC $(DOTHER)

MYFILES =  myAST.cpp myToken.cpp myPanic.cpp myTokenBuffer.cpp \
		noleakAST.cpp simpleToken.cpp

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

CLEAN_FILES = $(DLG_SPAWN) $(ANTLR_SPAWN) *.o *.MM *.bak a.* \
		depend proxy.dat diagram *.tmp zzz_*

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
	$(CCCP) $(CCCPOPT)  $(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

$(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

$(ANTLR_SPAWN) : $(GFILE).g
	@ echo "********************************************************************"
	@ echo "***                                                              ***"
	@ echo "***         Requires version 133MR14 (or later) of PCCTS         ***"
	@ echo "***                                                              ***"
	@ echo "********************************************************************"
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
        
oc :
	#suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 768
	#setopt mem_trace 10
	#load $(CFLAGS) $(LIB) $(SRC)

verify :
	../compareOutput ./$(GFILE)

clean :
	- $(RM) $(CLEAN_FILES)

##############################################################################
DLGLexer.o: DLGLexer.cpp
DLGLexer.o: ./tokens.h
DLGLexer.o: ./P.h
DLGLexer.o: ./DLGLexer.h
P.o: P.cpp
P.o: ./tokens.h
P.o: ./P.h
diagram.o: diagram.cpp
diagram.o: ./tokens.h
diagram.o: ./P.h
diagram.o: ./DLGLexer.h
diagram.o: ./myToken.h
diagram.o: ./tokens.h
diagram.o: ./simpleToken.h
diagram.o: ./tokens.h
diagram.o: ./myAST.h
diagram.o: ./noleakAST.h
diagram.o: ./myToken.h
diagram.o: ./myPanic.h
diagram.o: ./myTokenBuffer.h
diagram.o: ./tokens.h
myAST.o: myAST.cpp
myAST.o: ./myAST.h
myAST.o: ./noleakAST.h
myAST.o: ./myToken.h
myAST.o: ./tokens.h
myAST.o: ./simpleToken.h
myAST.o: ./tokens.h
myAST.o: ./P.h
myPanic.o: myPanic.cpp
myToken.o: myToken.cpp
myToken.o: ./myToken.h
myToken.o: ./tokens.h
myToken.o: ./simpleToken.h
myToken.o: ./tokens.h
myToken.o: ./myPanic.h
myToken.o: ./P.h
myTokenBuffer.o: myTokenBuffer.cpp
myTokenBuffer.o: ./myToken.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./simpleToken.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./myTokenBuffer.h
myTokenBuffer.o: ./tokens.h
noleakAST.o: noleakAST.cpp
noleakAST.o: ./noleakAST.h
simpleToken.o: simpleToken.cpp
simpleToken.o: ./simpleToken.h
simpleToken.o: ./tokens.h
