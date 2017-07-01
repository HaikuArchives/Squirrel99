#
# PCCTS makefile for: SQI-grammar.g
#
# Created from: genmk -CC -class SQIParser -project Squirrel SQI-grammar.g
#
# PCCTS release 1.33MR10
# Project: Squirrel
# C++ output
# DLG scanner
# ANTLR-defined token types
#
TOKENS = tokens.h
#
# The following filenames must be consistent with ANTLR/DLG flags
OPT = -O3
DLG_FILE = parser.dlg
ERR = err
HDR_FILE =
SCAN = DLGLexer
PCCTS = ../../pccts
ANTLR_H = $(PCCTS)/h
BIN = $(PCCTS)/bin
ANTLR = $(BIN)/antlr
DLG = $(BIN)/dlg
CFLAGS = $(OPT) -I. -I$(ANTLR_H) $(COTHER)
AFLAGS = -CC -mrhoist off $(AOTHER)
DFLAGS = -C2 -i -CC $(DOTHER)
GRM = SQI-grammar.g
SRC = SQI-grammar.cpp \
      SQIParser.cpp \
      $(ANTLR_H)/AParser.cpp $(ANTLR_H)/DLexerBase.cpp $(ANTLR_H)/ATokenBuffer.cpp $(SCAN).cpp
OBJ = $(OBJDIR)/SQI-grammar.o \
      $(OBJDIR)/SQIParser.o \
       $(OBJDIR)/AParser.o $(OBJDIR)/DLexerBase.o $(OBJDIR)/ATokenBuffer.o \
	$(OBJDIR)/NestedTokenStream.o $(OBJDIR)/fileToken.o $(OBJDIR)/noleakTok.o \
	$(OBJDIR)/myPanic.o $(SCAN).o
ANTLR_SPAWN = SQI-grammar.cpp SQIParser.cpp \
              SQIParser.h $(DLG_FILE) $(TOKENS)
DLG_SPAWN = $(SCAN).cpp $(SCAN).h
CCC=g++


OBJDIR = ./"(Objects.SQILib)"
ADDDIR = ./AddOns
HDIR   = ./Include

#SQIOBJ =  $(OBJDIR)/SQI-heap.o $(OBJDIR)/SQI-object.o $(OBJDIR)/SQI-string.o $(OBJDIR)/SQI-keyword.o $(OBJDIR)/SQI-number.o \
#	 $(OBJDIR)/SQI-list.o $(OBJDIR)/SQI-exception.o $(OBJDIR)/SQI-varmap.o $(OBJDIR)/SQI-node.o $(OBJDIR)/SQI-nbuiltin.o \
#	 $(OBJDIR)/SQI-nufunc.o $(OBJDIR)/SQI-ncontrol.o $(OBJDIR)/SQI-nvarref.o $(OBJDIR)/SQI-nblock.o $(OBJDIR)/SQI-nexpr.o \
#	 $(OBJDIR)/SQI-builtin.o $(OBJDIR)/SQI-interp.o $(OBJDIR)/SQI-squirrel.o $(OBJDIR)/SQI-defunc.o $(OBJDIR)/SQI-ufunc.o \
#	 $(OBJDIR)/SQI-ntag.o $(OBJDIR)/SQI-ngoto.o $(OBJDIR)/SQI-tcast.o $(OBJ)

SQIOBJ = $(OBJ)

$(OBJDIR)/SQIlib.a : $(SQIOBJ)
	ar -r $(OBJDIR)/SQIlib.a $(SQIOBJ)

$(OBJDIR)/SQI-heap.o : SQI-heap.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-heap.cpp -o $(OBJDIR)/SQI-heap.o

$(OBJDIR)/SQI-varmap.o : SQI-varmap.cpp $(HDIR)/SQI-varmap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-varmap.cpp -o $(OBJDIR)/SQI-varmap.o

$(OBJDIR)/SQI-node.o : SQI-node.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-node.cpp -o $(OBJDIR)/SQI-node.o

$(OBJDIR)/SQI-nbuiltin.o : SQI-nbuiltin.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-nbuiltin.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-nbuiltin.cpp -o $(OBJDIR)/SQI-nbuiltin.o

$(OBJDIR)/SQI-nufunc.o : SQI-nufunc.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-nufunc.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-nufunc.cpp -o $(OBJDIR)/SQI-nufunc.o

$(OBJDIR)/SQI-nblock.o : SQI-nblock.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-nblock.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-nblock.cpp -o $(OBJDIR)/SQI-nblock.o

$(OBJDIR)/SQI-ncontrol.o : SQI-ncontrol.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-ncontrol.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-ncontrol.cpp -o $(OBJDIR)/SQI-ncontrol.o

$(OBJDIR)/SQI-nvarref.o : SQI-nvarref.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-nvarref.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-nvarref.cpp -o $(OBJDIR)/SQI-nvarref.o

$(OBJDIR)/SQI-nexpr.o : SQI-nexpr.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-nexpr.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-nexpr.cpp -o $(OBJDIR)/SQI-nexpr.o

$(OBJDIR)/SQI-ntag.o : SQI-ntag.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-ntag.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-ntag.cpp -o $(OBJDIR)/SQI-ntag.o

$(OBJDIR)/SQI-ngoto.o : SQI-ngoto.cpp $(HDIR)/SQI-node.h $(HDIR)/SQI-object.h $(HDIR)/SQI-ngoto.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-ngoto.cpp -o $(OBJDIR)/SQI-ngoto.o

$(OBJDIR)/SQI-object.o : SQI-object.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-object.cpp -o $(OBJDIR)/SQI-object.o

$(OBJDIR)/SQI-number.o : SQI-number.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-number.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-number.cpp -o $(OBJDIR)/SQI-number.o

$(OBJDIR)/SQI-string.o : SQI-string.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-string.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-string.cpp -o $(OBJDIR)/SQI-string.o

$(OBJDIR)/SQI-keyword.o : SQI-keyword.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-string.h $(HDIR)/SQI-keyword.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-keyword.cpp -o $(OBJDIR)/SQI-keyword.o

$(OBJDIR)/SQI-list.o : SQI-list.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-list.h $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-list.cpp -o $(OBJDIR)/SQI-list.o

$(OBJDIR)/SQI-builtin.o : SQI-builtin.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-exception.h $(HDIR)/SQI-builtin.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-builtin.cpp -o $(OBJDIR)/SQI-builtin.o

$(OBJDIR)/SQI-interp.o : SQI-interp.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-varmap.h $(HDIR)/SQI-builtin.h $(HDIR)/SQI-interp.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-interp.cpp -o $(OBJDIR)/SQI-interp.o

$(OBJDIR)/SQI-squirrel.o : SQI-squirrel.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-varmap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-squirrel.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-squirrel.cpp -o $(OBJDIR)/SQI-squirrel.o

$(OBJDIR)/SQI-defunc.o : SQI-defunc.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-varmap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-defunc.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-defunc.cpp -o $(OBJDIR)/SQI-defunc.o

$(OBJDIR)/SQI-ufunc.o : SQI-ufunc.cpp $(HDIR)/SQI-heap.h $(HDIR)/SQI-varmap.h $(HDIR)/SQI-object.h $(HDIR)/SQI-ufunc.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-ufunc.cpp -o $(OBJDIR)/SQI-ufunc.o

$(OBJDIR)/SQI-exception.o : SQI-exception.cpp $(HDIR)/SQI-exception.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-exception.cpp -o $(OBJDIR)/SQI-exception.o
	
$(OBJDIR)/SQI-tcast.o : SQI-tcast.cpp $(HDIR)/SQI-tcast.h $(HDIR)/SQI-object.h
	$(CCC) $(OPT) -I$(HDIR) -c SQI-tcast.cpp -o $(OBJDIR)/SQI-tcast.o
	
$(OBJDIR)/SQI-parser.o : SQI-parser.cpp $(HDIR)/SQI-parser.h $(HDIR)/SQI-object.h
	$(CCC) $(OPT) -I$(HDIR) -I$(ANTLR_H) -c SQI-parser.cpp -o $(OBJDIR)/SQI-parser.o

Squirrel : $(OBJ) $(SRC) $(OBJDIR)/Squirrel.o $(OBJDIR)/SQIlib.a $(ADDDIR)/objects/SQI-Builtin.a
	$(CCC) -o Squirrel $(CFLAGS) $(OBJDIR)/Squirrel.o $(OBJ) $(ADDDIR)/objects/SQI-Builtin.a $(OBJDIR)/SQIlib.a 

$(OBJDIR)/Squirrel.o : Squirrel.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(OBJDIR)/Squirrel.o Squirrel.cpp

$(OBJDIR)/SQI-grammar.o : $(TOKENS) $(SCAN).h SQI-grammar.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR)  -o $(OBJDIR)/SQI-grammar.o SQI-grammar.cpp

$(OBJDIR)/SQIParser.o : $(TOKENS) $(SCAN).h SQIParser.cpp SQIParser.h
	$(CCC) -c $(CFLAGS) -I$(HDIR)  -o $(OBJDIR)/SQIParser.o SQIParser.cpp

$(OBJDIR)/NestedTokenStream.o :  NestedTokenStream.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR)  -o  $(OBJDIR)/NestedTokenStream.o  NestedTokenStream.cpp

$(OBJDIR)/fileToken.o :  fileToken.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o  $(OBJDIR)/fileToken.o  fileToken.cpp

$(OBJDIR)/noleakTok.o :  noleakTok.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(OBJDIR)/noleakTok.o  noleakTok.cpp

$(OBJDIR)/myPanic.o :  myPanic.cpp
	$(CCC) -c $(CFLAGS)  -I$(HDIR)  -o $(OBJDIR)/myPanic.o  myPanic.cpp

$(SCAN).o : $(SCAN).cpp $(TOKENS)
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(SCAN).o $(SCAN).cpp

$(ANTLR_SPAWN) : $(GRM)
	$(ANTLR) $(AFLAGS) $(GRM)

$(DLG_SPAWN) : $(DLG_FILE)
	$(DLG) $(DFLAGS) $(DLG_FILE)

$(OBJDIR)/AParser.o : $(ANTLR_H)/AParser.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(OBJDIR)/AParser.o $(ANTLR_H)/AParser.cpp

$(OBJDIR)/ATokenBuffer.o : $(ANTLR_H)/ATokenBuffer.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(OBJDIR)/ATokenBuffer.o $(ANTLR_H)/ATokenBuffer.cpp

$(OBJDIR)/DLexerBase.o : $(ANTLR_H)/DLexerBase.cpp
	$(CCC) -c $(CFLAGS) -I$(HDIR) -o $(OBJDIR)/DLexerBase.o $(ANTLR_H)/DLexerBase.cpp

clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.a core Squirrel

scrub:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.a core Squirrel $(ANTLR_SPAWN) $(DLG_SPAWN)
