/* 
======================================================================
						DISEÑO Y CONSTRUCCIÓN DE COMPILADORES

					PROGRAMAS FUENTES ENTREGADOS POR LA CÁTEDRA

						ADMINISTRADOR DE TABLA DE SÍMBOLOS
======================================================================
*/

/*
=====================================================================
						#include para usar en el programa
=====================================================================
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "ts.h"

// ============ FUNCIÓN DE INICIALIZACIÓN DE TABLAS =================

void inic_tablas()
{
	int i;
	topeTS = BASE_TS;
	topeTB = BASE_TB;

	//  TODOS LOS TOPES ESTAN INICIALIZADOS EN LA DECLARACION

	for(i=0; i<TAM_TS; i++) {		// inicializo Tabla de Símbolos
		ts[i].ptr_sinon = NIL;
		ts[i].ets = NULL;
	}

	for(i=0; i<TAM_HASH; i++)		// inicializo Tabla de Hash en nil
		th[i] = NIL;

	for(i=0; i<TAM_BLOQ; i++)		// inicializo Tabla de Bloques en nil
		tb[i] = NIL;

	// RESERVA MEMORIA para una entrada de la TS

	inf_id = (entrada_TS *) calloc(1, sizeof(entrada_TS));

	// inicializo la entrada para el tipo base VOID
	strcpy(inf_id->nbre, "void");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	inf_id->cant_byte = 0; 
	insertarTS();

	// inicializo la entrada para el tipo base CHAR
	strcpy(inf_id->nbre, "char");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	inf_id->cant_byte = 1; 
	insertarTS();

	// inicializo la entrada para el tipo base INT
	strcpy(inf_id->nbre, "int");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	inf_id->cant_byte = sizeof(int); 
	insertarTS();

	// inicializo la entrada para el tipo base FLOAT
	strcpy(inf_id->nbre, "float");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	inf_id->cant_byte = sizeof(float);
	insertarTS();

	// inicializo la entrada para el tipo estructurado ARREGLO
	strcpy(inf_id->nbre, "TIPOARREGLO");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	insertarTS();

	// inicializo la entrada para el tipo base erroneo TIPOERROR
	strcpy(inf_id->nbre, "TIPOERROR");
	inf_id->clase = CLASTYPE;
	inf_id->ptr_tipo = NIL;
	insertarTS();
 
}


// ================ FUNCIONES DE LA TABLA DE HASH ==================

int hash(char id[]) {
	int i;
	long l = 0;

	for(i=0; id[i]; i++)
		l = 3 * l + id[i];
	
	l = l % TAM_HASH;
	return l;
}


// =============== FUNCIONES DE LA TABLA DE BLOQUES ================

void popTB() {
	if(topeTB == BASE_TB) {
		error_handler(16);
		exit(1);
	}
	else {
		tb[topeTB] = NIL;
		topeTB --;
	}
}


void pushTB() {						// asumo que apunto al 1er ident del nuevo bloque
	topeTB ++;
	if(topeTB == TAM_BLOQ) {
		error_handler(15);
		exit(1);
	}
	else
		tb[topeTB] = topeTS + 1;
}


void pop_nivel() {					// El bloque a eliminar está al tope de TS y TB
	int h;
	while(topeTS >= tb[topeTB]) {
		h = hash(ts[topeTS].ets->nbre);
		th[h] = ts[topeTS].ptr_sinon;  	// modifico la TH según los sinónimos
		popTS();        						// elimino un identificador del bloque que abandono
	}
	popTB();      								// elimino el bloque que abandono
}


int get_nivel() {
	return topeTB;
}


// ================ FUNCIONES DE LA TABLA DE SÍMBOLOS ===============

int insertarTS() {					// la inf. del identif. está en inf_id que es global
	int i, h;
	h = hash(inf_id->nbre);
	if(th[h] != NIL)
		if(en_nivel_actual(inf_id->nbre) >= 0) {
			error_handler(72);
			memset((void*)inf_id, 0, sizeof(entrada_TS));
			return 0;      			//al retornar 0 indico que NO lo pude insertar
		}

	// inserto un nuevo identificador
	th[h] = pushTS(th[h], inf_id);

	// pido más memoria para el nuevo identificador
	inf_id = NULL;
	inf_id = (entrada_TS *) calloc(1, sizeof(entrada_TS));
	if(inf_id == NULL) {
		error_handler(12);
		exit(1);
	}
	return th[h];  					// retorno el lugar en TS donde se inserto
}


int en_tabla(char *st) {			//busca un identificador en tabla de símbolos,
	int h;								//retorna su posición o NIL (si no lo encuentra)
	h = th[hash(st)];
	while(h != NIL) {
		if(strcmp(ts[h].ets->nbre, st) == 0)
			return h;
		h = ts[h].ptr_sinon;
	}
	return NIL;
}


int Tipo_Ident(char *st) {			//busca un identificador en tabla de símbolos,
	int h;                     	//retorna su tipo o NIL (si no lo encuentra)
	h = th[hash(st)];
	while(h != NIL) {
		if(strcmp(ts[h].ets->nbre,st) == 0)
			return ts[h].ets->ptr_tipo;
		h = ts[h].ptr_sinon;
	}
	return NIL; 
}


int Clase_Ident(char *st) {		//busca un identificador en tabla de símbolos,
	int h;								//retorna su clase o NIL (si no lo encuentra)
	h = th[hash(st)];
	while(h != NIL) {
		if(strcmp(ts[h].ets->nbre, st) == 0)
			return ts[h].ets->clase;
		h = ts[h].ptr_sinon;
	}
	return NIL;
}


int en_nivel_actual(char *id) {	//busca un identificador en el bloque actual
	int h;                  		//retorna su posición o NIL (si no lo encuentra)
	h = th[hash(id)];
	while(h >= tb[topeTB]) {		// busco el identificador dentro del bloque
		if(!strcmp(ts[h].ets->nbre, id))
			return h;     				// lo encontro, devuelvo la posición h
		h = ts[h].ptr_sinon;
	}
	return NIL;          			// NO lo encontro ==> ident no declarado
}


void popTS() {
	if(topeTS == BASE_TS) {
		error_handler(13);
		exit(1);
	}
	else {
		ts[topeTS].ptr_sinon = NIL;
		free((void *)ts[topeTS].ets);
		ts[topeTS].ets = NULL;
		topeTS --;
	}
}


int pushTS(int s, entrada_TS *ptr) {
	topeTS ++;
	if(topeTS == TAM_TS) {
		error_handler(14);
		exit(1);        
	}
	else {
		ts[topeTS].ptr_sinon = s;
		ts[topeTS].ets = ptr;
	}
	return topeTS;   					// retorno la posición donde insertó
}
