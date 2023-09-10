#include "parser.h"

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
{//EL TEST DE INICIO TMB LLEVA NADA CREO MEPA, PUEDO TENER PROGRAMA VACIO, PQ SSE INVOCA DECLARACIONES CON {} Y ESTO ERA 0 o + VECES PAREC EN BNFE
    test(,,);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset); //ACA A DECLARACIONES LO LLAMO CON EL FOLSET DE FIRST DE UNIDAD DE TRADUCCION Y ADEMAS CON EL NADA PQ PUEDE TENER LAMBDA
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
    test(first(ESPECIFICADOR_DECLARACION),folset | first(DEFINICION_FUNCION) | first(DECLARACION_VARIABLE),);
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset);
			break;
		
		case CASIGNAC:
		    declaracion_variable(folset);
		    break;
		case CCOR_ABR:
		    declaracion_variable(folset);
		    break;
		case CCOMA:
		    declaracion_variable(folset);
		    break;
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


void declaracion_parametro(set folset) //BORRAR ESTE COMMENT, PARA EL LLAMADO AL TEST CON EL FOLSET INCLUIR CON EL [ TMB EL ] POR LAS DU, EXPPLICAR QUE ESTA POR SI PUSIERON ] Y NO [
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
	test(,,);
}


void lista_declaraciones_init(set folset)
{
	test(first(LISTA_DECLARACIONES_INIT),folset | CIDENT | first(DECLARADOR_INIT) | CCOMA | CIDENT,);
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
	test(,,);
}


void declarador_init(set folset)
{
	test(first(DECLARADOR_INIT),folset | CASIGNAC | first(CONSTANTE) | CCOR_ABR | CCONS_ENT | CCOR_CIE | CLLA_ABR | first(LISTA_INICIALIZADORES) | CLLA_CIE,);
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
	test(,,);
}


void lista_inicializadores(set folset)
{
	constante();

	while(lookahead_in(CCOMA))
	{
		scanner();
		constante();
	}
}


void proposicion_compuesta(set folset)
{
	test(,,);
	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones();

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones();

	match(CLLA_CIE, 25);
	test(,,);
}


void lista_declaraciones(set folset)
{
	declaracion(); 

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(); 
}


void declaracion(set folset)
{
	especificador_tipo();

	lista_declaraciones_init();

	match(CPYCOMA, 23);
	test(,,);
}


void lista_proposiciones(set folset)
{
	proposicion();

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion();
}


void proposicion(set folset)
{
	test(,,);
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta();
			break;
		
		case CWHILE:
			proposicion_iteracion();
			break;
		
		case CIF:
			proposicion_seleccion();
			break;
		
		case CIN:
		case COUT:
			proposicion_e_s();
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
			proposicion_expresion();
			break;
		
		case CRETURN:
			proposicion_retorno();
			break;
		
		default:
			error_handler(68);
	}
}


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion();

	match(CPAR_CIE, 21);

	proposicion();
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion();

	match(CPAR_CIE, 21);

	proposicion();

	if(lookahead_in(CELSE))
	{
		scanner();
		proposicion();
	}
}


void proposicion_e_s(set folset)
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 30);
			
			variable();
			
			while(lookahead_in(CSHR))
			{
				scanner();
				variable();
			}

			match(CPYCOMA, 23);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 31);
			
			expresion();

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion();
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(29);
	}
	test(,,);
}


void proposicion_retorno(set folset)
{
	scanner();
	
	expresion();
	
	match(CPYCOMA, 23);
	test(,,);
}


void proposicion_expresion(set folset)
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion();

	match(CPYCOMA, 23);
	test(,,);
}


void expresion(set folset)
{	
	expresion_simple();

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple();
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple();
				break;
		}
	}
}


void expresion_simple(set folset)
{
	test(,,);
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino();

	while(lookahead_in(CMAS | CMENOS | COR))
	{
		scanner();
		termino();
	}
}


void termino(set folset)
{	
	factor();

	while(lookahead_in(CMULT | CDIV | CAND))
	{
		scanner();
		factor();
	}
}


void factor(set folset)
{
	test(,,);
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion();
			else
				variable();
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante();
			break;
		
		case CCONS_STR:
			scanner();
			break;
		
		case CPAR_ABR:
			scanner();
			expresion();
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			expresion();
			break;
			
		default:
			error_handler(32);
	}
	test(,,);
}


void variable(set folset)
{
	test(,,);
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion();
		match(CCOR_CIE, 21);
	}
	test(,,);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones();

	match(CPAR_CIE, 21);
	test(,,);
}


void lista_expresiones(set folset)
{
	expresion();

	while(lookahead_in(CCOMA))
	{
		scanner();
		expresion();
	}
}


void constante(set folset)
{
	test(,,);
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
	test(,,);
}
