#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "parser_extension_private.h"

#ifdef TEST_EXPR
int main(int argc, char **argv)
{
    char *l = NULL;
    size_t n = 0;
    double q;
int t;
       printf("1: %0.8g\n", wiki_parser_template_expr_calc("55.752222 % 360"));
       printf("2: %0.8g\n", wiki_parser_template_expr_calc("2 * pi"));
       printf("3: %0.8g\n", wiki_parser_template_expr_calc("-8 mod 2.9"));
       printf("4: %0.8g\n", wiki_parser_template_expr_calc("-8 mod 3.2"));
       printf("5: %0.8g (%0.8g)\n", wiki_parser_template_expr_calc("e"), 123E-2);
       double a = modf(12.34, &q);
//a = 1234567.123;
       printf("remain: %f : %f, %d\n", a, q, t);
/*
    while((getline(&l, &n, stdin))) {
       printf("%0.8g\n", wiki_parser_template_expr_calc(l));
       free(l); l = NULL;
    }
*/
    return EXIT_SUCCESS;
}
#endif
