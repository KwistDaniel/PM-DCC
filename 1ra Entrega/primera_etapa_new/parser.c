#include "parser.h"
#include "util.h"

enum procedimientos {
	UNIDAD_TRADUCCION, DECLARACIONES, ESPECIFICADOR_TIPO, ESPECIFICADOR_DECLARACION, 
	DEFINICION_FUNCION, LISTA_DECLARACION_PARAM, DECLARACION_PARAMETRO, DECLARACION_VARIABLE,
	LISTA_DECLARACIONES_INIT, DECLARADOR_INIT, LISTA_INICIALIZADORES, PROPOSICION_COMPUESTA,
	LISTA_DECLARACIONES, DECLARACION, LISTA_PROPOSICIONES, PROPOSICION, PROPOSICION_ITERACION,
	PROPOSICION_SELECCION, PROPOSICION_E_S, PROPOSICION_RETORNO, PROPOSICION_EXPRESION, EXPRESION,
	EXPRESION_SIMPLE, TERMINO, FACTOR, VARIABLE, LLAMADA_FUNCION, LISTA_EXPRESIONES, CONSTANTE
};

int main(int argc, char *argv[])
{
	init_parser(argc, argv);
	
	unidad_traduccion(CEOF);

	match(CEOF, 9);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{
    test(first(UNIDAD_TRADUCCION),folset | first(DECLARACIONES),40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset);
}


void declaraciones(set folset)
{	
	especificador_tipo(folset | CIDENT | first(ESPECIFICADOR_DECLARACION));
	
	match(CIDENT, 17);
	
	especificador_declaracion(folset);
}


void especificador_tipo(set folset)
{
	test(first(ESPECIFICADOR_TIPO), folset, 41);
	switch(lookahead())
	{
		case CVOID:
			scanner();
			break;
			
		case CCHAR:
			scanner();
			break;
			
		case CINT:
			scanner();
			break;
			
		case CFLOAT:
			scanner();
			break;
			
		default:
			error_handler(18);
	}
    test(folset, NADA, 42);
}


void especificador_declaracion(set folset)
{
    test(first(ESPECIFICADOR_DECLARACION),folset | first(DEFINICION_FUNCION) | first(DECLARACION_VARIABLE),43);
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset);
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable(folset);
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset)
{
	match(CPAR_ABR, 20);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param(folset | CPAR_CIE);

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param(set folset)
{
	declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO));

	while(lookahead_in(CCOMA))
	{
		scanner();
		declaracion_parametro(folset);
	}
}


void declaracion_parametro(set folset)
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		match(CCOR_CIE, 22);
	}
	test(folset,NADA,45);
}


void lista_declaraciones_init(set folset)
{
	test(first(LISTA_DECLARACIONES_INIT),folset | CIDENT | first(DECLARADOR_INIT) | CCOMA | CIDENT,46);
	match(CIDENT, 17);

	declarador_init(folset | CCOMA | CIDENT | first(DECLARADOR_INIT));

	while(lookahead_in(CCOMA))
	{
		scanner();
		match(CIDENT, 17);
		declarador_init(folset);
	}
}


void declaracion_variable(set folset)
{
	declarador_init(folset | CCOMA | first(LISTA_DECLARACIONES_INIT) | CPYCOMA);

	if(lookahead_in(CCOMA))
	{
		scanner();
		lista_declaraciones_init(folset | CPYCOMA);
	}

	match(CPYCOMA, 23);
	test(folset,NADA,44);
}


void declarador_init(set folset)
{
	test(first(DECLARADOR_INIT),folset | CASIGNAC | first(CONSTANTE) | CCOR_ABR | CCONS_ENT | CCOR_CIE | CLLA_ABR | first(LISTA_INICIALIZADORES) | CLLA_CIE,47);
	switch(lookahead())
	{
		case CASIGNAC:
			scanner();
			constante(folset);
			break;

		case CCOR_ABR:
			scanner();
			
			if(lookahead_in(CCONS_ENT))
				constante(folset | CCOR_CIE);

			match(CCOR_CIE, 22);

			if(lookahead_in(CASIGNAC))
			{
				scanner();
				match(CLLA_ABR, 24);
				lista_inicializadores(folset | CLLA_CIE);
				match(CLLA_CIE, 25);
			}
			break;
	}
	test(folset,NADA,48);
}


void lista_inicializadores(set folset)
{
	constante(folset | CCOMA | first(CONSTANTE));

	while(lookahead_in(CCOMA))
	{
		scanner();
		constante(folset);
	}
}


void proposicion_compuesta(set folset)
{
	test(first(PROPOSICION_COMPUESTA),folset | first(LISTA_DECLARACIONES) | first(LISTA_PROPOSICIONES) | CLLA_CIE,49);
	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | first(LISTA_PROPOSICIONES));

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset);

	match(CLLA_CIE, 25);
	test(folset,NADA,50);
}


void lista_declaraciones(set folset)
{
	declaracion(folset | first(DECLARACION));

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset);
}


void declaracion(set folset)
{
	especificador_tipo(folset | first(LISTA_DECLARACIONES_INIT) | CPYCOMA);

	lista_declaraciones_init(folset | CPYCOMA);

	match(CPYCOMA, 23);
	test(folset,NADA,51);
}


void lista_proposiciones(set folset)
{
	proposicion(folset | first(PROPOSICION));

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion(folset);
}


void proposicion(set folset)
{
	test(first(PROPOSICION),folset | first(PROPOSICION_EXPRESION) | first(PROPOSICION_COMPUESTA) | first(PROPOSICION_ITERACION) | first(PROPOSICION_SELECCION) | first(PROPOSICION_RETORNO) | first(PROPOSICION_E_S),52);
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta(folset);
			break;
		
		case CWHILE:
			proposicion_iteracion(folset);
			break;

		case CIF:
			proposicion_seleccion(folset);
			break;

		case CIN:
		case COUT:
			proposicion_e_s(folset);
			break;
		
		case CMAS:
		case CMENOS:
		case CIDENT:
		case CPAR_ABR:
		case CNEG:
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
		case CCONS_STR:
		case CPYCOMA:
			proposicion_expresion(folset);
			break;
		
		case CRETURN:
			proposicion_retorno(folset);
			break;
		
		default:
			error_handler(68);
	}
}


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE);

	match(CPAR_CIE, 21);

	proposicion(folset);
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | first(PROPOSICION) | CELSE);

	match(CPAR_CIE, 21);

	proposicion(folset | CELSE | first(PROPOSICION));

	if(lookahead_in(CELSE))
	{
		scanner();
		proposicion(folset);
	}
}


void proposicion_e_s(set folset)
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 30);
			
			variable(folset | CSHR | first(VARIABLE) | CPYCOMA);
			
			while(lookahead_in(CSHR))
			{
				scanner();
				variable(folset | CPYCOMA);
			}

			match(CPYCOMA, 23);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 31);
			
			expresion(folset | CSHL | first(EXPRESION) | CPYCOMA);

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion(folset | CPYCOMA);
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset,NADA,53);
}


void proposicion_retorno(set folset)
{
	scanner();
	
	expresion(folset | CPYCOMA);
	
	match(CPYCOMA, 23);
	test(folset,NADA,54);
}


void proposicion_expresion(set folset)
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 23);
	test(folset,NADA,55);
}


void expresion(set folset)
{	
	expresion_simple(folset | CASIGNAC | first(EXPRESION_SIMPLE) | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple(folset);
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple(folset);
				break;
		}
	}
}


void expresion_simple(set folset)
{
	test(first(EXPRESION_SIMPLE),folset | first(TERMINO) | COR,56); //+ y - estan en el first
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset | CMAS | CMENOS | COR | first(TERMINO));

	while(lookahead_in(CMAS | CMENOS | COR))
	{
		scanner();
		termino(folset);
	}
}


void termino(set folset)
{	
	factor(folset | CMULT | CDIV | CAND | first(FACTOR));

	while(lookahead_in(CMULT | CDIV | CAND))
	{
		scanner();
		factor(folset);
	}
}


void factor(set folset)
{
	test(first(FACTOR),folset | first(VARIABLE) | first(CONSTANTE) | CNEG | CPAR_ABR | first(LLAMADA_FUNCION) | CCONS_STR,57);
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion(folset);
			else
				variable(folset);
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante(folset);
			break;
		
		case CCONS_STR:
			scanner();
			break;
		
		case CPAR_ABR:
			scanner();
			expresion(folset | CPAR_CIE);
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			expresion(folset);
			break;
			
		default:
			error_handler(32);
	}
	test(folset,NADA,58);
}


void variable(set folset)
{
	test(first(VARIABLE),folset | CCOR_ABR | first(EXPRESION) | CCOR_CIE,59);
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion(folset);
		match(CCOR_CIE, 21);
	}
	test(folset,NADA,60);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 21);
	test(folset,NADA,61);
}


void lista_expresiones(set folset)
{
	expresion(folset | CCOMA | first(EXPRESION));

	while(lookahead_in(CCOMA))
	{
		scanner();
		expresion(folset);
	}
}


void constante(set folset)
{
	test(first(CONSTANTE),folset,62);
	switch(lookahead())
	{
		case CCONS_ENT:
			scanner();
			break;
		
		case CCONS_FLO:
			scanner();
			break;
		
		case CCONS_CAR:
			scanner();
			break;
		
		default:
			error_handler(33);
	}
	test(folset,NADA,63);
}
