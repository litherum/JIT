%{
#include "Parser.h"
int regexlex(YYSTYPE *lvalp, RegexParser*);
void regexerror(RegexParser* parser, const char *s);
%}

%pure-parser

%parse-param { RegexParser* parser }
%lex-param { RegexParser* parser }

%union {
char c;
NFAHandle i;
}

%token <c> CHARACTER
%type <i> base factor term regex

%start regex

%%
regex: term '|' regex { $$ = parser->alternate($1, $3);   }
| term                { $$ = $1;                          }
;
term:                 { $$ = parser->epsilon();           }
| term factor         { $$ = parser->concatenate($1, $2); }
;
factor: base          { $$ = $1;                          }
| factor '*'          { $$ = parser->star($1);            }
;
base: CHARACTER       { $$ = parser->literal($1);         }
| '(' regex ')'       { $$ = $2;                          }
%%
