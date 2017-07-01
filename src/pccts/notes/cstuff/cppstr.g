/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	Copyright (c) 1995 John D. Mitchell -- All Rights Reserved

PROJECT:	PCCTS
MODULE:		Contrib
FILE:		cstr.g

AUTHOR:		John D. Mitchell, Jun 13, 1995
		johnm@alumni.EECS.Berkeley.EDU

LICENSE:
	Permission to use, copy, modify, and distribute this software
	and its documentation for any purpose, without fee, and
	without written agreement is hereby granted, provided that the
	above copyright notice and this license agreement appears in
	all copies of this software. 

	IN NO EVENT SHALL JOHN D. MITCHELL, ("THE AUTHOR"), BE LIABLE
	TO ANY PARTY FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
	CONSEQUENTIAL DAMAGES SUFFERED BY LICENSEE AS A RESULT OF
	USING, MODIFYING, OR DISTRIBUTING THIS SOFTWARE, ITS
	DOCUMENTATION, OR ITS  DERIVATIVES EVEN IF THE AUTHOR HAS BEEN
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	THE AUTHOR MAKES NO REPRESENTATIONS ABOUT THE SUTIABILITY OF
	THE SOFTWARE AND SPECIFICALLY DISCLAIMS ANY WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, EITHER EXPRESS OR IMPLIED 
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE, OR NON-INFRINGEMENT.  THE SOFTWARE PROVIDED HEREUNDER
	IS ON AN "AS IS" BASIS, AND THE AUTHOR HAS NO OBLIGATION TO
	PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
	MODIFICATIONS.

REVISION HISTORY:
	Name	Date		Description
	----	----		-----------
	JDM	95.06.13	Initial version.
	JDM	95.06.14	Added support for C style comments.
	JDM	95.06.14	Released v1.0a1
	THM	95.06.15	Changed printf statements
	JDM	95.06.15	Added adjacent string literal concatenation.
	JDM	95.06.16	Released v1.0a2.
	JDM	95.06.19	Incorporated THM's error handling changes
				into baseline.  Excellent!
	JDM	95.06.19	Released v1.0a3.
	JDM	95.06.20	Cleanup pass.
	JDM	95.06.21	Released v1.0.
	THM	95.06.22	C++ Mode version

DESCRIPTION:
	This file parses the ISO/ANSI Standard C characters, strings,
	and comments.

	Note clearly that this sample grammar does *NOT* deal with anything
	else.  Including things like trigraphs (since they are a function
	of the 'preprocessor' phases of translation).  Nor does this
	grammar deal with multi-byte characters nor strings containing
	multi-byte characters [these constructs are "exercises for the
	reader" as it were :-)]. 

	Also note that this grammar is written assuming you're using
	ANTLR's C mode output (*not* C++).

	Please refer to the ISO/ANSI C Language Standard if you believe
	this grammar to be in error.  Please cite chapter and verse in any
	correspondence to the author to back up your claim.   Arguments of
	the form '...but XX brand compiler accepts [pet shme here]...'
	shall be summarily disregarded. 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		Include Files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#lexaction <<

#include <stddef.h>			/* Standard C/C++.	*/
#include <stdlib.h>

>>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		Tokens
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
 * Eat whitespace.
 */
#token	"[\t\ ]+"	<< skip (); >>
#token	"[\r\n]"	<< skip (); newline(); >>

/*
 * Start of C-style comments.  Switch to LC_COMMENT lexical class and
 * continue... 
 */
#token	"/\*"		<< skip (); mode (LC_COMMENT); >>

#lexclass	LC_COMMENT

/*
 * End of the comment so switch back into STARTing lexical class.
 *
 * Note that nothing will be 'returned' as the token ID because
 * we're 'skipping' over the input.
 *
 * Note that this method of dealing with comments was lifted from
 * Tom Moog's PCCTS Notes for New Users of PCCTS.
 */
#token			"\*/"		<< skip (); mode (START);  >>
#token			"[\r\n]"	<< skip (); newline(); >>
#token			"~[]"		<< skip (); >>

/*
 * Switch back to the starting lexical class.
 */
#lexclass	START


/*
 * Note that if you want C++ style you can uncomment the following token
 * definition.
 *
 * C++ comments go until the end of line.
 */
/*
 #token	"//~[\n]*\n"	<< skip (); newline(); >>
 */


/*
 * Start of string literals.  Switch to LC_STRING lexical class and
 * continue...
 */
#token	DBLQUOTE	"\""	<< more (); mode (LC_STRING); >>

#lexclass	LC_STRING

/*
 * End of the string literal so switch back into STARTing lexical class.
 *
 * Note that STRING_CONST will be the 'returned' token ID and that
 * the token *will* contain the delimiting quotes.
 */
#token	STRING_CONST	"\""	<< mode (START); >>

/* Handle the various character escape sequences.	*/
#token	"\\\""		<< replchar ('\"'); more (); >>
#token	"\\'"		<< replchar ('\''); more (); >>
#token	"\\?"		<< replchar ('\?'); more (); >>
#token	"\\\\"		<< replchar ('\\'); more (); >>
#token	"\\a"		<< replchar ('\a'); more (); >>
#token	"\\b"		<< replchar ('\b'); more (); >>
#token	"\\f"		<< replchar ('\f'); more (); >>
#token	"\\n"		<< replchar ('\n'); more (); >>
#token	"\\r"		<< replchar ('\r'); more (); >>
#token	"\\t"		<< replchar ('\t'); more (); >>
#token	"\\v"		<< replchar ('\v'); more (); >>

/*
 * Handle the various numeric escape sequences.
 *
 * Note carefully that these numeric escape *sequences* are *not* of the 
 * same form as the C language numeric *constants*.
 *
 * There is no such thing as a binary numeric escape sequence.
 * Octal escape sequences are either 1, 2, or 3 octal digits exactly.
 * There is no such thing as a decimal escape sequence.
 * Hexadecimal escape sequences are begun with a leading \x and continue
 * until a non-hexadecimal character is found.
 */
#token	"\\[0-7][0-7][0-7]"	<< replchar ((char)
					strtol (_begexpr+1, NULL, 8));
				   more (); >>
#token	"\\[0-7][0-7]"		<< replchar ((char)
					strtol (_begexpr+1, NULL, 8));
				   more (); >>
#token	"\\[0-7]"		<< replchar ((char)
					strtol (_begexpr+1, NULL, 8));
				   more (); >>

#token	"\\x[0-9a-fA-F]+"	<< replchar ((char)
					strtol (_begexpr+2, NULL, 16));
				    more (); >>

/*
 * Match anything that doesn't get matched above and is illegal and switch
 * into a special 'error' token mode so that the grammar can deal with the
 * error and perform any cleanup, error handling, ignore it, etc.
 *
 * First, can't have imbedded raw newlines.  A strict reading of the
 * language standard just nukes the newline and does *not* restrict the
 * carriage return.  Hence, the dichtomy below.
 */
#token	"\r"		<< newline(); more (); >>
#token	"\n"		<< newline(); more ();
			   mode (LC_BAD_STRING_CONST); >>

/* Catch invalid hexadecimal escapes.	*/
#token	"\\x"		<< more (); mode (LC_BAD_STRING_CONST); >>

/* Catch the rest of the invalid escape sequences.	*/
#token	"\\"		<< more (); mode (LC_BAD_STRING_CONST); >>

/*
 * Suck up everything else.
 * 
 * Note that the items which stop the input consumption are the
 * prefixes of all of the REs in this lexclass.  This is because we
 * want to suck up as much as possible in this nice, fast lexer
 * construct but don't want to overrun any 'important' tokens.
 */
#token	"~[\"\\\n\r]+"		<< more (); >>


/*
 * Start of LC_BAD_STRING_CONST.
 *
 * Try to eat up the badly formed string literal so we can continue
 * gracefully.
 */
#lexclass	LC_BAD_STRING_CONST

#token	BAD_STRING_CONST	"\""		<< mode (START); >>
#token				"[\r\n]"	<< newline(); more (); >>
#token				"~[\r\n\"]+"	<< more (); >>

/*
 * Switch back to the starting lexical class.
 */
#lexclass	START


/*
 * Start of character constants.  Switch to LC_CHARS lexical class and
 * continue... 
 */
#token	SGLQUOTE	"'"	<< more (); mode (LC_CHARS); >>

#lexclass	LC_CHARS 

/*
 * Note clearly that character constants may *not* be empty in C (even
 * though C strings may).  So you can *not* have the naive:
 *
 * 	#token CHARACTER "'"	<< mode (START); >>
 *
 * to end the lexical class.
 *
 * The trick to end this character constant is to switch to a secondary
 * 'helper' lexical class after finding any valid character definition
 * which only looks for the ending tick (').
 */

/*
 * Handle the various character escape sequences.
 *
 * Note clearly the switch into the helper lexical class to force
 * the recognition of only a single character.
 */
#token	"\\\""		<< replchar ('\"');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\'"		<< replchar ('\'');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\?"		<< replchar ('\?');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\\\"		<< replchar ('\\');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\a"		<< replchar ('\a');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\b"		<< replchar ('\b');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\f"		<< replchar ('\f');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\n"		<< replchar ('\n');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\r"		<< replchar ('\r');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\t"		<< replchar ('\t');
			   more (); mode (LC_CHAR_DONE); >>
#token	"\\v"		<< replchar ('\v');
			   more (); mode (LC_CHAR_DONE); >>

/*
 * Handle the various numeric escape sequences.
 *
 * Note carefully that these numeric escape *sequences* are *not* of the 
 * same form as the C language numeric *constants*.
 *
 * There is no such thing as a binary numeric escape sequence.
 * Octal escape sequences are either 1, 2, or 3 octal digits exactly.
 * There is no such thing as a decimal escape sequence.
 * Hexadecimal escape sequences are begun with a leading \x and continue
 * until a non-hexadecimal character is found.
 */
#token	"\\[0-7][0-7][0-7]"	<< replchar ((char)
					strtol (_begexpr+1, NULL, 8));
				   more (); mode (LC_CHAR_DONE); >>
#token	"\\[0-7][0-7]"		<< replchar ((char) 
					strtol (_begexpr+1, NULL, 8));
				   more (); mode (LC_CHAR_DONE); >>
#token	"\\[0-7]"		<< replchar ((char) 
					strtol (_begexpr+1, NULL, 8));
				   more (); mode (LC_CHAR_DONE); >>

#token	"\\x[0-9a-fA-F]+"	<< replchar ((char)
					strtol (_begexpr+2, NULL, 16));
				   more (); mode (LC_CHAR_DONE); >>

/*
 * Match stuff that doesn't get matched above but which is illegal and
 * switch into a special 'error' token mode so that the grammar can deal
 * with the error and perform any cleanup, error handling, ignore it, etc.
 *
 * First, can't have imbedded raw newlines.  A strict reading of the
 * language standard just nukes the newline and does *not* restrict the
 * carriage return.  Hence, the dichtomy below.
 */
#token	"\r"		<< newline(); more ();
			   mode (LC_CHAR_DONE); >>
#token	"\n"		<< newline(); more ();
			   mode (LC_BAD_CHAR_CONST); >>

/* Can't have raw single quotes in character constants.	*/
#token	"\'"		<< more (); mode (LC_BAD_CHAR_CONST); >>

/* Catch invalid hexadecimal escapes.	*/
#token	"\\x"		<< more (); mode (LC_BAD_CHAR_CONST); >>

/* Catch all other invalid escape sequences.	*/
#token	"\\"		<< more (); mode (LC_BAD_CHAR_CONST); >>

/*
 * Suck up everything else.
 *
 * Note how we're *not* using the trick that we used in the
 * LC_STRING class above.  This is because we're *only* sucking
 * up a single character and all of the 'interesting' ones have
 * already been matched above.
 */
#token	"~[]"		<< more (); mode (LC_CHAR_DONE); >>


/*
 * Start of LC_BAD_CHAR_CONST.
 *
 * Try to eat up the badly formed character literal so we can continue
 * or exit gracefully.
 *
 * The plan here is to eat stuff until we find the (hopefully) intended
 * terminating single quote.  To keep from eating up the input we'll
 * stop if we reach the end of the current input line.
 */
#lexclass	LC_BAD_CHAR_CONST

#token	BAD_CHAR_CONST	"\'"		<< mode (START); >>
#token			"[\r\n]"	<< newline(); mode (START);
					   return BAD_CHAR_CONST; >>
#token			"~[\r\n\']+"	<< more (); >>

/*
 * Start of LC_CHAR_DONE.
 *
 * Try to end the character literal.
 *
 * The plan here is to look for the ending single quote.  If we
 * find anything else then return an error token instead.
 */

#lexclass LC_CHAR_DONE

#token	CHAR_CONST	"\'"		<< mode (START); >>
#token	BAD_CHAR_CONST	"[\r\n]"	<< newline(); mode (START); >>
#token			"~[]"		<< more ();
					   mode (LC_BAD_CHAR_CONST); >>

/*
 * Switch back to the starting lexical class.
 */
#lexclass	START


<<
#include <ctype.h>

#include "ATokenBuffer.h"
#include "DLGLexer.h"
#include "myToken.h"

int main() {
   DLGFileInput		in(stdin);
   DLGLexer		lexer(&in,2000);
   ANTLRTokenBuffer	pipe(&lexer,2);
   ANTLRToken		aToken;
   P			parser(&pipe);

   lexer.setToken(&aToken);
   parser.init();
   parser.start();
   return 0;
}
>>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		Rules
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class P {

start
	: (shme)*
	;

shme
	: cbad:BAD_CHAR_CONST
		<<
			{
			int c;

			c = $cbad->getText()[1] & 0xff;
			printf ("\nBad char constant\n");
			printf ("Line %d: ", $cbad->getLine());
			printf ("Char: (%s)\n", $cbad->getText());
			};
		>>
	| ch:CHAR_CONST
		<<
			{
			int c;

			c = $ch->getText()[1] & 0xff;
			printf ("\nGood character constant\n");
			printf ("Line %d: ", ch->getLine());
			printf ("Char: Char=(%c) Dec=(%u) Oct=(%o) (%s)\n",
				c, c, c, $ch->getText());
			};
		>>
	| sbad:BAD_STRING_CONST
		<<
			printf ("\nBad string constant\n");
			printf ("Line %d: ", sbad->getLine());
			printf ("String (%s)\n", $sbad->getText());
		>>
	| << int numStrings = 1; >>
	  (s:STRING_CONST
		<<
			{
			char * p;
			int c;

			printf ("\nGood string constant part #%d\n",
				numStrings++);
			printf ("Line %d: ", s->getLine());
			printf ("Octal version ");
		        for (p = $s->getText(); *p != 0; p++)
			    {
			    c = (*p & 0xff);
			    if ('\n' == c)
				printf ("{newline}");
			    else if (isprint (c))
				printf ("%c", c);
			    else
				printf ("{%o}", c);
			    };
			printf ("\n");

			printf ("Line %d: ", s->getLine());
			printf ("Hex version   ");
			for (p = $s->getText(); *p != 0; p++)
			    {
			    c = (*p & 0xff);
			    if ('\n' == c)
				printf ("{newline}");
			    else if (isprint (c))
				printf ("%c", c);
			    else
				printf ("{%x}", c);
			    };
			printf ("\n");
			};
		>>
	  )+
	;

}
