#include <string.h>
#include "parser.h"


//VER:
//cuando llamo a especificador_tipo usar el tipo
//ver que las funciones que devuelvan algo, mas que nada tipo, lo asignen, consulten o pasen a donde corresponda
//

int TIPOVOID, TIPOCHAR, TIPOINT, TIPOFLOAT, TIPOARREGLO, TIPOERROR;
int ARRINT = -11, ARRFLOAT = -12, ARRCHAR = -13, STRING = -14;

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

    inic_tablas();

    TIPOVOID = en_tabla("void");
    TIPOCHAR = en_tabla("char");
    TIPOINT = en_tabla("int");
    TIPOFLOAT = en_tabla("float");
    TIPOARREGLO = en_tabla("TIPOARREGLO");
    TIPOERROR = en_tabla("TIPOERROR");



    pushTB();//Bloq 0
	
	unidad_traduccion(CEOF);

    if(en_tabla("main") == NIL){ //Y ya esta en tabla
        error_handler(84); //Falta declarar la funcion main()
    }

	pop_nivel();

	//Ver si quiero limpiar el inic tabla

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
    int tipo = especificador_tipo(folset | CIDENT | first(ESPECIFICADOR_DECLARACION));

	strcpy(inf_id->nbre,sbol->lexema);

	match(CIDENT, 17);
	
	especificador_declaracion(folset,tipo);
}



int especificador_tipo(set folset)
{
	test(first(ESPECIFICADOR_TIPO), folset, 41);
	int posicionTipo;
	switch(lookahead())
	{
		case CVOID:
		    posicionTipo = TIPOVOID;
			scanner();
			break;
			
		case CCHAR:
		    posicionTipo = TIPOCHAR;
			scanner();
			break;
			
		case CINT:
		    posicionTipo = TIPOINT;
			scanner();
			break;
			
		case CFLOAT:
		    posicionTipo = TIPOFLOAT;
			scanner();
			break;
			
		default:
		    posicionTipo = TIPOERROR;
			error_handler(18);
	}
    test(folset, NADA, 42);
    return (posicionTipo);
}


void especificador_declaracion(set folset, int tipo)
{
    test(first(ESPECIFICADOR_DECLARACION), folset,43);
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset,tipo);
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
		    if(tipo == TIPOVOID){
		        tipo = TIPOERROR;
		        error_handler(73); //Una variable, un parametro o un arreglo no pueden ser de tipo void
		    }
			declaracion_variable(folset,tipo);
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset,int tipo)
{
    int insertando_main = 0;
    if(strcmp(inf_id->nbre,"main") == 0){ //Si el id de la funcion es main
        insertando_main = 1;
        if(en_tabla("main") != NIL){ //Y ya esta en tabla
            tipo = TIPOERROR;
            error_handler(102); //La funcion main() ya se encuentra declarada
        }
        else if(tipo != TIPOVOID){ //Si el tipo de retorno es distinto a void
            error_handler(85); //El tipo de la funcion main() debe ser void
            tipo = TIPOERROR;
        }
    }
    inf_id->ptr_tipo = tipo;
    inf_id->clase = CLASFUNC;
    int posicionTSF = insertarTS();
    int cantidad_parametros = 0;
	match(CPAR_ABR, 20);

    pushTB(); //Aumento nivel pq los parametros y variables son de mayor nivel

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		cantidad_parametros = lista_declaraciones_param(folset | CPAR_CIE | first(PROPOSICION_COMPUESTA),posicionTSF);

	match(CPAR_CIE, 21);

	//Ir a la pos en la TS y darle el num de params que conte
	if(insertando_main == 1){ //Si el id de la funcion es main
	    if(cantidad_parametros != 0){ //Si main tiene parametros
	        ts[posicionTSF].ets->ptr_tipo = TIPOERROR;
	        error_handler(86); //La funcion main() no lleva argumento
	    }
	}
	ts[posicionTSF].ets->desc.part_var.sub.cant_par = cantidad_parametros;

	proposicion_compuesta(folset,0); //paso 0 para indicar que no hay que hacer un pushTB();
}


int lista_declaraciones_param(set folset, int posicionTSF) //Agrego la posicion para completar param (***-)
{
    int cantidad_parametros = 0;

    tipo_inf_res *ptr_inf_res;
    ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res = (tipo_inf_res *) calloc(1, sizeof(tipo_inf_res));
    ptr_inf_res = ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res;

	declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF, ptr_inf_res);

    cantidad_parametros++;

	while(lookahead_in(CCOMA | first(DECLARACION_PARAMETRO)))
	{
		match(CCOMA,64);
		ptr_inf_res->ptr_sig = (tipo_inf_res *) calloc(1, sizeof(tipo_inf_res));
		ptr_inf_res = ptr_inf_res->ptr_sig;
		declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF, ptr_inf_res);
		cantidad_parametros++;
	}

	return cantidad_parametros;
}


void declaracion_parametro(set folset, int posicionTSF, tipo_inf_res *ptr_inf_res)
{
	int tipo = especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

	if(tipo == TIPOVOID){
	    tipo = TIPOERROR;
	    error_handler(73); //Una variable, un parametro o un arreglo no pueden ser de tipo void
	}

    int control28 = 0, flag28 = 0;

	if(lookahead_in(CAMPER)){
	    scanner();
	    control28 = 1; //Se uso el &, controlar que no venga un arreglo (28)
    }

	strcpy(inf_id->nbre,sbol->lexema);
	inf_id->clase = CLASPAR;
	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
	    if(control28 == 1){
	        flag28 = 1;
	        error_handler(92); //<tipo> & <nombre arreglo> []
	    }
	    else{
	        control28 = 1; //Utilizo este flag para saber que tengo un arreglo
	    }
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
	if(control28 == 1){
	    if(flag28 == 1){
            inf_id->ptr_tipo = TIPOERROR;
            inf_id->desc.part_var.param.ptero_tipo_base = TIPOERROR; //Creo que no hace falta
            ptr_inf_res->ptero_tipo = TIPOERROR;
            ptr_inf_res->ptero_tipo_base = TIPOERROR; //Creo que no hace falta
	    }
	    else{
	        inf_id->desc.part_var.param.ptero_tipo_base = tipo;
            inf_id->ptr_tipo = TIPOARREGLO;
            ptr_inf_res->ptero_tipo = TIPOARREGLO;
            ptr_inf_res->ptero_tipo_base = tipo;
	    }
	}
	else{
	    inf_id->ptr_tipo = tipo;
	    ptr_inf_res->ptero_tipo = tipo;
	}
	if(control28 == 1){
	    //Note that the formal parameter is specified as an array reference using the [] notation, but this notation is not used when an array is passed as an actual parameter.
	    //func(a); no uso el [] pq lo invoco con el nombre
	    inf_id->desc.part_var.param.tipo_pje = 'd';
        ptr_inf_res->tipo_pje = 'd';
    }
    else{
        inf_id->desc.part_var.param.tipo_pje = 'v';
        ptr_inf_res->tipo_pje = 'v';
    }
	insertarTS(); //.Insercion del parametro.

	//Ya estoy sumando la cantidad de parametros mas arriba
	test(folset,NADA,45);
}


void lista_declaraciones_init(set folset, int tipo)
{
	test(first(LISTA_DECLARACIONES_INIT),folset | first(DECLARADOR_INIT) | CCOMA,46);

    strcpy(inf_id->nbre,sbol->lexema);

	match(CIDENT, 17);

	declarador_init(folset | CCOMA | CIDENT | first(DECLARADOR_INIT), tipo);

	while(lookahead_in(CCOMA | CIDENT | first(DECLARADOR_INIT)))
	{
		match(CCOMA,64);
		strcpy(inf_id->nbre,sbol->lexema);
		match(CIDENT, 17);
		declarador_init(folset | CCOMA | CIDENT | first(DECLARADOR_INIT), tipo);
	}
}


void declaracion_variable(set folset, int tipo)
{
	declarador_init(folset | CCOMA | first(LISTA_DECLARACIONES_INIT) | CPYCOMA, tipo);

	if(lookahead_in(CCOMA | first(LISTA_DECLARACIONES_INIT)))
	{
		match(CCOMA,64);
		lista_declaraciones_init(folset | CPYCOMA,tipo);
	}

	match(CPYCOMA, 23);
	test(folset,NADA,51);
}


void declarador_init(set folset, int tipo)
{
	test(first(DECLARADOR_INIT) | folset, CASIGNAC | CCONS_FLO | CCONS_CAR | CCOR_ABR | CCOR_CIE | CLLA_ABR | CLLA_CIE,47); //Se quito el CCON_ENT y donde aparece (first const y first lis in) (!)

	int dimension = -1, sin_dimension = -1, tipo2;

    inf_id->ptr_tipo = tipo;

	switch(lookahead())
	{
	    case CCONS_FLO:
	    case CCONS_CAR:
		case CASIGNAC:
			match(CASIGNAC,66);
			tipo2 = constante(folset);
			if(tipo != tipo2){
			    //printf("\n\n");
			    //printf("\n\n");
			    //printf("REVISAR SI ES A UN ARREGLO QUE SEAN MISMO TIPO BASE");
			    //printf("\n\n");
			    //printf("\n\n");
			    //printf("Tipo: %d \n",tipo);
			    //printf("Tipo2: %d \n",tipo2);
			    // BORRAR
                tipo = TIPOERROR;
                error_handler(83); //Los tipos de ambos lados de la asignacion deben ser estructuralmente equivalentes
            }
			inf_id->ptr_tipo = tipo;
			break;

        case CLLA_ABR:
        case CLLA_CIE:
        case CCOR_CIE:
		case CCOR_ABR:
		    if((tipo == TIPOINT) || (tipo == TIPOFLOAT) || (tipo == TIPOCHAR)){
		        inf_id->ptr_tipo = TIPOARREGLO;
		        inf_id->desc.part_var.arr.ptero_tipo_base = tipo;
		    }
		    else{
                printf("\n\n");
                printf("\n\n");
                printf("DECLARADOR_INIT VER QUE NO ESTE REPETIDO ESTE ERROR");
                printf("\n\n");
                printf("\n\n");
		        inf_id->ptr_tipo = TIPOERROR;
		        error_handler(103); //Un arreglo debe ser de tipo simple
		    }

			match(CCOR_ABR, 35);

			if(lookahead_in(CCONS_ENT)){
			    dimension = atoi(sbol->lexema);
			    if(dimension <= 0){
			        error_handler(75); //La cantidad de elementos de un arreglo puede estar dada por un número natural (es decir, mayor a 0) y/o a través de la inicialización del mismo.

			    }
			    else{

			        inf_id->desc.part_var.arr.cant_elem = atoi(sbol->lexema);
                }
			    constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES));
			}
			else if(lookahead_in(CCOR_CIE)){
			    sin_dimension = 1;
			}
			else{
                error_handler(74); //La cdad. de elementos de un arreglo debe ser una cte. entera en la declaracion
            }

			match(CCOR_CIE, 22);

            int cantidad_elementos = 0;
			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES)))
			{
				match(CASIGNAC,66);
				match(CLLA_ABR, 24);
				cantidad_elementos = lista_inicializadores(folset | CLLA_CIE, tipo);

				if (cantidad_elementos == 0){
                    error_handler(101); //La cantidad de valores inicializadores no puede ser 0
                }

				if(sin_dimension == 1){
				    inf_id->desc.part_var.arr.cant_elem = cantidad_elementos;
				}
				else if((dimension > 0) && (cantidad_elementos > dimension)){
				    error_handler(76); // La cantidad de valores inicializadores debe ser igual o menor al tamaño del arreglo declarado
				}
				else{
				    inf_id->desc.part_var.arr.cant_elem = dimension;
				}
				match(CLLA_CIE, 25);
			}
			break;
	}
	inf_id->clase = CLASVAR;
	insertarTS();
	test(folset,NADA,48);
}


int lista_inicializadores(set folset, int tipo_base)
{
    int tipo_constante, cantidad_elementos = 0;
	tipo_constante = constante(folset | CCOMA | first(CONSTANTE));
	if (tipo_constante != 5){
	    cantidad_elementos++;
	}

	if(tipo_constante != tipo_base){
	    //printf("\nTipo constante: %d" , tipo_constante); CONTROLES POR ALGO QUE FALLABA, BORRAR
	    //printf("\nTipo base: %d" , tipo_base); CONTROLES POR ALGO QUE FALLABA, BORRAR
	    error_handler(77); //El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion
	}

	while(lookahead_in(CCOMA | first(CONSTANTE)))
	{
		match(CCOMA,64);
		tipo_constante = constante(folset | CCOMA | first(CONSTANTE));

	    if(tipo_constante != tipo_base){
            //printf("\nif while"); CONTROLES POR ALGO QUE FALLABA, BORRAR
	        //printf("\nTipo constante: %d" , tipo_constante); CONTROLES POR ALGO QUE FALLABA, BORRAR
            //printf("\nTipo base: %d" , tipo_base); CONTROLES POR ALGO QUE FALLABA, BORRAR
            error_handler(77); //El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion
        }
		cantidad_elementos++;
	}
	return cantidad_elementos;
}


void proposicion_compuesta(set folset,int abrir_bloque)
{
    if(abrir_bloque==1){
        pushTB();
    }
	test(first(PROPOSICION_COMPUESTA),folset | first(LISTA_DECLARACIONES) | first(LISTA_PROPOSICIONES) | CLLA_CIE,49);
	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | first(LISTA_PROPOSICIONES) | CLLA_CIE);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CSHL | CSHR | // Agregados CSHL y CSHR para reconocer que falta CIN y COUT (error: void main(){<< "hola";})
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE);

	match(CLLA_CIE, 25);
	pop_nivel();
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
	int tipo = especificador_tipo(folset | first(LISTA_DECLARACIONES_INIT) | CPYCOMA);

	if(tipo == TIPOVOID){
	    tipo = TIPOERROR;
	    error_handler(73); //Una variable, un parametro o un arreglo no pueden ser de tipo void
	}

	lista_declaraciones_init(folset | CPYCOMA,tipo);

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
			proposicion_compuesta(folset,1);
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

	expresion(folset | CPAR_CIE | first(PROPOSICION),1);

	match(CPAR_CIE, 21);

	proposicion(folset);
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | first(PROPOSICION) | CELSE,1);

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
			
			variable(folset | CSHR | first(VARIABLE) | CPYCOMA,1);
			
			while(lookahead_in(CSHR | first(VARIABLE)))
			{
				match(CSHR,30);
				variable(folset | CPYCOMA | CSHR | first(VARIABLE),1);
			}

			match(CPYCOMA, 23);
			
			break;

		case CSHL:
		case COUT:
			match(COUT,29);

			match(CSHL, 31);
			
			expresion(folset | CSHL | first(EXPRESION) | CPYCOMA,1);

			while(lookahead_in(CSHL | first(EXPRESION)))
			{
				match(CSHL,31);
				expresion(folset | CPYCOMA | CSHL | first(EXPRESION),1);
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset | CSHL | CSHR,NADA,53); //agregados el >> y << porque se fuerza entrada en case de arriba
}


void proposicion_retorno(set folset)
{
	scanner();
	
	expresion(folset | CPYCOMA,1);
	
	match(CPYCOMA, 23);
	test(folset,NADA,54);
}


void proposicion_expresion(set folset)
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA,1);

	match(CPYCOMA, 23);
	test(folset,NADA,55);
}


int expresion(set folset, int necesito_indice)
{
    int tipo, tipo2;
	tipo = expresion_simple(folset | CASIGNAC | first(EXPRESION_SIMPLE) | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, necesito_indice);

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				tipo2 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, 1);
				if(tipo != tipo2){
				    //VER ACA COERCION
				    tipo = TIPOERROR;
				    error_handler(83); //Los tipos de ambos lados de la asignacion deben ser estructuralmente equivalentes
				}
				else{
				    if(tipo == TIPOARREGLO){ //Por si devuelve tipo arreglo, en si deberia entrar por los 3 de abajo
				        tipo = TIPOERROR;
                        error_handler(81); //No se permite la asignacion de arreglos como un todo
				    }
				    else if(tipo == ARRINT){
				        tipo = TIPOERROR;
				        error_handler(81); //No se permite la asignacion de arreglos como un todo
				    }
				    else if(tipo == ARRFLOAT){
				        tipo = TIPOERROR;
                        error_handler(81); //No se permite la asignacion de arreglos como un todo
				    }
				    else if(tipo == ARRCHAR){
				        tipo = TIPOERROR;
                        error_handler(81); //No se permite la asignacion de arreglos como un todo
				    }
				    else{ //Hecho por las dudas en caso que quiera asignar TIPOERROR o TIPOVOID x ej
				        if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
				            tipo = TIPOERROR;
				            error_handler(104); //Tipo de la asignacion no valido
				        }
				    }
				}
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				tipo2 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, 1);
				if(tipo != tipo2){
				    // COERCION Ver si la implemento en futuro
                    tipo = TIPOERROR;
                    error_handler(96); //Los operandos de los operadores logicos o relacionales solo pueden ser de tipo char, int o float
                }
                else{
                    if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                        tipo = TIPOERROR;
                        error_handler(96); //Los operandos de los operadores logicos o relacionales solo pueden ser de tipo char, int o float
                    }
                }
				break;
		}
	}
	return tipo;
}


int expresion_simple(set folset, int necesito_indice)
{
    int tipo, tipo2;
	test(first(EXPRESION_SIMPLE),folset | COR,56);
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	tipo = termino(folset | CMAS | CMENOS | COR | first(TERMINO), necesito_indice);

	while(lookahead_in(CMAS | CMENOS | COR | first(TERMINO)))
	{
		match(CMAS | CMENOS | COR,65);
		tipo2 = termino(folset | CMAS | CMENOS | COR | first(TERMINO), 1);

		if(tipo != tipo2){
            // COERCION Ver si la implemento en futuro
            tipo = TIPOERROR;
            error_handler(105); //Los tipos de ambos lados de los operadores logicos o aritmeticos deben ser estructuralmente equivalentes
        }
        else{
            if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                tipo = TIPOERROR;
                error_handler(106); //Los operandos de los operadores logicos o aritmeticos solo pueden ser de tipo char, int o float
            }
        }
	}
	return tipo;
}


int termino(set folset, int necesito_indice)
{
    int tipo, tipo2;
	tipo = factor(folset | CMULT | CDIV | CAND | first(FACTOR), necesito_indice);

	while(lookahead_in(CMULT | CDIV | CAND | first(FACTOR)))
	{
		match(CMULT | CDIV | CAND,65);
		tipo2 = factor(folset | CMULT | CDIV | CAND | first(FACTOR), 1);
		if(tipo != tipo2){
            // COERCION Ver si la implemento en futuro
            tipo = TIPOERROR;
            error_handler(105); //Los tipos de ambos lados de los operadores logicos o aritmeticos deben ser estructuralmente equivalentes
        }
        else{
            if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                tipo = TIPOERROR;
                error_handler(106); //Los operandos de los operadores logicos o aritmeticos solo pueden ser de tipo char, int o float
            }
        }
	}
	return tipo;
}


int factor(set folset, int necesito_indice)
{
	test(first(FACTOR), folset /*| CPAR_CIE*/,57); //Ver si saco CPAR_CIE por lote: if(k==) a;
	/** Lo tuve que sacar por lote propio: lote3ce.c en linea 21: f1(ab,); se colgaba la ejecucion **/
	int tipo;

	switch(lookahead())
	{
		case CIDENT:
		    if(en_tabla(sbol->lexema) == NIL){ //COMPARO CON NIL PQ EN DEFINICION NIL ES -1
		        error_handler(71); //Identificador no declarado
		        strcpy(inf_id->nbre,sbol->lexema);
		        inf_id->ptr_tipo = TIPOERROR;
		        insertarTS();
		        // (***-) Ver si falta mas info que poner
		    }
		    else if(Clase_Ident(sbol->lexema) == CLASFUNC){
		        tipo = llamada_funcion(folset);
		    }
		    else{
		        tipo = variable(folset, necesito_indice);
		    }

		    //Tomar nota de aca, si no es funcion, lo mando a variable, si queria usar una funcion, me va a tirar error por variable en vez de funcion
		    //Informar en el informe que se hace esto
		    //Ademas, sino, podria traer el match antes del else if y de ahi ver si tiene un ( el lookahead o no y de ahi tratarlo.
		    //Aca no van a esperar un identificador ni llamada a funcion ni variable

		    /*else{
		        error_handler(10);
		        printf("\n\nError en factor -> switch -> case CIDENT -> else \n\n");
		        //Tendre un caso que entre aca? Revisar
		    }*/ //Despreciado ya que se mando el error a variable BORRAR cuando ponga lo de arriba en el informe
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			tipo = constante(folset);
			break;
		
		case CCONS_STR:
		    tipo = STRING; //PARA REVISAR EN COUT QUE LA EXPRESION SEA UN STRING, ENTONCES ES VALIDO
			scanner();
			break;
		
		case CPAR_ABR:
		//case CPAR_CIE: //SACAR PAR CIE SI SACO ARRIBA
			match(CPAR_ABR,20);
			tipo = expresion(folset | CPAR_CIE,1);
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			tipo = expresion(folset,1);
			break;
			
		default:
		    tipo = TIPOERROR;
			error_handler(32);
	}
	test(folset,NADA,58);
	return tipo;
}


int variable(set folset, int necesito_indice)
{
	test(first(VARIABLE),folset | CCOR_ABR,59); //POR SACAR DE ABAJO SACO DE ACA TMB FIRST EXP Y CCOR CIE
	int tipo = Tipo_Ident(sbol->lexema), flag = 0;
	if(tipo == TIPOARREGLO){
	    flag = 1;
	    tipo = ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base;

	    /**No pude hacerlo con un switch ya que TIPOINT, TIPOFLOAT y TIPOCHAR se le asignan valores en ejecucion**/
	    if(tipo == TIPOINT){
	        tipo = ARRINT;
	    }
	    else if(tipo == TIPOFLOAT){
            tipo = ARRFLOAT;
        }
        else if(tipo == TIPOCHAR){
            tipo = ARRCHAR;
        }
        else{
            tipo = TIPOERROR;
        }

	}
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR /*| CCOR_CIE*/)) // (!!!!) Me generaba problemas, muchos errores (Para guardar registro) BORRAR
	{
	    if(flag != 1){ //Veo si no tengo un arreglo
            tipo = TIPOERROR;
            error_handler(78); //La variable no es de tipo arreglo
        }
		match(CCOR_ABR,35);
		expresion(folset | CCOR_CIE,1); //Controlar que no uso el tipo de esta expresion, solo despues BORRAR ,onda el tipo deberia ser int
		match(CCOR_CIE, 22);
	}
	else{ //Si no hay un CCOR_ABR
	    if(flag == 1){ //Veo si tengo un arreglo
	        if(necesito_indice == 1){
	            tipo = TIPOERROR;
                error_handler(79); //En una expresion los arreglos deben ser accedidos por sus elementos
	        }
	    }
	}
	test(folset,NADA,60);
	return tipo;
}


int llamada_funcion(set folset)
{
    int posicionTSF = en_tabla(sbol->lexema);
    int cantidad_parametros_formales = ts[posicionTSF].ets->desc.part_var.sub.cant_par, cantidad_parametros_actuales = 0;
    int tipo = ts[posicionTSF].ets->ptr_tipo;

	match(CIDENT, 17);

	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		cantidad_parametros_actuales = lista_expresiones(folset | CPAR_CIE, posicionTSF);

    if(cantidad_parametros_formales != cantidad_parametros_actuales){
        tipo = TIPOERROR;
        error_handler(90); //La CANTIDAD de parametros actuales no coincide con la cantidad de parametros formales
    }

	match(CPAR_CIE, 21);
	test(folset,NADA,61);
	return tipo;
}


int lista_expresiones(set folset, int posicionTSF)
{
    int flag = 0, cantidad_parametros_actuales = 0;
    tipo_inf_res *ptr_inf_res;
    int tipo_parametro_formal, tipo_parametro_formal_base;
    int tipo_parametro_actual, tipo_parametro_actual_base;

    if(ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res != NULL){ //Veo que tenga definido parametros
        ptr_inf_res = ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res; //Recupero el puntero de parametros
        tipo_parametro_formal = ptr_inf_res->ptero_tipo; //Recupero el tipo de primer parametro formal
        if(tipo_parametro_formal == TIPOARREGLO){
            tipo_parametro_formal_base = ptr_inf_res->ptero_tipo_base;
        }
        else{
            tipo_parametro_formal_base = -1;
        }
    }
    else{
        tipo_parametro_formal = TIPOERROR; //Por si no tengo definidos parametros
    }

	tipo_parametro_actual = expresion(folset | CCOMA | first(EXPRESION), 0);
	cantidad_parametros_actuales++;

	if(tipo_parametro_actual == -11){
	    tipo_parametro_actual = TIPOARREGLO;
	    tipo_parametro_actual_base = TIPOINT;
	}
	else if(tipo_parametro_actual == -12){
        tipo_parametro_actual = TIPOARREGLO;
        tipo_parametro_actual_base = TIPOFLOAT;
    }
    else if(tipo_parametro_actual == -12){
        tipo_parametro_actual = TIPOARREGLO;
        tipo_parametro_actual_base = TIPOCHAR;
    }
    else{
        tipo_parametro_actual_base = -1;
    }

	if((tipo_parametro_formal != tipo_parametro_actual) || (tipo_parametro_formal_base != tipo_parametro_actual_base)){
	    flag = 1;
	}

	if(tipo_parametro_actual == TIPOARREGLO){
        if(ptr_inf_res->tipo_pje != 'd'){ //8/10 DECIDI CAMBIAR ESTE CONTROL DE 'v' a 'd' pq no tiene sentido preguntar por v, un arreglo se pasa por direccion, no por valor, si lo inserto en TS como un pasaje por valor esta mal BORRAR ATENCION Osea, para mi esta mal el 27, tendria que decir direccion en vez de valor
            error_handler(98); //Si el parametro formal es un arreglo, en el parametro real solo debe haber un identificador
        }
	}

	while(lookahead_in(CCOMA | first(EXPRESION)))
	{
		match(CCOMA,64);
		if(ptr_inf_res->ptr_sig != NULL){ //Si sigo teniendo parametros
		    ptr_inf_res = ptr_inf_res->ptr_sig; //Asigno el siguiente al puntero de parametros
		    tipo_parametro_formal = ptr_inf_res->ptero_tipo;
		    if(tipo_parametro_formal == TIPOARREGLO){
                tipo_parametro_formal_base = ptr_inf_res->ptero_tipo_base;
            }
            else{
                tipo_parametro_formal_base = -1;
            }
		}
		else{ //Tengo mas parametros actuales que formales
		    tipo_parametro_formal = TIPOERROR;
		}

		tipo_parametro_actual = expresion(folset | CCOMA | first(EXPRESION), 0);
		cantidad_parametros_actuales++;

        if(tipo_parametro_actual == -11){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOINT;
        }
        else if(tipo_parametro_actual == -12){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOFLOAT;
        }
        else if(tipo_parametro_actual == -12){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOCHAR;
        }
        else{
            tipo_parametro_actual_base = -1;
        }

        if((tipo_parametro_formal != tipo_parametro_actual) || (tipo_parametro_formal_base != tipo_parametro_actual_base)){
            flag = 1;
        }
	}

	if(flag != 0){
	    error_handler(91); //El TIPO de los parametros actuales no coincide con el de los parametros formales
	}
	return cantidad_parametros_actuales;
}


int constante(set folset)
{
    int tipo;
	test(first(CONSTANTE),folset,62);
	switch(lookahead())
	{
		case CCONS_ENT:
		    tipo = TIPOINT;
			scanner();
			break;
		
		case CCONS_FLO:
		    tipo = TIPOFLOAT;
			scanner();
			break;
		
		case CCONS_CAR:
		    tipo = TIPOCHAR;
			scanner();
			break;
		
		default:
		    tipo = TIPOERROR;
			error_handler(33);
	}
	test(folset,NADA,63);
	return tipo;
}
