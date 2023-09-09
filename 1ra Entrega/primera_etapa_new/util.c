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
		case UNIDAD_TRADUCCION:		            return(CVOID | CCHAR | CINT | CFLOAT | NADA);
		case DECLARACIONES:                     return(CVOID | CCHAR | CINT | CFLOAT);
		case LISTA_DECLARACION_PARAM:           return(CVOID | CCHAR | CINT | CFLOAT);
		case DECLARACION_PARAMETRO:             return(CVOID | CCHAR | CINT | CFLOAT);
		case DECLARACION:                       return(CVOID | CCHAR | CINT | CFLOAT);
		case LISTA_DECLARACIONES:               return(CVOID | CCHAR | CINT | CFLOAT);
		case ESPECIFICADOR_TIPO: 			    return(CVOID | CCHAR | CINT | CFLOAT);
		case ESPECIFICADOR_DECLARACION:         return(CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA);
		case DEFINICION_FUNCION: 		        return(CPAR_ABR);
		case DECLARACION_VARIABLE: 		        return(CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA);
		case VARIABLE:                          return(CIDENT);
		case LLAMADA_FUNCION:                   return(CIDENT);
		case LISTA_DECLARACIONES_INIT: 	        return(CIDENT);
		case DECLARADOR_INIT: 			        return(NADA | CASIGNAC | CCOR_ABR);
		case LISTA_INICIALIZADORES: 	        return(CCONS_ENT | CCONS_FLO | CCONS_CAR);
		case PROPOSICION_COMPUESTA:             return(CLLA_ABR);
		case LISTA_PROPOSICIONES:               return(CLLA_ABR);
		case PROPOSICION: 					    return(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR | CPYCOMA | CLLA_ABR | CWHILE | CIF | CRETURN | CIN | COUT);
		case PROPOSICION_ITERACION:             return(CWHILE);
		case PROPOSICION_SELECCION: 	        return(CIF);
		case PROPOSICION_E_S: 			        return(CIN | COUT);
		case PROPOSICION_RETORNO: 		        return(CRETURN);
		case PROPOSICION_EXPRESION: 	        return(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR | CPYCOMA);
		case EXPRESION:                         return(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		case LISTA_EXPRESIONES:                 return(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		case EXPRESION_SIMPLE: 			        return(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		case TERMINO:                           return(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		case FACTOR: 					        return(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		case CONSTANTE: 				        return(CCONS_ENT | CCONS_FLO | CCONS_CAR);
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