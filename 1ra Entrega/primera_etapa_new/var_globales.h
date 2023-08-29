#define TAM_LEXEMA 200

typedef long long set;

typedef struct { set codigo;
                 char lexema[TAM_LEXEMA];
               }token;

#define COD_IMP_ERRORES -1

char * linea;
char *liberar;
int nro_linea;

token token1;

extern int yylex(void);