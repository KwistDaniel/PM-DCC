#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigos.h"
#include "util.h"
#include "error.h"

int last_call=0;

/*********** prototipos *************/

void unidad_traduccion();
void declaraciones();
void especificador_tipo();
void especificador_declaracion();
void definicion_funcion();
void declaracion_variable();
void lista_declaraciones_param();
void declaracion_parametro();
void declarador_init();
void lista_declaraciones_init();
void lista_inicializadores();
void lista_proposiciones();
void lista_declaraciones();
void declaracion();
void proposicion();
void proposicion_expresion();
void proposicion_compuesta();
void proposicion_seleccion();
void proposicion_iteracion();
void proposicion_e_s();
void proposicion_retorno();
void expresion();
void expresion_simple();
void termino();
void factor();
void variable();
void llamada_funcion();
void lista_expresiones();
void constante();
