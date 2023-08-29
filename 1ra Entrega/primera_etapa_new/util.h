#include "var_globales.h"

token *sbol;
extern FILE *yyin;

void scanner();
void init_parser(int, char **);

void match(set, int);
set lookahead();
set lookahead_in(set);
