####GFILE = $(g)
GFILE=flexex
PARSERCLASS = P

.SUFFIXES:
.SUFFIXES:	.o .cpp .c .MM .h .g .i .flex .dlg

#CCCP			= cccp
#CCCPOPT 		=-lang-c++

CCCP			= cpp
CCCPOPT			=-B

CCC			= CC
CPLUSPLUS_INCLUDE	= /CenterLine/clc++/sparc-solaris2/incl

FLEX	= flex
TOKENS = tokens.h
ERR = err
HDR_FILE =
PCCTS = /pccts
ANTLR_H = $(PCCTS)/h
BIN = $(PCCTS)/bin
ANTLR = $(BIN)/antlr

INCLUDE = -I$(CPLUSPLUS_INCLUDE) \
           -I$(ANTLR_H)

CFLAGS = $(INCLUDE) $(COTHER) -g
AFLAGS = -CC -gs $(AOTHER) -gx
DFLAGS = -C2 -i -CC $(DOTHER)
FLEXFLAGS = -s

MYFILES = flexLexer.cpp  example.cpp

SRC = $(GFILE).cpp \
      $(PARSERCLASS).cpp \
      $(ANTLR_H)/AParser.cpp \
      $(ANTLR_H)/ATokenBuffer.cpp \
      $(MYFILES)

OBJ = $(GFILE).o \
      $(PARSERCLASS).o \
      AParser.o \
      ATokenBuffer.o \
      $(MYFILES:.cpp=.o)

ANTLR_SPAWN = $(GFILE).cpp $(PARSERCLASS).cpp \
	      $(PARSERCLASS).h $(TOKENS)

CLEAN_FILES = $(ANTLR_SPAWN) example.cpp *.o *.MM *.bak a.* \
		depend proxy.dat flexex *.tmp zzz_* *.jou

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
	$(CCCP) $(CCCPOPT) -MM $(INCLUDE) $< $*.MM
 
.flex.cpp :
	echo " "
	echo "Ignore \"warning, -s option given but default rule can be matched\""
	echo " "
	$(FLEX)  $(FLEXFLAGS) $<
	mv lex.yy.c $*.cpp

$(GFILE) : $(OBJ) $(SRC)
	$(CCC) -o $(GFILE) $(CFLAGS) $(OBJ)

$(ANTLR_SPAWN) : $(GFILE).g
	@ echo "*********************************************************"
	@ echo "***                                                   ***"
	@ echo "***    Requires version 133MR1 (or later) of PCCTS    ***"
	@ echo "***                                                   ***"
	@ echo "*********************************************************"
	$(ANTLR) $(AFLAGS) $(GFILE).g

depend : $(SRC:.cpp=.MM)
	cat *.MM $(ANTLR_H)/*.MM > a.a
	sed -f ../fixup_cpp.sed <a.a >depend
	echo " "
	echo "Dependency list in file \"depend\""
	echo " "

oc:	
	#suppress 70
	#suppress 42
	#suppress 41
	#suppress 68
	#suppress 768
	#setopt mem_trace 10
	#load $(CFLAGS) $(LIB) $(SRC)

AParser.o : $(ANTLR_H)/AParser.cpp
	$(CCC) -c $(CFLAGS) -o AParser.o $(ANTLR_H)/AParser.cpp
        
ATokenBuffer.o : $(ANTLR_H)/ATokenBuffer.cpp
	$(CCC) -c $(CFLAGS) -o ATokenBuffer.o $(ANTLR_H)/ATokenBuffer.cpp
        
verify : 
	../compareOutput ./$(GFILE)

clean :
	- $(RM) $(CLEAN_FILES)

###############################################################################
P.o: P.cpp
P.o: ./flexTokens.h
P.o: ./P.h
P.o: ./yyprotos.h
example.o: example.cpp
example.o: ./flexTokens.h
flexLexer.o: flexLexer.cpp
flexLexer.o: ./myToken.h
flexLexer.o: ./flexTokens.h
flexLexer.o: ./flexLexer.h
flexLexer.o: ./flexTokens.h
flexLexer.o: ./yyprotos.h
flexex.o: flexex.cpp
flexex.o: ./flexTokens.h
flexex.o: ./yyprotos.h
flexex.o: ./P.h
flexex.o: ./yyprotos.h
flexex.o: ./myToken.h
flexex.o: ./flexTokens.h
flexex.o: ./flexLexer.h
flexex.o: ./flexTokens.h
