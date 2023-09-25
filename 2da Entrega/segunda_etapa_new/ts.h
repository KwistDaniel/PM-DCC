/*
======================================================================
	DISEÑO Y CONSTRUCCIÓN DE COMPILADORES
	PROGRAMAS FUENTES ENTREGADOS POR LA CÁTEDRA
        HEADER PARA EL ADMINISTRADOR DE TABLA DE SÍMBOLOS
======================================================================
*/

/*
=====================================================================
	 Definiciones de constantes usadas por la tabla de símbolos
=====================================================================
*/

# define TAM_ID     15
# define TAM_TS    200
# define TAM_HASH  101
# define TAM_BLOQ   30
# define BASE_TS    -1
# define BASE_TB    -1
# define NIL        -1         // indica que no apunto a ningún tipo


// ========== Definición de las clases de identificadores ==========

			// CLASES CORRESPONDIENTES A:

#define CLASTYPE   1    // identificador de tipo
#define CLASVAR    2    // identificador de variable
#define CLASFUNC   3    // identificador de función
#define CLASPAR    4    // identificador de parámetro formal

/*

=====================================================================
 Definiciones de tipos del administrador de tabla de símbolos
=====================================================================
*/

/*   LAS SIGUIENTES ESTRUCTURAS FORMAN PARTE DEL DESCRIPTOR
     DE UNA ENTRADA EN LA TABLA DE SÍMBOLOS         */

// ------------------------- TIPO_INF_RES --------------------------
// Estructura que forma la lista con información resumida de parámetros

 typedef struct inf_res {
	int ptero_tipo;					// tipo del parámetro
	char tipo_pje;						// tipo de pasaje: valor o referencia
	int ptero_tipo_base;				// descriptor para cuando el parámetro es de tipo arreglo
	struct inf_res  *ptr_sig;		// siguiente parámetro
} tipo_inf_res;

// ---------------------------- STC_TS -----------------------------
// Estructura propia de: variables (anónimas), subrutinas y parámetros

typedef struct {
	int nivel;  						// nivel lexicográfico del objeto computacional
	int despl; 							// desplazamiento dentro del registro de activación usado solamente
											// para identificadores de clase "variable" y  "parámetro"
	union {
		struct {							// si es un identificador de parámetro
		char tipo_pje;					// tipo de pasaje de este parámetro
		int ptero_tipo_base;			// descriptor para cuando el parámetro es de tipo arreglo
		} param;
		
		struct { 								// si es un identificador de función
			int dir_cod;  						// dirección del código
			int cant_par;  					// cantidad de parámetros
			tipo_inf_res *ptr_inf_res;		// puntero a la lista que mantiene información de los parámetros
		} sub;
	
		struct {									// si es una variable de tipo arreglo
			int ptero_tipo_base;				// tipo de los elementos
			int cant_elem;						// cantidad de elementos
		} arr;					

	} part_var;  						// fin de la unión de la parte variante

} stc_TS;


/* LA SIGUIENTE ESTRUCTURA FORMA UNA ENTRADA EN LA TABLA DE SÍMBOLOS*/

// -------------------------- ENTRADA_TS ---------------------------
//          Estructura de una entrada en la tabla de símbolos

typedef struct {

	char nbre[TAM_ID];	// nombre del identificador

	int clase;				// clase de objeto computacional: vble., función, parám., tipo.

	int ptr_tipo;      	// puntero al tipo del identificador, apunta a la tabla de símbolos
								// también sirve para indicar el tipo del valor retornado por una función

	int cant_byte;     	// cantidad de bytes del objeto computacional

	stc_TS desc;      	// descriptor de variable, función o parámetro

} entrada_TS;


// ---------------------------- TIPO_TS ----------------------------
//          Estructura que define el tipo de la tabla de símbolos

typedef struct {

	int ptr_sinon;			// puntero que encadena los sinónimos
	entrada_TS  *ets;		// entrada_TS particular

} tipo_TS;


/*
=====================================================================
						Definición de variables globales
=====================================================================
*/

//extern entrada_TS  *inf_id;
//extern tipo_TS  ts[TAM_TS];		// Tabla de Símbolos


entrada_TS  *inf_id;		// Puntero a la estructura que contiene la
								// información de un identificador, la cual es
								// completada previamente a una inserción en TS

int       th[TAM_HASH];   // Tabla de Hash
tipo_TS   ts[TAM_TS];     // Tabla de Símbolos
int       tb[TAM_BLOQ];   // Tabla de Bloques
int  topeTS;
int  topeTB;

/* 
=====================================================================
		Funciones para la administración de la Tabla de Símbolos 
=====================================================================
*/

/* PROTOTIPOS */

void inic_tablas();

int insertarTS();
int en_tabla(char *);
int en_nivel_actual(char *);
int Tipo_Ident(char *);
int Clase_Ident(char *);
 
void popTS();
int pushTS(int, entrada_TS *);

void popTB();
void pushTB();
void pop_nivel();
int get_nivel();
