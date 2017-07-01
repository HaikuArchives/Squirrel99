/*

YastBe

File			: Utils.h
What's in 		: Various utils functions
Language 		: C++

Creation 		: 05/12/99
Last update 	: 05/23/99
Author			: Jean-Louis Villecroze

(c) Handled Brain 1999
*/

int last(char *str,char c); // return the position of the last occurence of a char in a string (-1 if not found)
char *strsub(char *str,int from,int to); // create a string which is a part of another string 
char *ustrcon(char *left,char *right); // Return a new string by concatening the 2  but not free the memory
char *strcon(char *left,char *right); // Return a new string by concatening the 2 
char *long2str(long val); // create a string with the value long in
char *int2str(int val); // create a string with the value int in
char *float2str(float val); // create a string with the value int in
char *float2str(float val,int precision); // create a string with the value int float using a specify precision


