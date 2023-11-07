#include <string.h>
#include "parser.h"
#include "sistejec.h"


int TIPOVOID, TIPOCHAR, TIPOINT, TIPOFLOAT, TIPOARREGLO, TIPOERROR; //Variables que contendran el valor en tabla del tipo que sugiere su nombre
int ARRCHAR = -41, ARRINT = -42, ARRFLOAT = -43, STRING = -14, VARCHAR = -11, VARINT = -12, VARFLOAT = -13; //Valores auxiliares para facilitar el uso y comparacion en arreglos y strings
 //Los anteriores se implementan en valores negativos para que no conflictuen con valores devueltos por la pila de la tabla de simbolos

float CODE[TAM_PROG];
int libreCODE = 0;           				// próximo libre del programa

char CAUX[TAM_CTES];
int libreCAUX = 0; //Si uso strcat esto en 0 o ir agregando pos a pos del tama'o del lexema (si hago esta segunda borrar la inicializacion de arriba)

char lexema_aux_izq[200]; //Para guardar el lexema del lado izquierdo a un operador por si es lado izquierdo de una asignacion.
int guarde_variable = 0;
int desplazamiento = 0;

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
    char lote[100];
    strcpy(lote, argv[2]);
    int length = strlen(lote);
    lote[length - 1] = 'o';

	init_parser(argc, argv);

    // Manejo inicial de la tabla de simbolos
    inic_tablas();
    TIPOVOID = en_tabla("void");
    TIPOCHAR = en_tabla("char");
    TIPOINT = en_tabla("int");
    TIPOFLOAT = en_tabla("float");
    TIPOARREGLO = en_tabla("TIPOARREGLO");
    TIPOERROR = en_tabla("TIPOERROR");

    CODE[libreCODE++] = INPP;

    pushTB();//Bloq 0

	CODE[libreCODE++] = ENBL;
	CODE[libreCODE++] = get_nivel();

	unidad_traduccion(CEOF);

    if(en_tabla("main") == NIL){ //Control para revisar que se haya creado una funcion main
        error_handler(84); //Falta declarar la funcion main()
    }
    CODE[libreCODE++] = FINB;
    CODE[libreCODE++] = get_nivel();

	pop_nivel();

	//Aqui se podria limpiar lo que queda por la ejecucion del procedimiento inic_tablas()

	match(CEOF, 9);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	if(GEN){
        CODE[libreCODE] = PARAR;
        guardar_codgen(CODE, CAUX, lote);
        cargar_codgen(lote);
        //aca recordar guardar codigo generado, esto era llamar funcion del sistejec
        //Lo llamo con code, con caux y el argv[2] del main que tiene el nombre del archivo, capaz que tengo que parsear, iterar hasta encontrar el . y ahi reemplazo .c por .o y ahi lo puedo pasar
    }

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
			error_handler(18); //Tipo no definido
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
        if(tipo != TIPOVOID){ //Si el tipo de retorno es distinto a void
            tipo = TIPOERROR;
            error_handler(85); //El tipo de la funcion main() debe ser void
        }
    }
    inf_id->ptr_tipo = tipo;
    inf_id->clase = CLASFUNC;
    int posicionTSF = insertarTS();
    int cantidad_parametros = 0;
	match(CPAR_ABR, 20);

    pushTB(); //Aumento nivel para insertar correctamente los parametros y variables de la funcion

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		cantidad_parametros = lista_declaraciones_param(folset | CPAR_CIE | first(PROPOSICION_COMPUESTA),posicionTSF);

	match(CPAR_CIE, 21);

	if(insertando_main == 1){ //Si el id de la funcion es main
	    if(cantidad_parametros != 0){ //Si main tiene parametros
	        ts[posicionTSF].ets->ptr_tipo = TIPOERROR;
	        error_handler(86); //La funcion main() no lleva argumento
	    }
	}
	ts[posicionTSF].ets->desc.part_var.sub.cant_par = cantidad_parametros;

	proposicion_compuesta(folset,0); //paso 0 para indicar que no hay que hacer un pushTB();
}


int lista_declaraciones_param(set folset, int posicionTSF)
{
    int cantidad_parametros = 0; //Variable que se retornara indicando la cantidad de parametros insertados

    tipo_inf_res *ptr_inf_res; //Utilizo un puntero auxiliar que contendra la direccion del puntero de la correspondiente funcion insertada en la TS
    ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res = (tipo_inf_res *) calloc(1, sizeof(tipo_inf_res));
    ptr_inf_res = ts[posicionTSF].ets->desc.part_var.sub.ptr_inf_res; //Solicitud de memoria y actualizacion de puntero que mantendra la informacion de los parametros

	declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF, ptr_inf_res);

    cantidad_parametros++;

	while(lookahead_in(CCOMA | first(DECLARACION_PARAMETRO)))
	{
		match(CCOMA,64);
		ptr_inf_res->ptr_sig = (tipo_inf_res *) calloc(1, sizeof(tipo_inf_res));
		ptr_inf_res = ptr_inf_res->ptr_sig; //Solicitud de memoria y actualizacion de puntero que mantendra la informacion de los parametros
		declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF, ptr_inf_res);
		cantidad_parametros++;
	}

	return cantidad_parametros;
}


void declaracion_parametro(set folset, int posicionTSF, tipo_inf_res *ptr_inf_res)
{
	int tipo = especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

    if(tipo == TIPOERROR){
        error_handler(41); //Simbolo inesperado o falta especificador de tipo
    }

	if(tipo == TIPOVOID){
	    tipo = TIPOERROR;
	    error_handler(73); //Una variable, un parametro o un arreglo no pueden ser de tipo void
	}

    int control28 = 0, flag_Arreglo = 0;

	if(lookahead_in(CAMPER)){
	    scanner();
	    control28 = 1; //Se uso el &, controlar que no venga un arreglo (28)
	    inf_id->desc.part_var.param.tipo_pje = 'd';
        ptr_inf_res->tipo_pje = 'd';
    }
    else{
        inf_id->desc.part_var.param.tipo_pje = 'v';
        ptr_inf_res->tipo_pje = 'v';
    }

	strcpy(inf_id->nbre,sbol->lexema);
	inf_id->clase = CLASPAR;
	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
	    if(control28 == 1){
	        error_handler(92); //No se permite <tipo> & <id_arreglo> [] en la definicion de un parametro
	        inf_id->ptr_tipo = TIPOERROR;
            ptr_inf_res->ptero_tipo = TIPOERROR;
            inf_id->desc.part_var.param.ptero_tipo_base = TIPOERROR; //Creo que no hace falta
            ptr_inf_res->ptero_tipo_base = TIPOERROR; //Creo que no hace falta
	    }
	    else{
	        flag_Arreglo = 1;
	        inf_id->ptr_tipo = TIPOARREGLO;
            inf_id->desc.part_var.param.ptero_tipo_base = tipo;
            ptr_inf_res->ptero_tipo = TIPOARREGLO;
            ptr_inf_res->ptero_tipo_base = tipo;
	    }
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
	if(flag_Arreglo != 1){
	    inf_id->ptr_tipo = tipo;
        ptr_inf_res->ptero_tipo = tipo;
	}

	insertarTS(); //Insercion del parametro como variable.
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
    if(tipo != TIPOARREGLO){
        if(tipo == TIPOCHAR){ //No me deja switch
            inf_id->cant_byte = sizeof(char);
        }
        else if(tipo == TIPOINT){
            inf_id->cant_byte = sizeof(int);
        }
        else if(tipo == TIPOFLOAT){
            inf_id->cant_byte = sizeof(float);
        }
        inf_id->desc.nivel = get_nivel();
        inf_id->desc.despl = desplazamiento;
        desplazamiento += inf_id->cant_byte;
        if(GEN){
            CODE[libreCODE++] = ALOC;
            CODE[libreCODE++] = inf_id->cant_byte;
        }
    }

	switch(lookahead())
	{
	    case CCONS_FLO:
	    case CCONS_CAR:
		case CASIGNAC:
			match(CASIGNAC,66);
			tipo2 = constante(folset);
			if(tipo != tipo2){
                tipo = TIPOERROR;
                error_handler(83); //Los tipos de ambos lados de la asignacion deben ser estructuralmente equivalentes
            }
			inf_id->ptr_tipo = tipo;
			if(GEN){
			    CODE[libreCODE++] = ALM;
                CODE[libreCODE++] = inf_id->desc.nivel;
                CODE[libreCODE++] = inf_id->desc.despl;
                CODE[libreCODE++] = inf_id->cant_byte;
			}
			break;
        case CLLA_ABR:
        case CLLA_CIE:
        case CCOR_CIE:
		case CCOR_ABR:
		    if((tipo == TIPOINT) || (tipo == TIPOFLOAT) || (tipo == TIPOCHAR)){
		        inf_id->ptr_tipo = TIPOARREGLO;
		        inf_id->desc.part_var.arr.ptero_tipo_base = tipo;
		    } //Se desprecio el else ya que era ambiguo.

			match(CCOR_ABR, 35);

			if(lookahead_in(CCONS_ENT)){
			    dimension = atoi(sbol->lexema);
			    if(dimension <= 0){
			        inf_id->ptr_tipo = TIPOERROR;
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
			    inf_id->ptr_tipo = TIPOERROR;
                error_handler(74); //La cdad. de elementos de un arreglo debe ser una cte. entera en la declaracion
            }

			match(CCOR_CIE, 22);

            int cantidad_elementos = 0;
			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES)))
			{
				match(CASIGNAC,66);
				match(CLLA_ABR, 24);
				cantidad_elementos = lista_inicializadores(folset | CLLA_CIE, tipo);

				if(cantidad_elementos == -1){ //El error se indica en lista_inicializadores
				    inf_id->ptr_tipo = TIPOERROR;
				}
				else if (cantidad_elementos == 0){
				    inf_id->ptr_tipo = TIPOERROR;
                    error_handler(101); //La cantidad de valores inicializadores no puede ser 0
                }

				if(sin_dimension == 1){
				    inf_id->desc.part_var.arr.cant_elem = cantidad_elementos;
				}
				else if((dimension > 0) && (cantidad_elementos > dimension)){
				    inf_id->ptr_tipo = TIPOERROR;
				    error_handler(76); // La cantidad de valores inicializadores debe ser igual o menor al tamaño del arreglo declarado
				}
				else{
				    inf_id->desc.part_var.arr.cant_elem = dimension;
				}
				match(CLLA_CIE, 25);
			}
			if(cantidad_elementos == 0 && sin_dimension == 1){
			    inf_id->ptr_tipo = TIPOERROR;
			    error_handler(75); //La cantidad de elementos de un arreglo debe ser mayor a 0
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
	    error_handler(77); //El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion
	    return -1; //Salgo para indicar el error
	}

	while(lookahead_in(CCOMA | first(CONSTANTE)))
	{
		match(CCOMA,64);
		tipo_constante = constante(folset | CCOMA | first(CONSTANTE));

	    if(tipo_constante != tipo_base){
            error_handler(77); //El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion
            return -1; //Salgo para indicar el error
        }
		cantidad_elementos++;
	}
	return cantidad_elementos;
}


void proposicion_compuesta(set folset,int abrir_bloque)
{
	test(first(PROPOSICION_COMPUESTA),folset | first(LISTA_DECLARACIONES) | first(LISTA_PROPOSICIONES) | CLLA_CIE,49);

	if(abrir_bloque==1){ //Por si vengo de proposicion y necesito incrementar el nivel
        pushTB();
    }

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
	test(first(PROPOSICION),folset | CSHL | CSHR ,52); //Se restan elementos en c2 ya que se pueden encontrar en c1
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
    int tipo;

	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	tipo = expresion(folset | CPAR_CIE | first(PROPOSICION),1,0);
	if(tipo == VARCHAR){
        tipo = TIPOCHAR;
    }
    else if(tipo == VARINT){
        tipo = TIPOINT;
    }
    else if(tipo == VARFLOAT){
        tipo = TIPOFLOAT;
    }

    if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT)){
        error_handler(97); //Las condiciones de las prop. de seleccion e iteracion solo pueden ser de tipo char, int y float
    }

	match(CPAR_CIE, 21);

	proposicion(folset);
}


void proposicion_seleccion(set folset)
{
    int tipo;

	match(CIF, 28);

	match(CPAR_ABR, 20);

	tipo = expresion(folset | CPAR_CIE | first(PROPOSICION) | CELSE,1,0);

	if(tipo == VARCHAR){
        tipo = TIPOCHAR;
    }
    else if(tipo == VARINT){
        tipo = TIPOINT;
    }
    else if(tipo == VARFLOAT){
        tipo = TIPOFLOAT;
    }

    if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT)){
        error_handler(97); //Las condiciones de las prop. de seleccion e iteracion solo pueden ser de tipo char, int y float
    }

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
    int tipo;
	switch(lookahead())
	{
	    case CSHR:
		case CIN:
			match(CIN,29);
			
			match(CSHR, 30);
			if(en_tabla(sbol->lexema) == NIL){ //en_tabla devuelve NIL (-1) si no esta en tabla
                error_handler(71); //Identificador no declarado
                strcpy(inf_id->nbre,sbol->lexema);
                inf_id->ptr_tipo = TIPOERROR;
                tipo = TIPOERROR;
                inf_id->clase = -1; //Le asigno una clase incorrecta
                insertarTS();
            }

			tipo = variable(folset | CSHR | first(VARIABLE) | CPYCOMA,1,1); //BORRAR ACA PUSE UN 1 PORQUE TENGO QUE ASIGNAR CREO
			int tam_tipo;
			if(tipo == VARCHAR){
                tipo = TIPOCHAR;
                tam_tipo = sizeof(char);
            }
            else if(tipo == VARINT){
                tipo = TIPOINT;
                tam_tipo = sizeof(int);
            }
            else if(tipo == VARFLOAT){
                tipo = TIPOFLOAT;
                tam_tipo = sizeof(float);
            }

			if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT)){
			    error_handler(95); //Las proposiciones de E/S solo aceptan variables y/o expresiones de tipo char, int y float
			}
			if(GEN){
			    CODE[libreCODE++] = LEER;
			    CODE[libreCODE++] = tam_tipo;
			    CODE[libreCODE++] = ALM;
			    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.nivel;
			    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.despl;
			    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->cant_byte;
			}
			while(lookahead_in(CSHR | first(VARIABLE)))
			{
				match(CSHR,30);
				tipo = variable(folset | CPYCOMA | CSHR | first(VARIABLE),1,1); //BORRAR ACA PUSE UN 1 PORQUE TENGO QUE ASIGNAR CREO
                int tam_tipo;
                if(tipo == VARCHAR){
                    tipo = TIPOCHAR;
                    tam_tipo = sizeof(char);
                }
                else if(tipo == VARINT){
                    tipo = TIPOINT;
                    tam_tipo = sizeof(int);
                }
                else if(tipo == VARFLOAT){
                    tipo = TIPOFLOAT;
                    tam_tipo = sizeof(float);
                }
				if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT)){
                    error_handler(95); //Las proposiciones de E/S solo aceptan variables y/o expresiones de tipo char, int y float
                }
                if(GEN){
                    CODE[libreCODE++] = LEER;
                    CODE[libreCODE++] = tam_tipo;
                    CODE[libreCODE++] = ALM;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.nivel;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.despl;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->cant_byte;
                }
			}

			match(CPYCOMA, 23);
			break;

		case CSHL:
		case COUT:
			match(COUT,29);

			match(CSHL, 31);
			
			tipo = expresion(folset | CSHL | first(EXPRESION) | CPYCOMA,1,0);
			if(tipo == VARCHAR){
                tipo = TIPOCHAR;
            }
            else if(tipo == VARINT){
                tipo = TIPOINT;
            }
            else if(tipo == VARFLOAT){
                tipo = TIPOFLOAT;
            }

			if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != STRING)){ //DUDA: Aqui deberia hacer algo mas que marcar el error?
                error_handler(95); //Las proposiciones de E/S solo aceptan variables y/o expresiones de tipo char, int y float
            }


            /*Ver donde acomodo*/
            if(tipo == STRING){

            }



			while(lookahead_in(CSHL | first(EXPRESION)))
			{
				match(CSHL,31);
				tipo = expresion(folset | CPYCOMA | CSHL | first(EXPRESION),1,0);
				if((tipo != TIPOCHAR) && (tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != STRING)){ //DUDA: Aqui deberia hacer algo mas que marcar el error?
                    error_handler(95); //Las proposiciones de E/S solo aceptan variables y/o expresiones de tipo char, int y float
                }
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
	
	expresion(folset | CPYCOMA,1,0);
	
	match(CPYCOMA, 23);
	test(folset,NADA,54);
}


void proposicion_expresion(set folset)
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA,1,1);

	match(CPYCOMA, 23);
	test(folset,NADA,55);
}


int expresion(set folset, int necesito_indice, int posible_asignacion)
{
    int tipo, tipo2, cargue_izquierdo = 0;
	tipo = expresion_simple(folset | CASIGNAC | first(EXPRESION_SIMPLE) | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, necesito_indice, posible_asignacion);

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				tipo2 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, 1,0);
				if(tipo == VARCHAR){
                    tipo = TIPOCHAR;
                }
                else if(tipo == VARINT){
                    tipo = TIPOINT;
                }
                else if(tipo == VARFLOAT){
                    tipo = TIPOFLOAT;
                }
                if(tipo2 == VARCHAR){
                    tipo2 = TIPOCHAR;
                }
                else if(tipo2 == VARINT){
                    tipo2 = TIPOINT;
                }
                else if(tipo2 == VARFLOAT){
                    tipo2 = TIPOFLOAT;
                }
				if(tipo != tipo2){ ACA VER QUE TENGO QUE HACER CAST JE
				    if(!( //Aceptacion de Coercion
				        ((tipo == TIPOFLOAT) &&
				            ((tipo2 == TIPOINT) || (tipo2 == TIPOCHAR)) //Float acepta casteo de Int y Char
                        ) ||
                        ((tipo == TIPOINT) && (tipo2 == TIPOCHAR) //Int acepta casteo de Char
                        )
                    )){
                        tipo = TIPOERROR;
                        error_handler(83); //Los tipos de ambos lados de la asignacion deben ser estructuralmente equivalentes
				    }
				}
				else{ //Ramificacion else obsoleta ya que se solicita que se accedan a los arreglos mediante un indice (Excepto en invocacion a funcion)
				    if((tipo == TIPOARREGLO) || (tipo == ARRINT) || (tipo == ARRFLOAT) || (tipo == ARRCHAR))
				    { //Por si devuelve tipo arreglo, en si deberia entrar por los 3 de abajo
				        tipo = TIPOERROR;
                        error_handler(81); //No se permite la asignacion de arreglos como un todo
				    }
				    else{ //Hecho por las dudas en caso que quiera asignar TIPOERROR o TIPOVOID x ej, de todas formas no deberia ser posible
				        if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
				            tipo = TIPOERROR;
				            error_handler(104); //Tipo de la asignacion no valido
				        }
				    }
				}
				if(GEN){
				    CODE[libreCODE++] = ALM;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.nivel;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.despl;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->cant_byte;
                    cargue_izquierdo = 1;
				}
				strcpy(lexema_aux_izq,"");
                guarde_variable = 0;
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
			    if(guarde_variable == 1){
			        if(GEN){
			            int tam_tipo;
			            if(tipo == TIPOCHAR){
			                tam_tipo = sizeof(char);
			            }
			            else if(tipo == TIPOINT){
			                tam_tipo = sizeof(int);
			            }
			            else if(tipo == TIPOFLOAT){
			                tam_tipo = sizeof(float);
			            }
                        CODE[libreCODE++] = CRVL;
                        CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.nivel;
                        CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.despl;
                        CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->cant_byte;
                        cargue_izquierdo = 1;
                    }
                    strcpy(lexema_aux_izq,"");
                    guarde_variable = 0;
                }
			    int operador = 0;
                switch(lookahead()){ //Guardo el operador para cargarlo luego
                    case CDISTINTO:
                        operador = 1;
                        break;
                    case CIGUAL:
                        operador = 2;
                        break;
                    case CMENOR:
                        operador = 3;
                        break;
                    case CMEIG:
                        operador = 4;
                        break;
                    case CMAYOR:
                        operador = 5;
                        break;
                    case CMAIG:
                        operador = 6;
                        break;
                }
				scanner();
				tipo2 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG, 1,0);
				if(tipo == VARCHAR){
                    tipo = TIPOCHAR;
                }
                else if(tipo == VARINT){
                    tipo = TIPOINT;
                }
                else if(tipo == VARFLOAT){
                    tipo = TIPOFLOAT;
                }
                if(tipo2 == VARCHAR){
                    tipo2 = TIPOCHAR;
                }
                else if(tipo2 == VARINT){
                    tipo2 = TIPOINT;
                }
                else if(tipo2 == VARFLOAT){
                    tipo2 = TIPOFLOAT;
                }

				if(tipo != tipo2){
				    // Posible implementacion de COERCION a futuro
                    tipo = TIPOERROR;
                    error_handler(96); //Los operandos de los operadores logicos o relacionales solo pueden ser de tipo char, int o float
                }
                else{
                    if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                        tipo = TIPOERROR;
                        error_handler(96); //Los operandos de los operadores logicos o relacionales solo pueden ser de tipo char, int o float
                    }
                }
                if(GEN){
                    int tam_tipo;
                    if(tipo2 == TIPOCHAR){
                        tam_tipo = sizeof(char);
                    }
                    else if(tipo2 == TIPOINT){
                        tam_tipo = sizeof(int);
                    }
                    else if(tipo2 == TIPOFLOAT){
                        tam_tipo = sizeof(float);
                    }
                     switch(operador){ //Cargo el operador guardado
                        case 1: //DISTINTO
                            CODE[libreCODE++] = CMNI;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                        case 2: //CIGUAL
                            CODE[libreCODE++] = CMIG;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                        case 3: //CMENOR
                            CODE[libreCODE++] = CMME;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                        case 4: //CMEIG
                            CODE[libreCODE++] = CMEI;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                        case 5: //CMAYOR
                            CODE[libreCODE++] = CMMA;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                        case 6: //CMAIG
                            CODE[libreCODE++] = CMAI;
                            CODE[libreCODE++] = tam_tipo;
                            break;
                    }
                }
                break;
		}
		if (cargue_izquierdo == 0){
		    if(GEN){
                CODE[libreCODE++] = CRVL;
                CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.nivel;
                CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->desc.despl;
                CODE[libreCODE++] = ts[en_tabla(lexema_aux_izq)].ets->cant_byte;
		    }
		    strcpy(lexema_aux_izq,"");
            guarde_variable = 0;
		}
	}
	return tipo;
}


int expresion_simple(set folset, int necesito_indice, int posible_asignacion)
{
    int tipo, tipo2;
	test(first(EXPRESION_SIMPLE),folset | COR,56);
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	tipo = termino(folset | CMAS | CMENOS | COR | first(TERMINO), necesito_indice, posible_asignacion);

	while(lookahead_in(CMAS | CMENOS | COR | first(TERMINO)))
	{
	    int operador = 0;
        switch(lookahead()){ //Guardo el operador para cargarlo luego
            case CMAS:
                operador = 1;
                break;
            case CMENOS:
                operador = 2;
                break;
            case COR:
                operador = 3;
                break;
        }
		match(CMAS | CMENOS | COR,65);
		tipo2 = termino(folset | CMAS | CMENOS | COR | first(TERMINO), 1, 0);
		if(tipo == VARCHAR){
            tipo = TIPOCHAR;
        }
        else if(tipo == VARINT){
            tipo = TIPOINT;
        }
        else if(tipo == VARFLOAT){
            tipo = TIPOFLOAT;
        }
        if(tipo2 == VARCHAR){
            tipo2 = TIPOCHAR;
        }
        else if(tipo2 == VARINT){
            tipo2 = TIPOINT;
        }
        else if(tipo2 == VARFLOAT){
            tipo2 = TIPOFLOAT;
        }

		if(tipo != tipo2){
            // Posible implementacion de COERCION a futuro
            tipo = TIPOERROR;
            error_handler(105); //Los tipos de ambos lados de los operadores logicos o aritmeticos deben ser estructuralmente equivalentes
        }
        else{
            if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                tipo = TIPOERROR;
                error_handler(106); //Los operandos de los operadores logicos o aritmeticos solo pueden ser de tipo char, int o float
            }
        }
        if(GEN){
            int tam_tipo;
            if(tipo2 == TIPOCHAR){
                tam_tipo = sizeof(char);
            }
            else if(tipo2 == TIPOINT){
                tam_tipo = sizeof(int);
            }
            else if(tipo2 == TIPOFLOAT){
                tam_tipo = sizeof(float);
            }
            switch(operador){ //Cargo el operador guardado
                case 1:
                    CODE[libreCODE++] = SUM;
                    CODE[libreCODE++] = tam_tipo;
                    break;
                case 2:
                    CODE[libreCODE++] = SUB;
                    CODE[libreCODE++] = tam_tipo;
                    break;
                case 3:
                    CODE[libreCODE++] = OR;
                    CODE[libreCODE++] = tam_tipo;
                    break;
            }
        }
	}
	return tipo;
}


int termino(set folset, int necesito_indice, int posible_asignacion)
{
    int tipo, tipo2;
	tipo = factor(folset | CMULT | CDIV | CAND | first(FACTOR), necesito_indice, posible_asignacion);

	while(lookahead_in(CMULT | CDIV | CAND | first(FACTOR)))
	{
	    int operador = 0;
	    switch(lookahead()){ //Guardo el operador para cargarlo luego
	        case CMULT:
                operador = 1;
                break;
            case CDIV:
                operador = 2;
                break;
            case CAND:
                operador = 3;
                break;
	    }
		match(CMULT | CDIV | CAND,65);
		tipo2 = factor(folset | CMULT | CDIV | CAND | first(FACTOR), 1, 0);
		if(tipo == VARCHAR){
            tipo = TIPOCHAR;
        }
        else if(tipo == VARINT){
            tipo = TIPOINT;
        }
        else if(tipo == VARFLOAT){
            tipo = TIPOFLOAT;
        }
        if(tipo2 == VARCHAR){
            tipo2 = TIPOCHAR;
        }
        else if(tipo2 == VARINT){
            tipo2 = TIPOINT;
        }
        else if(tipo2 == VARFLOAT){
            tipo2 = TIPOFLOAT;
        }
		if(tipo != tipo2){
            // Posible implementacion de COERCION a futuro
            tipo = TIPOERROR;
            error_handler(105); //Los tipos de ambos lados de los operadores logicos o aritmeticos deben ser estructuralmente equivalentes
        }
        else{
            if((tipo != TIPOINT) && (tipo != TIPOFLOAT) && (tipo != TIPOCHAR)){
                tipo = TIPOERROR;
                error_handler(106); //Los operandos de los operadores logicos o aritmeticos solo pueden ser de tipo char, int o float
            }
        }
        if(GEN){
            int tam_tipo;
            if(tipo2 == TIPOCHAR){
                tam_tipo = sizeof(char);
            }
            else if(tipo2 == TIPOINT){
                tam_tipo = sizeof(int);
            }
            else if(tipo2 == TIPOFLOAT){
                tam_tipo = sizeof(float);
            }
            switch(operador){ //Cargo el operador guardado
                case 1:
                    CODE[libreCODE++] = MUL;
                    CODE[libreCODE++] = tam_tipo;
                    break;
                case 2:
                    CODE[libreCODE++] = DIV;
                    CODE[libreCODE++] = tam_tipo;
                    break;
                case 3:
                    CODE[libreCODE++] = AND;
                    CODE[libreCODE++] = tam_tipo;
                    break;
            }
        }
	}
	return tipo;
}


int factor(set folset, int necesito_indice, int posible_asignacion)
{
	test(first(FACTOR), folset /*| CPAR_CIE*/,57); //Ver si saco CPAR_CIE por lote: if(k==) a;
	/** Lo tuve que sacar por lote propio: lote3ce.c en linea 21: f1(ab,); se colgaba la ejecucion **/
	int tipo;

	switch(lookahead())
	{
		case CIDENT:
		    if(en_tabla(sbol->lexema) == NIL){ //en_tabla devuelve NIL (-1) si no esta en tabla
		        error_handler(71); //Identificador no declarado
		        strcpy(inf_id->nbre,sbol->lexema);
		        inf_id->ptr_tipo = TIPOERROR;
		        tipo = TIPOERROR;
		        inf_id->clase = -1; //Le asigno una clase incorrecta
		        scanner(); //Consumo el identificador
		        insertarTS();
		    }
		    else if(Clase_Ident(sbol->lexema) == CLASFUNC){
		        tipo = llamada_funcion(folset);
		    }
		    else{
		        tipo = variable(folset, necesito_indice, posible_asignacion);
		    }
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			tipo = constante(folset);
			break;
		
		case CCONS_STR:
		    tipo = STRING; //Tipo auxiliar para comprobacion en proposicion_e_s en salida
		    CODE[libreCODE++] = CRCTS;
		    CODE[libreCODE++] = libreCAUX;
		    int i=0;
		    while(sbol->lexema[i] != '\0'){ //Copio el Lexema a la zona strings
		        if(libreCAUX == TAM_CTES){
		            error_handler(xxxx);//DAR CODIGO DE ERROR QUE ME PASE
		        }
		        CAUX[libreCAUX++] = sbol->lexema[i++];
		    }
		    if(libreCAUX == TAM_CTES){
                error_handler(xxxx);//DAR CODIGO DE ERROR QUE ME PASE
            }
            else{
                CAUX[libreCAUX++] = '\0';
            }

			scanner();
			break;
		
		case CPAR_ABR:
			match(CPAR_ABR,20);
			tipo = expresion(folset | CPAR_CIE,1,0); //BORRAR PUSE UN 0 PORQUE NO SE EVALUA POR ACA
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			tipo = expresion(folset,1,0); //BORRAR PUSE UN 0 PORQUE NO SE EVALUA POR ACA
			break;
			
		default:
		    tipo = TIPOERROR;
			error_handler(32);
	}
	test(folset,NADA,58);
	return tipo;
}


int variable(set folset, int necesito_indice, int posible_asignacion)
{
	test(first(VARIABLE),folset | CCOR_ABR,59);
	char lexema_aux[200];
	strcpy(lexema_aux,sbol->lexema);
	int tipo = Tipo_Ident(sbol->lexema), flag = 0;

	if(tipo == TIPOARREGLO){
	    flag = 1;
	    if(Clase_Ident(sbol->lexema) == CLASVAR){
	        tipo = ts[en_tabla(sbol->lexema)].ets->desc.part_var.arr.ptero_tipo_base;
	    }
	    else if (Clase_Ident(sbol->lexema) == CLASPAR){
	        tipo = ts[en_tabla(sbol->lexema)].ets->desc.part_var.param.ptero_tipo_base;
	    }
	    else{
	        tipo = TIPOERROR;
        }
	}
	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR))
	{
	    if(flag != 1){ //Veo si no tengo un arreglo
            tipo = TIPOERROR;
            error_handler(78); //La variable no es de tipo arreglo
        }
		match(CCOR_ABR,35);
		int tipoaux = expresion(folset | CCOR_CIE,1,0);
		if(tipoaux != TIPOINT){ //Control propio ajeno al sistema de tipos
		    tipo = TIPOERROR;
		    error_handler(103); //El indice de un arreglo debe ser una constante entera
		}
		if(tipo == TIPOCHAR){
            tipo = VARCHAR;
        }
        else if(tipo == TIPOINT){
            tipo = VARINT;
        }
        else if(tipo == TIPOFLOAT){
            tipo = VARFLOAT;
        }

		match(CCOR_CIE, 22);
	}
	else{ //Si no hay un CCOR_ABR
	    if(flag == 1){ //Veo si tengo un arreglo
	        if(necesito_indice == 1){ //Por si necesito un elemento para expresion
	            tipo = TIPOERROR;
                error_handler(79); //En una expresion los arreglos deben ser accedidos por sus elementos
	        }
	        else{ //Por si necesito devolver el tipo del arreglo junto con el tipo base
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
	    }
	    else{
	        if(posible_asignacion == 1){ //Puedo llegar a necesitar asignarle un valor a esta variable.
	            strcpy(lexema_aux_izq,lexema_aux);
	            guarde_variable = 1;
	        }
	        else{
	            if(GEN){
	                CODE[libreCODE++] = CRVL;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux)].ets->desc.nivel;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux)].ets->desc.despl;
                    CODE[libreCODE++] = ts[en_tabla(lexema_aux)].ets->cant_byte;
	            }
	        }
	        if(tipo == TIPOCHAR){
                tipo = VARCHAR;
            }
            else if(tipo == TIPOINT){
                tipo = VARINT;
            }
            else if(tipo == TIPOFLOAT){
                tipo = VARFLOAT;
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

	tipo_parametro_actual = expresion(folset | CCOMA | first(EXPRESION), 0,0); // No se evaluan llamados a funciones
	cantidad_parametros_actuales++;

	if(ptr_inf_res->tipo_pje == 'd'){
        if(tipo_parametro_actual == VARCHAR || tipo_parametro_actual == VARINT || tipo_parametro_actual == VARFLOAT){
            if(tipo_parametro_actual == VARCHAR){
                tipo_parametro_actual = TIPOCHAR;
            }
            else if(tipo_parametro_actual == VARINT){
                tipo_parametro_actual = TIPOINT;
            }
            else if(tipo_parametro_actual == VARFLOAT){
                tipo_parametro_actual = TIPOFLOAT;
            }
        }
        else{
            if((tipo_parametro_actual != ARRCHAR) && (tipo_parametro_actual != ARRINT) && (tipo_parametro_actual != ARRFLOAT)){
                error_handler(93); //Si el pasaje es por REFERENCIA, el parametro real debe ser una variable
            }
        }
    }
    else{
        if(tipo_parametro_actual == VARCHAR){
            tipo_parametro_actual = TIPOCHAR;
        }
        else if(tipo_parametro_actual == VARINT){
            tipo_parametro_actual = TIPOINT;
        }
        else if(tipo_parametro_actual == VARFLOAT){
            tipo_parametro_actual = TIPOFLOAT;
        }
    }

	if(tipo_parametro_actual == ARRINT){
	    tipo_parametro_actual = TIPOARREGLO;
	    tipo_parametro_actual_base = TIPOINT;
	}
	else if(tipo_parametro_actual == ARRFLOAT){
        tipo_parametro_actual = TIPOARREGLO;
        tipo_parametro_actual_base = TIPOFLOAT;
    }
    else if(tipo_parametro_actual == ARRCHAR){
        tipo_parametro_actual = TIPOARREGLO;
        tipo_parametro_actual_base = TIPOCHAR;
    }
    else{
        tipo_parametro_actual_base = -1;
    }

	if((tipo_parametro_formal != tipo_parametro_actual) || (tipo_parametro_formal_base != tipo_parametro_actual_base)){
	    flag = 1;
	}

	if((tipo_parametro_formal == TIPOARREGLO) && (tipo_parametro_actual != TIPOARREGLO)){
	    error_handler(98); //Si el parametro formal es un arreglo, en el parametro real solo debe haber un identificador
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

		tipo_parametro_actual = expresion(folset | CCOMA | first(EXPRESION), 0,0); // No se evaluan llamados a funciones
		cantidad_parametros_actuales++;

		if(ptr_inf_res->tipo_pje == 'd'){
            if(tipo_parametro_actual == VARCHAR || tipo_parametro_actual == VARINT || tipo_parametro_actual == VARFLOAT){
                if(tipo_parametro_actual == VARCHAR){
                    tipo_parametro_actual = TIPOCHAR;
                }
                else if(tipo_parametro_actual == VARINT){
                    tipo_parametro_actual = TIPOINT;
                }
                else if(tipo_parametro_actual == VARFLOAT){
                    tipo_parametro_actual = TIPOFLOAT;
                }
            }
            else{
                if((tipo_parametro_actual != ARRCHAR) && (tipo_parametro_actual != ARRINT) && (tipo_parametro_actual != ARRFLOAT)){
                    error_handler(93); //Si el pasaje es por REFERENCIA, el parametro real debe ser una variable
                }
            }
        }
        else{
            if(tipo_parametro_actual == VARCHAR){
                tipo_parametro_actual = TIPOCHAR;
            }
            else if(tipo_parametro_actual == VARINT){
                tipo_parametro_actual = TIPOINT;
            }
            else if(tipo_parametro_actual == VARFLOAT){
                tipo_parametro_actual = TIPOFLOAT;
            }
        }

        if(tipo_parametro_actual == ARRINT){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOINT;
        }
        else if(tipo_parametro_actual == ARRFLOAT){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOFLOAT;
        }
        else if(tipo_parametro_actual == ARRCHAR){
            tipo_parametro_actual = TIPOARREGLO;
            tipo_parametro_actual_base = TIPOCHAR;
        }
        else{
            tipo_parametro_actual_base = -1;
        }

        if((tipo_parametro_formal != tipo_parametro_actual) || (tipo_parametro_formal_base != tipo_parametro_actual_base)){
            flag = 1;
        }

        if((tipo_parametro_formal == TIPOARREGLO) && (tipo_parametro_actual != TIPOARREGLO)){
            error_handler(98); //Si el parametro formal es un arreglo, en el parametro real solo debe haber un identificador
        }
	}

	if(flag != 0){ //DUDA: Aqui deberia hacer algo mas que marcar el error?
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
		    if(GEN){
                CODE[libreCODE++] = CRCT;
                CODE[libreCODE++] = sizeof(float);
                CODE[libreCODE++] = atoi(sbol->lexema);
            }
			scanner();
			break;
		
		case CCONS_FLO:
		    tipo = TIPOFLOAT;
		    if(GEN){
                CODE[libreCODE++] = CRCT;
                CODE[libreCODE++] = sizeof(float);
                CODE[libreCODE++] = atof(sbol->lexema);
            }
			scanner();
			break;
		
		case CCONS_CAR:
		    tipo = TIPOCHAR;
		    if(GEN){
		        CODE[libreCODE++] = CRCT;
		        CODE[libreCODE++] = sizeof(char);
		        CODE[libreCODE++] = sbol->lexema[0]; //BORRAR VER SI ES DE 0 o 1, PROBAR
		    }
			scanner();
			break;
		
		default:
		    tipo = TIPOERROR;
			error_handler(33);
	}
	test(folset,NADA,63);
	return tipo;
}
