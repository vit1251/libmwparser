%{
/*
 * Sample lexical analyzer for a simple calculator.
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define YYSTYPE double
#include "libmwparser_la-expr.h"
// extern YYSTYPE yylval;    /* value of numeric token */
/* \.?{digit}+\.?([eE][+-]?\.?{digit}+)?|{digit}*\.{digit}+([eE][+-]?\.?{digit}+)? { */
%}

digit [0-9]
xdigit [0-9A-F]
space [ \t]

%%
{space}         { ; }    /* spaces are ignored */
{digit}*(\.{digit}*)? {
                   yylval = strtod(yytext, 0); 
                   return NUMBER; }
0x{xdigit}+ {
                   yylval = strtod(yytext, 0); 
                   return NUMBER; }
\*\*            { return '^';      }
last            { return LAST;     }
acos            { return ACOS;     }
asin            { return ASIN;     }
atan            { return ATAN;     }
cos             { return COS;      }
sin             { return SIN;      }
tan             { return TAN;      }
exp             { return EXP;      }
sqrt            { return SQRT;     }
abs             { return ABS;      }
div             { return DIV;      }
mod             { return MOD;      }
\%              { return MOD;      }
ln              { return LN;       }
trunc           { return TRUNC;    }
round           { return ROUND;    }
ceil            { return CEIL;     }
floor           { return FLOOR;    }
\<=             { return INFEQUAL; }
\>=             { return SUPEQUAL; }
\<              { return INF;      }
\>              { return SUP;      }
or              { return OR;       }
and             { return AND;      }
=               { return EQUAL;    }
!=              { return NOTEQUAL; }
not             { return NOT;      }
e               { return E;        }
pi              { yylval = M_PI; return NUMBER; }
\n              { return '\n'; }
.               { return yytext[0]; }
%%
