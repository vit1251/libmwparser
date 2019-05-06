%{
/*
 *   calculator:   a simple calculator.
 *   This calculator allows all common arithmetic operations,
 *   including sin, cos, tan, sqrt, exp, pow, log.
 *   The calculator is a simple example of using the yacc
 *   parser generator.
 *  
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pcre_replace.h"
#include "util.h"

#include <Eina.h>

// #include "expr.h"

#define YYSTYPE double

YYSTYPE last_value;

// extern yy_scan_string(yyconst char *);
extern int yylex(void);
int yyerror(const char *s);
double round_by(double a, double b);

%}

/*
  * Define the tokens produced by yylex()
 */
%token NUMBER
%token LAST
%left OR AND 
%left NOTEQUAL EQUAL 
%left INFEQUAL SUPEQUAL SUP INF
%left ROUND
%left '+' '-'
%left '*' '/' MOD DIV
%left '^'
%left NEGATIVE POSITIVE
%left E EXP SQRT LN ABS TRUNC CEIL FLOOR ACOS ASIN ATAN COS SIN TAN NOT

/*
 *  Begin specification of the parser.
 */
%%
/*
 * a 'list' may be empty, contain blank lines or expressions.
 */
list:                    
    |    list '\n'      
    |    list expr '\n'  {last_value=$2;} 
    ; 
/*
 * Expressions are defined recursively as strings of terms
 * and expressions. Note that the 'sin',... functions do not
 * require bracketed parameters although sin x +1 is
 * interpreted as (sin(x))+1
 */
expr:     term                     { $$ = $1;         }
    |     '-' expr  %prec NEGATIVE { $$ = - $2;       }
    |     '+' expr  %prec POSITIVE { $$ =  $2;        }
    |     expr OR    expr          { $$ = $1 || $3;   }
    |     expr AND   expr          { ulog("EXPR AND %f %f\n", $1, $3); $$ = $1 && $3;   }
    |     expr EQUAL expr          { ulog("EXPR EQUAL %f %f\n", $1, $3); $$ = $1 == $3;   }
    |     expr SUPEQUAL expr       { $$ = $1 >= $3;   }
    |     expr INFEQUAL expr       { $$ = $1 <= $3;   }
    |     expr SUP expr            { ulog("EXPR SUP %f %f\n", $1, $3); $$ = $1 > $3;    }
    |     expr INF  expr           { $$ = $1 < $3;    }
    |     expr NOTEQUAL expr       { $$ = $1 != $3;   }
    |     expr ROUND expr          { ulog("EXPR ROUND %f %f\n", $1, $3); $$ = round_by($1, $3); }
    |     expr '+' expr            { ulog("MATHADD: %f, %f\n", $1, $3); $$ = $1 + $3;    }
    |     expr '-' expr            { $$ = $1 - $3;    }
    |     expr MOD    expr         { ulog("MATHMOD: %f, %f\n", $1, $3); 
                                     double r = fmod($1, $3);
                                      $$ = (int) r;
                                      // $$ = (r - floor(r) > 0.5) ? floor(r) : round(r); 
                                   }
    |     expr '*' expr            { $$ = $1 * $3;    }
    |     expr '/' expr            { $$ = $1 / $3;    }
    |     expr '^' expr            { $$ = pow($1, $3); }
    |     expr DIV    expr         { ulog("MATHDIV: %f, %f\n", $1, $3); $$ = $1 / $3; }
    |     expr E expr              { $$ = $1 * pow(10, $3); }
    |     ACOS   expr              { $$ = acos($2);  }
    |     ASIN   expr              { $$ = asin($2);  }
    |     ATAN   expr              { $$ = atan($2);  }
    |     COS    expr              { $$ = cos($2);   }
    |     SIN    expr              { $$ = sin($2);   }
    |     TAN    expr              { $$ = tan($2);   }
    |     EXP    expr              { $$ = exp($2);   }
    |     SQRT   expr              { $$ = sqrt($2);  }
    |     LN     expr              { $$ = log($2);   }
    |     NOT    expr              { $$ = ! $2;   }
    |     ABS    expr              { ulog("MATHABS: %f, %f\n", $2, fabs($2)); $$ = fabs($2);   }
    |     TRUNC  expr              { $$ = trunc($2); }
    |     CEIL   expr              { $$ = ceil($2);  }
    |     FLOOR  expr              { $$ = floor($2); }
    ;
/*
 * The following are of the highest precedence.
 * They needed to be distinguished to allow the
 * functions (sin...) to operate properly without
 * parentheses
 */
term:     NUMBER                   { $$ = $1;         }
    |     LAST                     { $$ = last_value; }
    |     '(' expr ')'             { $$ = $2;         }
    ;
%%

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

int lineno;
static const char *line = NULL;

int yywrap()
{  
   return 1;
}

int yyerror(const char *s)
{
    ulog("EXPR ERROR: %s\n", line);
    last_value = nan(line); //fprintf(stderr,"msg: %s\n", s);
    return 0;
}

double round_by(double a, double b)
{
    if(b != 0.0) {
       double t, r, e;
       e = pow(10.0, ceil(b));
       t = round((a * e));
       r = (t / e);
//       printf("ROUND: %f %f => %f (%f : %f)\n", a, b, r, t, e);
       return r;
    } else {
       return round(a);
    }
}

double wiki_parser_template_expr_calc(const char *expr)
{
    last_value = nan(expr);
    if(expr) {
       char *e = strdup(expr);
       line = expr;
       if(e) {
           e = pcre_replace("/(\r|\n|\t| )/", "", e);
           e = pcre_replace("/$/", "\n", e);
           e = pcre_replace("/<>/", "!=", e);
           if(e) {
               ulog("EXPR: %s", e);
               yy_scan_string(e);
               yyparse();
               yylex_destroy();
               free(e);
           }
           
       }
    }

    return last_value;
}

