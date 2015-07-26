%{
#include "Parser.h"
int regexlex(YYSTYPE *lvalp, Regex::Parser*);
void regexerror(Regex::Parser* parser, const char *s);
%}

%pure-parser

%parse-param { Regex::Parser* parser }
%lex-param { Regex::Parser* parser }

%union {
char c;
NFAHandle i;
CSetHandle s;
}

%token <c> CHARACTER
%type <i> base factor term regex cclass
%type <s> cset crange

%start regex

%%
regex: term '|' regex     { $$ = parser->alternate($1, $3);   }
| term                    { $$ = $1;                          }
;
term:                     { $$ = parser->epsilon();           }
| term factor             { $$ = parser->concatenate($1, $2); }
;
factor: base              { $$ = $1;                          }
| factor '*'              { $$ = parser->star($1);            }
| factor '+'              { $$ = parser->plus($1);            }
;
base: CHARACTER           { $$ = parser->literal($1);         }
| '[' cclass ']'          { $$ = $2;                          }
| '(' regex ')'           { $$ = $2;                          }
;
cclass: cset              { $$ = parser->cset($1);            }
| '^' cset                { $$ = parser->ncset($2);           }
;
cset: crange              { $$ = $1;                          }
| cset crange             { $$ = parser->csetUnion($1, $2);   }
;
crange: CHARACTER         { $$ = parser->crange($1);          }
| CHARACTER '-' CHARACTER { $$ = parser->crange($1, $3);      }
;
%%
