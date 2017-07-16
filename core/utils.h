#ifndef __UTILS_H_
#define __UTILS_H_

#include "mytypes.h"

void trim(char* s);
char* skipSpaces(char* s);
char* skipDigits(char* s);
char charInStr(char c, char* s);
char cmpNStrToStr(nstring* ns, char* s);
int decFromStr(char* s);
char isDigit(char c);
char isDigitBased(char c, char base);
char isAlpha(char c);
char isAlNum(char c);
char isSpace(char c);
char toUpper(char c);
char makeDigit(char c, char base);

char input(char* s, short n);
void outputChar(char c);
void outputStr(char* s);
void outputNStr(nstring* s);
void outputInt(long n);
void outputCr();

short adcRead(char channel);
char pinRead(char pin);
uchar sysPeek(unsigned long addr);

#endif

