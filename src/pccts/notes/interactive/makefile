####GFILE = $(g)
GFILE=cli
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
AFLAGS = -CC -gt -gs -ck 2 $(AOTHER)
DFLAGS = -C2 -i -CC $(DOTHER)

MYFILES =  mySyn.cpp myAST.cpp numToken.cpp myPanic.cpp myTokenBuffer.cpp \
		simpleToken.cpp noleakAST.cpp mathAST.cpp

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
		depend proxy.dat cli *.tmp zzz_*

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
	@ echo "***    Requires version 133MR14 (or later) of PCCTS   ***"
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
        
oc :	
	#suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 768
	#setopt mem_trace 10
	#load $(CFLAGS) $(LIB) $(SRC)

verify :
	../compareOutput ./cli

clean :
	- $(RM) $(CLEAN_FILES)

##############################################################################
DLGLexer.o: DLGLexer.cpp
DLGLexer.o: ./tokens.h
DLGLexer.o: ./DLGLexer.h
P.o: P.cpp
P.o: ./tokens.h
P.o: ./P.h
cli.o: cli.cpp
cli.o: ./tokens.h
cli.o: ./P.h
cli.o: ./myToken.h
cli.o: ./numToken.h
cli.o: ./tokens.h
cli.o: ./simpleToken.h
cli.o: ./tokens.h
cli.o: ./myAST.h
cli.o: ./noleakAST.h
cli.o: ./myToken.h
cli.o: ./mathAST.h
cli.o: ./myAST.h
cli.o: ./myPanic.h
cli.o: ./myTokenBuffer.h
cli.o: ./tokens.h
cli.o: ./DLGLexer.h
mathAST.o: mathAST.cpp
mathAST.o: ./mathAST.h
mathAST.o: ./myAST.h
mathAST.o: ./noleakAST.h
mathAST.o: ./myToken.h
mathAST.o: ./numToken.h
mathAST.o: ./tokens.h
mathAST.o: ./simpleToken.h
mathAST.o: ./tokens.h
myAST.o: myAST.cpp
myAST.o: ./myAST.h
myAST.o: ./noleakAST.h
myAST.o: ./myToken.h
myAST.o: ./numToken.h
myAST.o: ./tokens.h
myAST.o: ./simpleToken.h
myAST.o: ./tokens.h
myAST.o: ./P.h
myPanic.o: myPanic.cpp
mySyn.o: mySyn.cpp
mySyn.o: ./tokens.h
mySyn.o: ./myToken.h
mySyn.o: ./numToken.h
mySyn.o: ./tokens.h
mySyn.o: ./simpleToken.h
mySyn.o: ./tokens.h
mySyn.o: ./P.h
myTokenBuffer.o: myTokenBuffer.cpp
myTokenBuffer.o: ./myToken.h
myTokenBuffer.o: ./numToken.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./simpleToken.h
myTokenBuffer.o: ./tokens.h
myTokenBuffer.o: ./myTokenBuffer.h
myTokenBuffer.o: ./tokens.h
noleakAST.o: noleakAST.cpp
noleakAST.o: ./noleakAST.h
numToken.o: numToken.cpp
numToken.o: ./numToken.h
numToken.o: ./tokens.h
numToken.o: ./simpleToken.h
numToken.o: ./tokens.h
numToken.o: ./myPanic.h
numToken.o: ./P.h
simpleToken.o: simpleToken.cpp
simpleToken.o: ./simpleToken.h
simpleToken.o: ./tokens.h
