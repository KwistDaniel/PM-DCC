#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigos.h"
#include "util.h"
#include "error.h"
#include "ts.h"

int last_call=0;

/*********** prototipos *************/

void unidad_traduccion(set);
void declaraciones(set);
int especificador_tipo(set);
void especificador_declaracion(set, int);
void definicion_funcion(set, int);
void declaracion_variable(set, int);
int lista_declaraciones_param(set, int);
void declaracion_parametro(set, int, tipo_inf_res*);
void declarador_init(set, int);
void lista_declaraciones_init(set, int);
int lista_inicializadores(set, int);
void lista_proposiciones(set);
void lista_declaraciones(set);
void declaracion(set);
void proposicion(set);
void proposicion_expresion(set);
void proposicion_compuesta(set, int);
void proposicion_seleccion(set);
void proposicion_iteracion(set);
void proposicion_e_s(set);
void proposicion_retorno(set);
int expresion(set, int, int);
int expresion_simple(set, int, int);
int termino(set, int, int);
int factor(set, int, int);
int variable(set, int, int);
int llamada_funcion(set);
int lista_expresiones(set, int);
int constante(set);
