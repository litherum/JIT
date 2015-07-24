%{
#include "ASTParserHelpers.h"
int yylex (void);
void yyerror(const char*);
%}

%union {
char c;
ASTNodeForParsing n;
}

%token <c> CHARACTER
%type <n> base factor term regex

%start regex

%%
regex: term '|' regex { $$ = alternation($1, $3);   }
| term                { $$ = $1;                    }
;
term:                 { $$ = epsilon();             }
| term factor         { $$ = concatenation($1, $2); }
;
factor: base          { $$ = $1;                    }
| factor '*'          { $$ = star($1);              }
;
base: CHARACTER       { $$ = literal($1);           }
| '(' regex ')'       { $$ = $2;                    }
%%

int index = 0;
extern char *thestring;
int yylex(void)
{
    int c = thestring[index++];
    if (c == '\\')
        c = thestring[index++];
    switch (c) {
    case 0:
        return 0;
    case '|':
    case '*':
    case '(':
    case ')':
        return c;
    default:
        yylval.c = c;
        return CHARACTER;
    }
}

#include <stdio.h>
void yyerror(const char *s)
{
    fprintf(stderr, "%s\n", s);
}
