#
# PCCTS makefile for: cstr.g
#
# Created from: /pccts/bin/genmk -project cstr cstr.g
#
# PCCTS release 1.30
# Project: cstr
# C output
# DLG scanner
# ANTLR-defined token types
#
TOKENS = tokens.h
#
# The following filenames must be consistent with ANTLR/DLG flags
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
CFLAGS = -I. -I$(ANTLR_H)
AFLAGS =
DFLAGS = -C2 -i
GRM = cstr.g
SRC = cstr.c $(SCAN).c $(ERR).c
OBJ = cstr.o $(SCAN).o $(ERR).o
ANTLR_SPAWN = cstr.c $(DLG_FILE) $(TOKENS) $(ERR).c
DLG_SPAWN = $(SCAN).c $(MOD_FILE)
#CC=cc

cstr : $(OBJ) $(SRC)
	$(CC) -o cstr $(CFLAGS) $(OBJ)

cstr.o : $(MOD_FILE) $(TOKENS) cstr.c
	$(CC) -c $(CFLAGS) -o cstr.o cstr.c

$(ERR).o : $(ERR).c $(TOKENS)
	$(CC) -c $(CFLAGS) -o $(ERR).o $(ERR).c

$(SCAN).o : $(SCAN).c $(TOKENS)
	$(CC) -c $(CFLAGS) -o $(SCAN).o $(SCAN).c

$(ANTLR_SPAWN) : $(GRM)
	$(ANTLR) $(AFLAGS) $(GRM)

$(DLG_SPAWN) : $(DLG_FILE)
	$(DLG) $(DFLAGS) $(DLG_FILE) $(SCAN).c

clean :
	rm -f *.o core cstr $(DLG_SPAWN) $(ANTLR_SPAWN) *.MM *.bak \
		depend proxy.dat cstr *.tmp zzz_* a.*

verify :
	../compareOutput ./cstr
