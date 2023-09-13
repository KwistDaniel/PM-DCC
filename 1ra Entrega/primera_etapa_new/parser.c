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
{
    test(first(UNIDAD_TRADUCCION) | folset, NADA,40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset | first(DECLARACIONES));
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
    test(first(ESPECIFICADOR_DECLARACION), folset,43);
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

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT)) //Analizar si incluyo algo mas(simbolos facil olvidar) (!)
		lista_declaraciones_param(folset | CPAR_CIE | first(PROPOSICION_COMPUESTA));

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param(set folset) //Si toco lo de arriba, aca se afecta el folset de dec de param (!)
{
	declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO));

    //while(lookahead_in(CCOMA )) //falta first declaracion parametro o mepa? (!!) CONSULTAR
	while(lookahead_in(CCOMA | first(DECLARACION_PARAMETRO))) //si toco arriba este lookahead in tmb
	{
		//scanner();
		match(CCOMA,64);//Cambie scanner para que no consuma algo de dec parametro
		declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO)); //si toco arriba esta llamada tmb
	}
}


void declaracion_parametro(set folset)
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 17);

    //if(lookahead_in(CCOR_ABR))
	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
	    //scanner();
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
	test(folset,NADA,45);
}


void lista_declaraciones_init(set folset)
{
	test(first(LISTA_DECLARACIONES_INIT),folset | first(DECLARADOR_INIT) | CCOMA,46);
	match(CIDENT, 17);

	declarador_init(folset | CCOMA | CIDENT | first(DECLARADOR_INIT));

	while(lookahead_in(CCOMA | CIDENT | first(DECLARADOR_INIT)))
	{
		match(CCOMA,64);
		match(CIDENT, 17);
		declarador_init(folset | CCOMA | CIDENT | first(DECLARADOR_INIT));
	}
}


void declaracion_variable(set folset)
{
	declarador_init(folset | CCOMA | first(LISTA_DECLARACIONES_INIT) | CPYCOMA);

	if(lookahead_in(CCOMA | first(LISTA_DECLARACIONES_INIT)))
	{
		match(CCOMA,64);
		lista_declaraciones_init(folset | CPYCOMA);
	}

	match(CPYCOMA, 23);
	test(folset,NADA,51);
}


void declarador_init(set folset)
{
	test(first(DECLARADOR_INIT) | folset, CASIGNAC | CCONS_FLO | CCONS_CAR | CCOR_ABR | CCOR_CIE | CLLA_ABR | CLLA_CIE,47); //Se quito el CCON_ENT y donde aparece (first const y first lis in) (!)
	switch(lookahead())
	{
	    case CCONS_FLO:
	    case CCONS_CAR:
		case CASIGNAC:
			match(CASIGNAC,66);
			constante(folset);
			break;

        case CLLA_ABR:
        case CLLA_CIE:
        case CCOR_CIE:
		case CCOR_ABR:
			match(CCOR_ABR, 35);
			
			if(lookahead_in(CCONS_ENT))
				constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES));

			match(CCOR_CIE, 22);

			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES)))
			{
				match(CASIGNAC,66);
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

	while(lookahead_in(CCOMA | first(CONSTANTE)))
	{
		match(CCOMA,64);
		constante(folset | CCOMA | first(CONSTANTE));
	}
}


void proposicion_compuesta(set folset)
{
	test(first(PROPOSICION_COMPUESTA),folset | first(LISTA_DECLARACIONES) | first(LISTA_PROPOSICIONES) | CLLA_CIE,49);
	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | first(LISTA_PROPOSICIONES) | CLLA_CIE);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE);

	match(CLLA_CIE, 25);
	test(folset,NADA,50);
}


void lista_declaraciones(set folset)
{
	declaracion(folset | first(DECLARACION));

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | first(DECLARACION));
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
		proposicion(folset | first(PROPOSICION));
}


void proposicion(set folset)
{
	test(first(PROPOSICION),folset | CSHL | CSHR /*| first(PROPOSICION_EXPRESION) | first(PROPOSICION_COMPUESTA) | first(PROPOSICION_ITERACION) | first(PROPOSICION_SELECCION) | first(PROPOSICION_RETORNO) | first(PROPOSICION_E_S)*/,52); //c2 puede ya estar en el first que esta en c1 (!)
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

        case CSHL: //Agregado por forzada en proposicion_e_s
        case CSHR: //Agregado por forzada en proposicion_e_s
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
			error_handler(26);
	}
}


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | first(PROPOSICION));

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
	    case CSHR:
		case CIN:
			match(CIN,29);
			
			match(CSHR, 30);
			
			variable(folset | CSHR | first(VARIABLE) | CPYCOMA);
			
			while(lookahead_in(CSHR | first(VARIABLE)))
			{
				match(CSHR,30);
				variable(folset | CPYCOMA | CSHR | first(VARIABLE));
			}

			match(CPYCOMA, 23);
			
			break;

		case CSHL:
		case COUT:
			match(COUT,29);

			match(CSHL, 31);
			
			expresion(folset | CSHL | first(EXPRESION) | CPYCOMA);

			while(lookahead_in(CSHL | first(EXPRESION)))
			{
				match(CSHL,31);
				expresion(folset | CPYCOMA | CSHL | first(EXPRESION));
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset | CSHL | CSHR,NADA,53); //Ver si necesito agregar con el folset el >> y << (!!) arreglar comentario
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
				expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG); //Deberia llevar first de Exp simp??
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG);  //Deberia llevar first de Exp simp??
				break;
		}
	}
}


void expresion_simple(set folset)
{
	test(first(EXPRESION_SIMPLE),folset | COR,56); //+ y - estan en el first y f1 de termino incluido en f1 exp_sim
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset | CMAS | CMENOS | COR | first(TERMINO));

	while(lookahead_in(CMAS | CMENOS | COR | first(TERMINO)))
	{
		match(CMAS | CMENOS | COR,65);
		termino(folset | CMAS | CMENOS | COR | first(TERMINO));
	}
}


void termino(set folset)
{	
	factor(folset | CMULT | CDIV | CAND | first(FACTOR));

	while(lookahead_in(CMULT | CDIV | CAND | first(FACTOR)))
	{
		match(CMULT | CDIV | CAND,65);
		factor(folset | CMULT | CDIV | CAND | first(FACTOR));
	}
}


void factor(set folset)
{
	test(first(FACTOR), folset | CPAR_CIE,57); //Ver si saco CPAR_CIE por lote: if(k==) a;
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
		case CPAR_CIE: //SACAR PAR CIE SI SACO ARRIBA
			match(CPAR_ABR,20);
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
	test(first(VARIABLE),folset | CCOR_ABR,59); //POR SACAR DE ABAJO SACO DE ACA TMB FIRST EXP Y CCOR CIE
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR /*| CCOR_CIE*/)) // (!!!!) Me generaba problemas, muchos errores (Para guardar registro)
	{
		match(CCOR_ABR,35);
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset,NADA,60);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR)) // Incluyo CCOMA? con f(,b)
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 21);
	test(folset,NADA,61);
}


void lista_expresiones(set folset)
{
	expresion(folset | CCOMA | first(EXPRESION));

	while(lookahead_in(CCOMA | first(EXPRESION)))
	{
		match(CCOMA,64);
		expresion(folset | CCOMA | first(EXPRESION));
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
