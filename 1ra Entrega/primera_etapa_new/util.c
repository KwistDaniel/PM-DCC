#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "codigos.h"
#include "error.h"


void scanner()
{	
	int i;
	for(; (i = yylex()) != NADA && lookahead() == SEGUIR;);

	if(i == NADA)
		sbol->codigo = CEOF;
	/* yylex retorna 0 si llego a fin de archivo */

	liberar = linea;
	linea = (char *) malloc (strlen(linea) + strlen (token1.lexema) + 3);
	strcpy(linea, liberar);
	strcat(linea, token1.lexema);
	free((void *) liberar);
}


void init_parser(int argc, char *argv[])
{
	linea = (char *) malloc (2);
	strcat(linea, "");
	nro_linea = 0;

	if(argc != 3)
	{
		error_handler(6);
		error_handler(COD_IMP_ERRORES);
		exit(1);
	}
	else
	{
		if(strcmp(argv[1], "-c") == 0)
		{
			if((yyin = fopen(argv[2], "r")) == NULL)
			{
				error_handler(8);
				error_handler(COD_IMP_ERRORES);
				exit(1);
			}
		}
		else
		{
			error_handler(7);
			error_handler(COD_IMP_ERRORES);
			exit(1);
		}
	}

	sbol = &token1;

	scanner();
}


void match(set codigo, int ne)
{
    if(lookahead() & codigo)
        scanner();
    else
        error_handler(ne);
}


set lookahead()
{
	return sbol->codigo;
}


set lookahead_in(set conjunto)
{
	return lookahead() & conjunto;
}



enum procedimientos {
	UNIDAD_TRADUCCION, DECLARACIONES, ESPECIFICADOR_TIPO, ESPECIFICADOR_DECLARACION, 
	DEFINICION_FUNCION, LISTA_DECLARACION_PARAM, DECLARACION_PARAMETRO, DECLARACION_VARIABLE,
	LISTA_DECLARACIONES_INIT, DECLARADOR_INIT, LISTA_INICIALIZADORES, PROPOSICION_COMPUESTA,
	LISTA_DECLARACIONES, DECLARACION, LISTA_PROPOSICIONES, PROPOSICION, PROPOSICION_ITERACION,
	PROPOSICION_SELECCION, PROPOSICION_E_S, PROPOSICION_RETORNO, PROPOSICION_EXPRESION, EXPRESION,
	EXPRESION_SIMPLE, TERMINO, FACTOR, VARIABLE, LLAMADA_FUNCION, LISTA_EXPRESIONES, CONSTANTE
};
//Capaz tmb necesite ponerlo en parser.c


set first(int noterminal)
{
	switch(noterminal)
	{
		case UNIDAD_TRADUCCION:		
		case DECLARACIONES:
		case LISTA_DECLARACION_PARAM:
		case DECLARACION_PARAMETRO:
		case DECLARACION:
		case LISTA_DECLARACIONES:
		case ESPECIFICADOR_TIPO: 			return(CVOID | CCHAR | CINT | CFLOAT);
		case ESPECIFICADOR_DECLARACION:
		case DEFINICION_FUNCION: 		
		case DECLARACION_VARIABLE: 		
		case VARIABLE:
		case LLAMADA_FUNCION:
		case LISTA_DECLARACIONES_INIT: 	
		case DECLARADOR_INIT: 			
		case LISTA_INICIALIZADORES: 	
		case PROPOSICION_COMPUESTA: 
		case LISTA_PROPOSICIONES:
		case PROPOSICION: 					
		case PROPOSICION_ITERACION:
		case PROPOSICION_SELECCION: 	
		case PROPOSICION_E_S: 			
		case PROPOSICION_RETORNO: 		
		case PROPOSICION_EXPRESION: 	
		case EXPRESION:
		case LISTA_EXPRESIONES:
		case EXPRESION_SIMPLE: 			
		case TERMINO: 
		case FACTOR: 					
		case CONSTANTE: 				
		default: 								error_handler(39); // case 39: printf("\t Error %d: En FIRST\n", ne); break;
	}
}


void test(set cjto1, set cjto2, int ne)
{
	if(!lookahead_in(cjto1))
	{
		error_handler(ne);
		cjto1 = cjto1 | cjto2;
		while(!lookahead_in(cjto1))
			scanner();
	}
}