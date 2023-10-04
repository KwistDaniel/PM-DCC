#include <string.h>
#include "parser.h"


int TIPOVOID, TIPOCHAR, TIPOINT, TIPOFLOAT, TIPOARREGLO, TIPOERROR;

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
			declaracion_variable(folset,tipo);
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset,int tipo)
{
    inf_id->ptr_tipo = tipo;
    inf_id->clase = CLASFUNC;
    int posicionTSF = insertarTS();
    int cantidad_parametros = 0;

    //RECORDAR PARAMETROS (insertar ts devuelve pos donde inserta, guardar eso en una variable para los parametros)
	match(CPAR_ABR, 20);
	pushTB();

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		cantidad_parametros = lista_declaraciones_param(folset | CPAR_CIE | first(PROPOSICION_COMPUESTA),posicionTSF);

	match(CPAR_CIE, 21);

	//Ir a la pos en la TS y darle el num de params que conte
	ts[posicionTSF].ets->desc.part_var.sub.cant_par = cantidad_parametros;

	proposicion_compuesta(folset,0); //paso 0 para indicar que no hay que hacer un pushTB();
}


int lista_declaraciones_param(set folset, int posicionTSF) //Agrego la posicion para completar param (***-)
{
    int cantidad_parametros = 0;

	declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF);

    cantidad_parametros++;

	while(lookahead_in(CCOMA | first(DECLARACION_PARAMETRO)))
	{
		match(CCOMA,64);
		declaracion_parametro(folset | CCOMA | first(DECLARACION_PARAMETRO), posicionTSF);
		//Falta hacer algo mas? por donde estan los parametros
		cantidad_parametros++;
	}

	return cantidad_parametros;
}


void declaracion_parametro(set folset, int posicionTSF)
{
	int tipo = especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);

    int control28 = 0;

	if(lookahead_in(CAMPER)){
	    scanner();
	    //inf_id->desc.param.tipo_pje = 'd';
	    inf_id->desc.part_var.param.tipo_pje = 'd';
	    control28 = 1; //Se uso el &, controlar que no venga un arreglo (28)
	}
	else{
	    //inf_id->desc.param.tipo_pje = 'v';
	    inf_id->desc.part_var.param.tipo_pje = 'v';
	}

	strcpy(inf_id->nbre,sbol->lexema);
	inf_id->clase = CLASPAR;

	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
	    if(control28){
	        error_handler(92); //<tipo> & <nombre arreglo> []
	    }
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
		//inf_id->desc.param.ptero_tipo_base = tipo;
		inf_id->desc.part_var.param.ptero_tipo_base = tipo;
		inf_id->ptr_tipo = TIPOARREGLO;
	}
	else{
	    inf_id->ptr_tipo = tipo;
	}


    //Ver con el profe como hago para poner los datos que faltan al parametro
    //Osea tengo que usar la info que tengo de posicionTSF para llenar la info de la func y eso

	insertarTS();//Insert de tipo parametro
	//Actualizar la ts
	ts[posicionTSF]-> o . ,....
	//Ya estoy sumando la cantidad de parametros mas arriba
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


void declaracion_variable(set folset, int tipo)
{
	declarador_init(folset | CCOMA | first(LISTA_DECLARACIONES_INIT) | CPYCOMA, tipo);

	if(lookahead_in(CCOMA | first(LISTA_DECLARACIONES_INIT)))
	{
		match(CCOMA,64);
		lista_declaraciones_init(folset | CPYCOMA);
	}

	match(CPYCOMA, 23);
	test(folset,NADA,51);
}


void declarador_init(set folset, int tipo)
{
	test(first(DECLARADOR_INIT) | folset, CASIGNAC | CCONS_FLO | CCONS_CAR | CCOR_ABR | CCOR_CIE | CLLA_ABR | CLLA_CIE,47); //Se quito el CCON_ENT y donde aparece (first const y first lis in) (!)

	int dimension = -1, sin_dimension = -1;

	switch(lookahead())
	{
	    case CCONS_FLO:
	    case CCONS_CAR:
		case CASIGNAC:
		    inf_id->ptr_tipo = tipo;
			match(CASIGNAC,66);
			//falta algo? X la asignacion digo
			constante(folset);
			break;

        case CLLA_ABR:
        case CLLA_CIE:
        case CCOR_CIE:
		case CCOR_ABR:
		    inf_id->ptr_tipo = TIPOARREGLO;
		    //Controlar que tipo sea char int o float asi cumplo regla 7 aca.
		    //VER DONDE REVISO REGLA 5
		    inf_id->desc.part_var.arr.ptero_tipo_base = tipo;
		    // (***-) Aca puedo controlar que sea float,int o char para regla 7???
			match(CCOR_ABR, 35);
			
			if(lookahead_in(CCONS_ENT)){
			    dimension = atoi(sbol->lexema);
			    if(dimension <= 0){
			        error_handler(75); //La cantidad de elementos de un arreglo puede estar dada por un número natural (es decir, mayor a 0) y/o a través de la inicialización del mismo.

			    }
			    ////*****//// (***-) Cual de las 2 uso? el else o le asigno nomas??
			    //inf_id->desc.arr.cant_elem = atoi(sbol->lexema);
			    else{inf_id->desc.arr.cant_elem = atoi(sbol->lexema);}
			    constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES));
			}
			else if(lookahead_in(CCOR_CIE)){
			    sin_dimension = 1;
			}


			match(CCOR_CIE, 22);


            int cantidad_elementos = 0;
			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | first(LISTA_INICIALIZADORES)))
			{
				match(CASIGNAC,66);
				match(CLLA_ABR, 24);
				cantidad_elementos = lista_inicializadores(folset | CLLA_CIE, tipo);

				if(sin_dimension){
				    inf_id->desc.part_var.arr.cant_elem = cantidad_elementos;
				}
				else if((dimension > 0) && cantidad_elementos > dimension){
				    error_handler(76); // La cantidad de valores inicializadores debe ser igual o menor al tamaño del arreglo declarado
				}
				else{
				    inf_id->desc.part_var-arr-cant_elem = dimension;
				}
				//// !!!! //Else error 74???? (***-) hace falta aca? va en otro lado? SI NO VINO CONSTANTE ENTERA NI LISTA DE INICIALIZADORES VA ERROR 74
				//VER QUE VAYA DONDE VA

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
	cantidad_elementos++;

	if(tipo_constante != tipo_base){
	    error_handler(77); //El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion
	}

	while(lookahead_in(CCOMA | first(CONSTANTE)))
	{
		match(CCOMA,64);
		tipo_constante = constante(folset | CCOMA | first(CONSTANTE));

	    if(tipo_constante != tipo_base){
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
	test(folset | CSHL | CSHR,NADA,53); //agregados el >> y << porque se fuerza entrada en case de arriba
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
	test(first(FACTOR), folset /*| CPAR_CIE*/,57); //Ver si saco CPAR_CIE por lote: if(k==) a;
	/** Lo tuve que sacar por lote propio: lote3ce.c en linea 21: f1(ab,); se colgaba la ejecucion **/
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
		        llamada_funcion(folset);
		    }
		    else(Clase_Ident(sbol->lexema) == CLASVAR){
		        variable(folset);
		    }

		    //Tomar nota de aca, si no es funcion, lo mando a variable, si queria usar una funcion, me va a tirar error por variable en vez de funcion
		    //Informar en el informe que se hace esto
		    //Ademas, sino, podria traer el match antes del else if y de ahi ver si tiene un ( el lookahead o no y de ahi tratarlo.
		    //Aca no van a esperar un identificador ni llamada a funcion ni variable

		    /*else{
		        error_handler(10);
		        printf("\n\nError en factor -> switch -> case CIDENT -> else \n\n");
		        //Tendre un caso que entre aca? Revisar
		    }*/ //Despreciado ya que se mando el error a variable
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
		//case CPAR_CIE: //SACAR PAR CIE SI SACO ARRIBA
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
