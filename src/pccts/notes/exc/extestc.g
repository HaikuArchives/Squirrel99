/*
  extestc.g

  30-Jun-97
  pccts 1.33MR7

  example of use of exceptions in C mode
*/

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#header<<
#define D_TextSize 50
#include <stdio.h>
#include "charbuf.h"
>>

exception
  default: 
    <<printf("\tGlobal default exception handler (token is \"%s\")\n",
                                                            LATEXT(1));
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>

<<
main ()
{
 int	signal;
 printf("\nExamples of exceptions in C mode - 30-Jun-97\n\n");
 ANTLR(t0(&signal),stdin);
 printf("\n\nEnd of program\n");
}
>>

#token              "[\ \t]+"        << zzskip(); >>
#token              "[\n]"           << zzskip(); >>
#token Word	        "[a-z A-Z]+"
#token Number		"[0-9]+"
#token Comment      "\" ~[\"]* \""
#token Dollar       "\$"
#token Pound        "\#"
t0 : (
        c:Comment <<printf("Expecting %s\n",$c);>>
          ( "t1" ex1:t1
          | "t2" ex2:t2
          | "t3" t3
          | "t4" t4
          )
     )* "@"
;

exception [ex1] 
  catch MismatchedToken:
    <<printf("\tException [ex1] (MismatchedToken) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  catch NoViableAlt:
    <<printf("\tException [ex1] (NoViableAlt) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  catch NoSemViableAlt:
    <<printf("\tException [ex1] (NoSemViableAlt) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  default:
    <<printf("\tException [ex1] (default) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>

exception [ex2] 
  catch MismatchedToken:
    <<printf("\tException [ex2] (MismatchedToken) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  catch NoViableAlt:
    <<printf("\tException [ex2] (NoViableAlt) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  catch NoSemViableAlt:
    <<printf("\tException [ex2] (NoSemViableAlt) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>
  default:
    <<printf("\tException [ex2] (default) in rule t0\n");
      zzconsumeUntilToken(Comment);
      zzsuppressSignal;
    >>

t1 : Word n:Number 
                <<printf("\tNormal Action for t1 alt 1\n");>>
	         exception[n]
               catch MismatchedToken:
        		<<printf("\tException [n] in t1: MismatchedToken\n");>>
        exception 
          catch MismatchedToken:
            <<printf("\tException alt 1 of t1: MismatchedToken\n");>>
    | Number w:Word { Dollar }
                <<printf("\tNormal Action for t1 alt 2\n");>>
        exception
          catch MismatchedToken:
	        <<printf("\tException in alt 2 of t1: MismatchedToken\n");>>
    | Dollar
         { Word Word
                 <<printf("\tNormal Action for t1 alt 3 subrule {...}\n");>>
             exception
                catch MismatchedToken:
        	       <<printf("\tException in alt 3 subrule {...} of t1: MismatchedToken\n");>>
         }    
         Number
            <<printf("\tNormal Action for t1 alt 3\n");>>
             exception
                catch MismatchedToken:
        	       <<printf("\tException in alt 3 of t1: MismatchedToken\n");>>
;
exception
  catch MismatchedToken:
	<<printf("\tException for rule t1: MismatchedToken\n");
      zzsetSignal(_signal);
    >>
  catch NoViableAlt:
	<<printf("\tException for rule t1: NoViableAlt\n");
      zzsetSignal(_signal);
    >>

exception [w]
  catch MismatchedToken:
	<<printf("\tException [w] in t1: MismatchedToken\n");
    >>


t2 : "\*2\*" (
     Word n:Number 
                <<printf("\tNormal Action for t2 alt 1\n");>>
	         exception[n]
               catch MismatchedToken:
        		<<printf("\tException [n] in t2: MismatchedToken\n");>>
        exception 
          catch MismatchedToken:
            <<printf("\tException alt 1 of t2: MismatchedToken\n");>>
    | Number w:Word { Dollar }
                <<printf("\tNormal Action for t2 alt 2\n");>>
        exception
          catch MismatchedToken:
	        <<printf("\tException in alt 2 of t2: MismatchedToken\n");>>
    | Dollar
         { Word Word
                 <<printf("\tNormal Action for t2 alt 3 subrule {...}\n");>>
             exception
                catch MismatchedToken:
        	       <<printf("\tException in alt 3 subrule {...} of t2: MismatchedToken\n");>>
         }    
         Number
            <<printf("\tNormal Action for t2 alt 3\n");>>
             exception
                catch MismatchedToken:
        	       <<printf("\tException in alt 3 of t2: MismatchedToken\n");>>
  )
;
exception
  catch MismatchedToken:
	<<printf("\tException for rule t2: MismatchedToken\n");
      zzexportSignal;
    >>
  catch NoViableAlt:
	<<printf("\tException for rule t2: NoViableAlt\n");
      zzexportSignal;
    >>

exception [w]
  catch MismatchedToken:
	<<printf("\tException [w] in t2: MismatchedToken\n");
    >>

t3 : Word @ <<printf("\tNormal Action for t3\n");>>
;
t4 : ( Word  Word
         <<printf("\tNormal Action t4 alt 1\n");>>
     | Number Number
         <<printf("\tNormal Action t4 alt 2\n");>>
         exception 
           catch MismatchedToken:
        	<<printf("\tException for rule t4 alt 2: MismatchedToken\n");
            >>          
     ) 
        <<printf("\tNormal Action for rule t4 entire block\n");>>

        exception
         catch MismatchedToken:
        	<<printf("\tException for rule t4 entire block: MismatchedToken\n");
            >>
         default:
        	<<printf("\tException for rule t4 entire block: default\n");
            >>
;
