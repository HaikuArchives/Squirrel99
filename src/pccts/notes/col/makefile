####GFILE = $(g)
GFILE=colex
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
AFLAGS = -CC -gt -gs $(AOTHER)
DFLAGS = -C2 -i -CC $(DOTHER)

MYFILES =  mySyn.cpp myAST.cpp colToken.cpp myPanic.cpp myTokenBuffer.cpp \
		noleakTok.cpp noleakAST.cpp

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

$(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

ANTLR_SPAWN = $(GFILE).cpp $(PARSERCLASS).cpp \
	      $(PARSERCLASS).h $(DLG_FILE) $(TOKENS)

DLG_SPAWN = $(SCAN).cpp $(SCAN).h

CLEAN_FILES = $(DLG_SPAWN) $(ANTLR_SPAWN) *.o *.MM *.bak a.* \
		depend proxy.dat colex *.tmp zzz_*

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

$(ANTLR_SPAWN) : $(GFILE).g
	@ echo "*********************************************************"
	@ echo "***                                                   ***"
	@ echo "***    Requires version 133MR11 (or later) of PCCTS   ***"
	@ echo "***                                                   ***"
	@ echo "*********************************************************"
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
        
verify :
	../compareOutput ./$(GFILE)

oc :
	#suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 768
	#setopt mem_trace 10
	#load $(CFLAGS) $(LIB) $(SRC)

clean :
	- $(RM) $(CLEAN_FILES)

###############################################################################
DLGLexer.o: DLGLexer.cpp
DLGLexer.o: ./tokens.h
DLGLexer.o: ./DLGLexer.h
P.o: P.cpp
P.o: ./tokens.h
P.o: ./P.h
colToken.o: colToken.cpp
colToken.o: ./noleakTok.h
colToken.o: ./tokens.h
colToken.o: ./simpleToken.h
colToken.o: ./colToken.h
colToken.o: ./tokens.h
colToken.o: ./noleakTok.h
colToken.o: ./myPanic.h
colToken.o: ./P.h
colex.o: colex.cpp
colex.o: ./tokens.h
colex.o: ./P.h
colex.o: ./myToken.h
colex.o: ./colToken.h
colex.o: ./tokens.h
colex.o: ./noleakTok.h
colex.o: ./tokens.h
colex.o: ./simpleToken.h
colex.o: ./myAST.h
colex.o: ./noleakAST.h
colex.o: ./myToken.h
colex.o: ./myPanic.h
colex.o: ./myTokenBuffer.h
colex.o: ./tokens.h
colex.o: ./DLGLexer.h
myAST.o: myAST.cpp
myAST.o: ./myAST.h
myAST.o: ./noleakAST.h
myAST.o: ./myToken.h
myAST.o: ./colToken.h
myAST.o: ./tokens.h
myAST.o: ./noleakTok.h
myAST.o: ./tokens.h
myAST.o: ./simpleToken.h
myAST.o: ./P.h
myPanic.o: myPanic.cpp
mySyn.o: mySyn.cpp
mySyn.o: ./tokens.h
mySyn.o: ./myToken.h
mySyn.o: ./colToken.h
mySyn.o: ./tokens.h
mySyn.o: ./noleakTok.h
mySyn.o: ./tokens.h
mySyn.o: ./simpleToken.h
mySyn.o: ./P.h
myTokenBuffer.o: myTokenBuffer.cpp
myTokenBuffer.o: ./myToken.h
myTokenBuffer.o: ./colToken.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./noleakTok.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./simpleToken.h
myTokenBuffer.o: ./myTokenBuffer.h
myTokenBuffer.o: ./tokens.h
noleakAST.o: noleakAST.cpp
noleakAST.o: ./noleakAST.h
noleakTok.o: noleakTok.cpp
noleakTok.o: ./noleakTok.h
noleakTok.o: ./tokens.h
noleakTok.o: ./simpleToken.h
noleakTok.o: ./P.h
