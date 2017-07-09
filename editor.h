#ifndef __EDITOR_H_
#define __EDITOR_H_

#include "utils.h"

#define MAX_LINE_LEN 80

void initEditor(char* prgBody);
char readLine(char* line);
prgline* findLine(short num);
void injectLine(char* s, short num);

#endif

