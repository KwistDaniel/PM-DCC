#include <stdio.h>
#include <string.h>
#include "error.h"
#include "var_globales.h"

extern int last_call;
int hubo_error=0;


void error_print(int ne)
{
	switch(ne)
	{
		/* errores de scanner */
		case 1:  printf("\t Error %d: Fin de comentario no encontrado\n", ne); break;
		case 2:  printf("\t Error %d: El string es demasiado largo\n", ne); break;
		case 3:  printf("\t Warning %d: Falta comilla de cierre. La linea sera consumida\n", ne); break;
		case 4:  printf("\t Warning %d: Identificador de mas de 8 caracteres, sera truncado\n", ne); break;
		case 5:  printf("\t Error %d: Caracter invalido\n", ne); break;
		/* errores generales */
		case 6:  printf("\t Error %d: El numero de argumentos no coincide. ucc -c programa.c \n", ne); break;
		case 7:  printf("\t Error %d: El segundo argumento debe ser '-c'. ucc -c programa.c\n", ne); break;
		case 8:  printf("\t Error %d: El archivo no existe\n ", ne); break;
		case 9:  printf("\t Error %d: No se encontro el fin de archivo\n",ne); break;
		case 10: printf("\t Error %d: Ocurrio un error \n", ne); break;
		/* errores de la tabla de simbolos */
		case 12: printf("\t Error %d: Falta Memoria\n", ne); break;
		case 13: printf("\t Error %d: Tabla de Simbolos vacia\n", ne); break;
		case 14: printf("\t Error %d: Tabla de Simbolos llena\n", ne); break;
		case 15: printf("\t Error %d: Tabla de Bloques llena\n", ne); break;
		case 16: printf("\t Error %d: Tabla de Bloques vacia\n", ne); break;
		/* errores de parser*/
		case 17: printf("\t Error %d: Falta identificador\n", ne); break;
		case 18: printf("\t Error %d: Tipo no definido\n", ne); break;
		case 19: printf("\t Error %d: En Especificador de Declaracion\n", ne); break;
		case 20: printf("\t Error %d: Falta (\n", ne); break;
		case 21: printf("\t Error %d: Falta )\n", ne); break;
		case 22: printf("\t Error %d: Falta ]\n", ne); break;
		case 23: printf("\t Error %d: Falta ;\n", ne); break;
		case 24: printf("\t Error %d: Falta {\n", ne); break;
		case 25: printf("\t Error %d: Falta }\n", ne); break;
		case 26: printf("\t Error %d: En Proposicion\n", ne); break;
		case 27: printf("\t Error %d: Falta while\n", ne); break;
		case 28: printf("\t Error %d: Falta if\n", ne); break;
		case 29: printf("\t Error %d: Falta cin o cout\n", ne); break;
		case 30: printf("\t Error %d: Falta >>\n", ne); break;
		case 31: printf("\t Error %d: Falta <<\n", ne); break;
		case 32: printf("\t Error %d: En Factor\n", ne); break;
		case 33: printf("\t Error %d: Falta constante\n", ne); break;
		case 34: printf("\t Error %d: Falta identificador de funcion\n", ne); break;
		case 35: printf("\t Error %d: Falta [\n", ne); break;
		/* posibles errores personalizados (que no sean parafrasis de errores ya existentes) 36-39*/
		/* errores de recuperacion de errores */
		case 40: printf("\t Error %d: Al inicio del programa\n", ne); break;
		case 41: printf("\t Error %d: Simbolo inesperado o falta especificador de tipo\n", ne); break;
		case 42: printf("\t Error %d: Simbolo inesperado despues de especificador de tipo\n", ne); break;
		case 43: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de especif. de declaracion\n", ne); break;
		case 44: printf("\t Error %d: Simbolo inesperado despues de especificador de declaracion\n", ne); break;
		case 45: printf("\t Error %d: Simbolo inesperado despues de declarar un parametro\n", ne); break;
		case 46: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de lista decl. init\n", ne); break;
		case 47: printf("\t Error %d: Simbolo inesperado al comienzo de declarador init\n", ne); break;
		case 48: printf("\t Error %d: Simbolo inesperado despues de declarador init\n", ne); break;
		case 49: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de proposicion compuesta\n", ne); break;
		case 50: printf("\t Error %d: Simbolo inesperado despues de proposicion compuesta\n", ne); break;
		case 51: printf("\t Error %d: Simbolo inesperado despues de declaracion\n", ne); break;
		case 52: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de proposicion\n", ne); break;
		case 53: printf("\t Error %d: Simbolo inesperado despues de proposicion entrada - salida\n", ne); break;
		case 54: printf("\t Error %d: Simbolo inesperado despues de proposicion retorno\n", ne); break;
		case 55: printf("\t Error %d: Simbolo inesperado despues de proposicion expresion\n", ne); break;
		case 56: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de expresion simple\n", ne); break;
		case 57: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de factor\n", ne); break;
		case 58: printf("\t Error %d: Simbolo inesperado despues de factor\n", ne); break;
		case 59: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de variable\n", ne); break;
		case 60: printf("\t Error %d: Simbolo inesperado despues de variable\n", ne); break;
		case 61: printf("\t Error %d: Simbolo inesperado despues de llamada a funcion\n", ne); break;
		case 62: printf("\t Error %d: Simbolo inesperado o falta simb. al comienzo de constante\n", ne); break;
		case 63: printf("\t Error %d: Simbolo inesperado despues de constante\n", ne); break;
		case 64: printf("\t Error %d: Falta , \n", ne); break;
		case 65: printf("\t Error %d: Falta *, /, &&, +, -, ||, = u operador de relacion \n", ne); break;
		case 66: printf("\t Error %d: Falta = \n", ne); break;
		case 67: printf("\t Error %d: Simbolo inesperado o falta simb. en lista de inicializadores\n", ne); break;
		case 68: printf("\t Error %d: Simbolo inesperado despues de proposicion\n", ne); break;
		case 69: printf("\t Error %d: Simbolo inesperado o falta simb. despues de expresion simple\n", ne); break;
		/* errores de semantica */
		case 71: printf("\t Error %d: Identificador no declarado\n", ne); break;																								
		case 72: printf("\t Error %d: Identificador ya declarado\n", ne); break;																								
		case 73: printf("\t Error %d: Una variable, un parametro o un arreglo no pueden ser de tipo void\n", ne); break;											
		case 74: printf("\t Error %d: La cdad. de elementos de un arreglo debe ser una cte. entera en la declaracion\n", ne); break;							
		case 75: printf("\t Error %d: La cantidad de elementos de un arreglo debe ser mayor a 0\n", ne); break;														
		case 76: printf("\t Error %d: La cantidad de valores inicializadores debe ser igual o menor al tamaño del arreglo declarado\n", ne); break;		
		case 77: printf("\t Error %d: El tipo de los valores inicializadores del arreglo debe coincidir con su declaracion\n", ne); break;					
		case 78: printf("\t Error %d: La variable no es de tipo arreglo\n", ne); break;																						
		case 79: printf("\t Error %d: En una expresion los arreglos deben ser accedidos por sus elementos\n", ne); break;											
		case 80: printf("\t Error %d: Los indices validos del arreglo son entre 0 y tamaño-1\n", ne); break;															
		case 81: printf("\t Error %d: No se permite la asignacion de arreglos como un todo\n", ne); break;																
		case 82: printf("\t Error %d: En el lado izquierdo de una asignacion debe haber una variable\n", ne); break;												
		case 83: printf("\t Error %d: Los tipos de ambos lados de la asignacion deben ser estructuralmente equivalentes\n", ne); break;
		case 84: printf("\t Error %d: Falta declarar la funcion main()\n", ne); break;																						
		case 85: printf("\t Error %d: El tipo de la funcion main() debe ser void\n", ne); break;																			
		case 86: printf("\t Error %d: La funcion main() no lleva argumento\n", ne); break;																					
		case 87: printf("\t Error %d: El tipo del valor de retorno no coincide con el tipo de la funcion\n", ne); break;											
		case 88: printf("\t Error %d: Falta return\n", ne); break;																													
		case 89: printf("\t Error %d: Un procedimiento no lleva una sentencia de retorno\n", ne); break;																
		case 90: printf("\t Error %d: La CANTIDAD de parametros actuales no coincide con la cantidad de parametros formales\n", ne); break;					
		case 91: printf("\t Error %d: El TIPO de los parametros actuales no coincide con el de los parametros formales\n", ne); break;						
		case 92: printf("\t Error %d: No se permite <tipo> & <id_arreglo> [] en la definicion de un parametro\n", ne); break;									
		case 93: printf("\t Error %d: Si el pasaje es por REFERENCIA, el parametro real debe ser una variable\n", ne); break;									
		case 94: printf("\t Error %d: La constante string solo puede aparecer en las proposiciones de E/S\n", ne); break;											
		case 95: printf("\t Error %d: Las proposiciones de E/S solo aceptan variables y/o expresiones de tipo char, int y float\n", ne); break;			
		case 96: printf("\t Error %d: Los operandos de los operadores logicos o relacionales solo pueden ser de tipo char, int o float\n", ne); break;	
		case 97: printf("\t Error %d: Las condiciones de las prop. de seleccion e iteracion solo pueden ser de tipo char, int y float\n", ne); break;	
		case 98: printf("\t Error %d: Si el parametro formal es un arreglo, en el parametro real solo debe haber un identificador\n", ne); break;		
		case 99: printf("\t Error %d: El identificador debe ser una funcion o un procedimiento\n", ne); break;														
		case 100:printf("\t Error %d: El identificador debe ser una variable o parametro\n", ne); break;
	}
}


void error_handler(int ne)
{	
int i;
hubo_error++;
if(last_call){
	if(hubo_error==1) printf("Linea %d - %s\n", nro_linea, linea);
	else {
	if(strlen(linea)==2) error_print(errores_x_linea[i]);
	for(i = 0; i < cant_errores_x_linea; i++)
		error_print(errores_x_linea[i]); 
	cant_errores_x_linea = 0;}
}
else{
	if(ne == COD_IMP_ERRORES)
	{
		printf("Linea %d - %s\n", nro_linea, linea);
		for(i = 0; i < cant_errores_x_linea; i++)
			error_print(errores_x_linea[i]);
		cant_errores_x_linea = 0;
	}
	else
	{
		errores_x_linea [cant_errores_x_linea] = ne;
		cant_errores_x_linea++;
	}
    }
}
