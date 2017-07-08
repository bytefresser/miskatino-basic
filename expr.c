#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "tokenint.h"
#include "expr.h"
#include "editor.h"

char parseExprUnary() {
    if (!charInStr(*getCurTokPos(), "-!")) {
        return 0;
    }
    parseSymbol();
    if (prevTok->body.symbol == '-') {
        prevTok->body.symbol = '~';
    }
    return 1;
}

char parseExprVal(void) {
    if (parseNumber()) {
        if (*getCurTokPos() == '(') {
            setTokenError(getCurTokPos(), 9);
            return 'e';
        }
        return '+';
    }
    if (parseName(0)) {
        if (*getCurTokPos() == '(') {
            prevTok->type = TT_FUNCTION;
            skipTokenInInput(1);
            return 'f';
        }
        prevTok->type = TT_VARIABLE;
        return '+';
    }
    if (*getCurTokPos() == '(') {
        parseSymbol();
        return '(';
    }
    if (parseExprUnary()) {
        return '1';
    }
    setTokenError(getCurTokPos(), 9);
    return 'e';
}

char parseExprBop(void) {
    char c = *getCurTokPos();
    if (c == 0 || c == ';') {
        return 's';
    }
    if (*getCurTokPos() == ')') {
        return ')';
    }
    if (*getCurTokPos() == ',') {
        return ',';
    }
    if (charInStr(*getCurTokPos(), "+-*^/%<>=&|")) {
        parseSymbol();
        return '1';
    }
    setTokenError(getCurTokPos(), 9);
    return 'e';
}

char parseExprRbr(char brCount, char argCount) {
    if (brCount < 1) {
        setTokenError(getCurTokPos(), 9);
        return 'e';
    } else {
        parseSymbol();
        if (argCount > 0) {
            prevTok->type = TT_FUNC_END;
            prevTok->body.symbol = argCount;
        }
        return '+';
    }
}

char parseExprComma(char inFunc) {
    if (inFunc == 0) {
        setTokenError(getCurTokPos(), 9);
        return 'e';
    } else {
        parseSymbol();
        return '1';
    }
}

char operatorPriority(char op) {
    switch (op) {
        case '&':
        case '|':
            return 5;
        case '<':
        case '>':
        case '=':
        case '#':
        case '{':
        case '}':
            return 10;
        case '+':
        case '-':
            return 15;
        case '*':
        case '/':
        case '%':
            return 20;
        case '!':
        case '~':
            return 25;
        case '(':
            return 0;
    }
}

char isUnary(char op) {
    return op == '!' || op == '~';
}

char convertRpnPop(char op) {
    if (op == '(') {
        return 0;
    }
    curTok->type = TT_SYMBOL;
    curTok->body.symbol = op;
    curTok = nextToken(curTok);
    return 1;
}

void shuntingYard(token* next) {
    char opstack[16];
    char sp = -1;
    char prio;
    char op;
    void* start = next;
    while (next->type != TT_ERROR) {
        if (next->type == TT_VARIABLE || next->type == TT_NUMBER) {
            copyToken(curTok, next);
            curTok = nextToken(curTok);
        } else if (next->type == TT_SYMBOL) {
            op = next->body.symbol;
            if (op == '(') {
                opstack[++sp] = '(';
            } else if (op == ')' || op == ',') {
                while (convertRpnPop(opstack[sp--]));
                if (op == ',') {
                    sp++;
                }
            } else {
                prio = operatorPriority(op);
                if (isUnary(op)) {
                    prio += 1;
                }
                while (sp >= 0 && operatorPriority(opstack[sp]) >= prio) {
                    convertRpnPop(opstack[sp--]);
                }
                opstack[++sp] = op;
            }
        } else if (next->type == TT_FUNCTION) {
            opstack[++sp] = (char)(((void*)next) - start);
            opstack[++sp] = '(';
        } else if (next->type == TT_FUNC_END) {
            while (convertRpnPop(opstack[sp--]));
            copyToken(curTok, start + opstack[sp--]);
            curTok = nextToken(curTok);
        }
        next = nextToken(next);
    }
    while (sp >= 0) {
        convertRpnPop(opstack[sp--]);
    }
}

void convertToRpn(token* next) {
    char buf[MAX_LINE_LEN * 2];
    curTok->type = TT_ERROR;
    memcpy(buf, next, ((void*) curTok) - ((void*) next) + 1);
    curTok = next;
    next = (void*) buf;
    shuntingYard(next);
    curTok->type = TT_ERROR;
    prevTok = NULL;
}

char parseExpression(void) {
    token* startTok = curTok;
    char funcBrackets[16];
    char iFuncBr = 0;
    char state = '1';
    funcBrackets[iFuncBr] = 0;
    while (state != 's') {
        switch (state) {
            case '1':
                state = parseExprVal();
                break;
            case 'f':
                funcBrackets[++iFuncBr] = 1;
                state = '1';
                break;
            case '(':
                funcBrackets[++iFuncBr] = 0;
                state = '1';
                break;
            case '+':
                state = parseExprBop();
                break;
            case ')':
                state = parseExprRbr(iFuncBr, funcBrackets[iFuncBr]);
                iFuncBr -= 1;
                break;
            case ',':
                state = parseExprComma(funcBrackets[iFuncBr]++);
                break;
            case 'e':
                return 0;
        }
    }
    convertToRpn(startTok);
    return 1;
}


