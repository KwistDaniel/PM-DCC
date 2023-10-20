/* *********************************************************************
						S I S T E M A   D E   E J E C U C I Ó N
*********************************************************************** */

# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include "sistejec.h"  			// Definiciones y Constantes del Sistema de Ejecución

int tc = sizeof(char); 				//tamaño del char
int te = sizeof(int);  				//tamaño del integer
int tf = sizeof(float); 			//tamaño del float


/* ---------------- REGISTRO DE PRÓXIMA INSTRUCCIÓN ------------- */

int rpi;

/* -------------------- ZONA PARA EL PROGRAMA ------------------- */

float P[TAM_PROG];
int lp = 0;           				// próximo libre del programa


/* ---------------- ZONA PARA LAS CONSTANTES -------------------- */

char C[TAM_CTES];
int lc = 0;           				// próximo libre de las constantes

/* ---------------- ZONA PARA EL VECTOR DISPLAY ----------------- */
		
int D[TAM_DISP];

/* -------------- ZONA PARA EL STACK DE EJECUCIÓN --------------- */

char S[TAM_STACK];
int ls = 0;           				// tope del stack

/* -------------------------------------------------------------- */


int cargar_codgen(char * archivo)				//Toma como entrada el nombre del archivo .o que se quiere ejecutar
{												//retorna 0 si hubo un error al abrirlo
	FILE* fp;									//y sino carga las estructuras correspondientes de la MAC con el contenido del archivo
	if((fp = fopen(archivo,"rb")) == NULL)
		return 0;
	else {
		fread(P, TAM_PROG, 1, fp);
		fread(C, TAM_CTES, 1, fp);
		fclose(fp);
		return 1;
	}
}

int guardar_codgen(float codigo_programa[], char zona_strings[], char * archivo)	//Toma como entrada las estructuras que se deben guardar en el archivo .o y
{																					//el nombre del archivo, retorna 0 si hubo un error al abrirlo/crearlo
	FILE* fp;																		//y sino guarda el código generado seguido de la zona de las ctes strings
	if((fp = fopen(archivo, "wb")) == NULL)
		return 0;
	else {
		fwrite(codigo_programa, TAM_PROG, 1, fp);
		fwrite(zona_strings, TAM_CTES, 1, fp);
		fclose(fp);
		return 1;
	}
}

void interprete()
{  
	int op1,op2,op3,op4, tamentero;

	rpi = 0;

	while(P[rpi] != PARAR)
	{  
		if(ls >= TAM_STACK)
		{
			printf("\nError: Overflow del Stack de Ejecucion\n");
			exit(1);
		}
		
		if(ls < TAM_STACK - tf)
		{
			switch((int)P[rpi])
			{
				case CRCT:  			// CRCT tipo cte
					rpi++;
					switch((int)P[rpi])
					{
						case 0: 
							rpi++;
							S[ls] = (char) P[rpi];
							ls += tc;
							break;
						case 1: 
							rpi ++;
							*( (int*) &S[ls] ) = (int)P[rpi];
							ls += te;
							break;
						case 2: 
							rpi ++;
							*( (float*) &S[ls] ) = P[rpi];
							ls += tf;
							break;
					}
					rpi++;
					break;
					
				case CRVL:				// CRVL nivel despl tipo
				{ 			
					int nivel, despl, tipo, i;
					rpi++;
					nivel = (int)P[rpi++];
					despl = (int)P[rpi++];
					switch ((int)P[rpi++])
					{
						case 0:
							tipo = tc;
							break;
						case 1: 
							tipo = te;
							break;
						case 2: 
							tipo = tf;
							break;
					}
					for(i = 0; i < tipo; i++)
					{
						if(ls >= TAM_STACK)
						{
							printf("\nError: Overflow del Stack de Ejecucion\n");
							exit(1);
						}
						S[ls] = S[ D[nivel] + despl + i ];
						ls++;
					}
					break;
				}
				
				case SUM: 				// SUM tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = (int) S[ls-2 * tc] + (int)S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) += * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(float *) &S[ls-2 * tf]) += * (float *) &S[ls - tf];
							ls = (ls - tf);
							break;
					}
					break;

				case SUB: 				// SUB tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = (int) S[ls-2 * tc] - (int)S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) -= * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(float *) &S[ls-2 * tf]) -= * (float *) &S[ls - tf];
							ls = (ls - tf);
							break;
					}
					break;

				case MUL: 				// MUL tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = (int) S[ls-2 * tc] * (int)S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) *= * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(float *) &S[ls-2 * tf]) *= * (float *) &S[ls - tf];
							ls = (ls - tf);
							break;
					}
					break;

				case DIV: 				// DIV tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = (int) S[ls-2 * tc] / (int)S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) /= * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(float *) &S[ls-2 * tf]) /= * (float *) &S[ls - tf];
							ls = (ls - tf);
							break;
					}
					break;

				case INV:				// INV tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls - tc]) = - (int)S[ls - tc];
							ls = (ls - tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls - te]) = - * (int *) &S[ls - te];
							break;
						case 2: 
							(*(float *) &S[ls - tf]) = - * (float *) &S[ls - tf];
							break;
					}
					break;

				case AND:				// AND  tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = S[ls-2 * tc] && S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) = (*(int *) &S[ls-2 * te]) 
							&& * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(int *) &S[ls-2 * tf]) = (*(float *) &S[ls-2 * tf]) 
							&& * (float *) &S[ls - tf];
							ls = (ls - 2 * tf) + te;
							break;
					}  
					break;

				case OR:					// OR  tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls-2 * tc]) = S[ls-2 * tc] || S[ls - tc];
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls-2 * te]) = (*(int *) &S[ls-2 * te]) 
							|| * (int *) &S[ls - te];
							ls = (ls - te);
							break;
						case 2: 
							(*(int *) &S[ls-2 * tf]) = (*(float *) &S[ls-2 * tf]) 
							|| * (float *) &S[ls - tf];
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case NEG: 				// NEG  tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							(*(int *) &S[ls - tc]) = ! (int)S[ls - tc];
							ls = (ls - tc) + te;
							break;
						case 1: 
							(*(int *) &S[ls - te]) = ! * (int *) &S[ls - te];
							break;
						case 2: 
							(*(int *) &S[ls - tf]) = ! * (float *) &S[ls - tf];
							ls = (ls - tf) + te;   
							break;
					}
					break;

				case CMMA: 				// CMMA tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							if( (int) S[ls-2 * tc] > (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else 
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if( (*(int *) &S[ls-2 * te]) > * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if( (*(float *) &S[ls-2 * tf]) > * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;
				
				case CMME: 				// CMME tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0:
							if( (int) S[ls-2 * tc] < (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else 
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if( (*(int *) &S[ls-2 * te]) < * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if( (*(float *) &S[ls-2 * tf]) < * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case CMIG: 				// CMIG tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							if( (int) S[ls-2 * tc] == (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if( (*(int *) &S[ls-2 * te]) == * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if( (*(float *) &S[ls-2 * tf]) == * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case CMAI: 				// CMAI tipo
					rpi++;
					switch ((int)P[rpi++])
					{
						case 0: 
							if( (int) S[ls-2 * tc] >= (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else 
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if( (*(int *) &S[ls-2 * te]) >= * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if ( (*(float *) &S[ls-2 * tf]) >= * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case CMEI: 				// CMEI tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							if( (int) S[ls-2 * tc] <= (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else 
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if( (*(int *) &S[ls-2 * te]) <= * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if ( (*(float *) &S[ls-2 * tf]) <= * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case CMNI: 				// CMNI tipo
					rpi++;
					switch ((int)P[rpi++])
					{
						case 0: 
							if( (int) S[ls-2 * tc] != (int)S[ls - tc] )
								(*(int *) &S[ls-2 * tc]) = 1;
							else 
								(*(int *) &S[ls-2 * tc]) = 0;
							ls = (ls - 2 * tc) + te;
							break;
						case 1: 
							if ( (*(int *) &S[ls-2 * te]) != * (int *) &S[ls - te] )
								(*(int *) &S[ls-2 * te]) = 1;
							else
								(*(int *) &S[ls-2 * te]) = 0;
							ls = (ls - te);
							break;
						case 2: 
							if( (*(float *) &S[ls-2 * tf]) != * (float *) &S[ls - tf] )
								(*(int *) &S[ls-2 * tf]) = 1;
							else
								(*(int *) &S[ls-2 * tf]) = 0;
							ls = (ls - 2 * tf) + te;
							break;
					}
					break;

				case ALM:				// ALM nivel despl tipo
				{ 				
					int nivel, despl, tipo, i;
					rpi++;
					nivel = (int)P[rpi++];
					despl = (int)P[rpi++];
					switch ((int)P[rpi++])
					{
						case 0: 
							tipo = tc;
							break;
						case 1: 
							tipo = te;
							break;
						case 2: 
							tipo = tf;
							break;
					}	
					//copia byte a byte el tope del stack en la variable.
					ls -= tipo;
					for(i = 0; i < tipo; i++)
					{
						if(ls >= TAM_STACK)
						{
							printf("\nError: Overflow del Stack de Ejecucion\n");
							exit(1);
						}
						S[ D[nivel] + despl + i ] = S[ls];
						ls++;
					}
					break;
				}
      
				case ALOC: 				// ALOC cant_bytes
					rpi++;
					ls += (int)P[rpi++]; 
					break;

				case DMEM: 				// DMEN cant_bytes
					rpi++;
					ls -= (int)P[rpi++]; 
					break;

				case INPP: 
					rpi++;
					memset(S, 0, TAM_STACK);
					ls = 0;
					memset(D, 0, TAM_DISP);
					D[0] = 0;
					break;

				case CAST: 				// CAST tipo1 -> tipo2
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 			//char
							switch((int)P[rpi++])
							{
								case 1: 
									(*(int *) &S[ls-tc]) = (int) S[ls-tc]; 
									ls = (ls - tc) + te;
									break;
								case 2: 
									(*(float *) &S[ls-tc]) = (int) S[ls-tc];
									ls = (ls - tc) + tf;
									break;
							}
							break;
						case 1: 			//int
							if((int)P[rpi++]==2)
							{
								(*(float *) &S[ls-te]) = (*(int *) &S[ls-te]);
								ls = (ls - te) + tf;  
							}
							break;
					}
					break;
				
				case LEER: 				// LEER tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							scanf("%c", &S[ls]);
							ls += tc;
							break;
						case 1: 
							scanf("%d", (int *) &S[ls]);
							ls += te;
							break;
						case 2: 
							scanf("%f", (float *) &S[ls]);
							ls += tf;
							break;
					}
					break;
       	
				case IMPR: 				// IMPR tipo
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							printf("%c", S[ls - tc]);
							ls -= tc;
							break;
						case 1: 
							printf("%d", (*(int *) &S[ls - te]) );
							ls -= te;
							break;
						case 2: 
							printf("%f", (*(float *) &S[ls - tf]) );
							ls -= tf;
							break;
					}
					break;
    	
				case CRCTS: 			// CRCTS k
					rpi++;
					(* (int *) & S[ls]) = (int) P[rpi++];
					ls += te;
					break;

				case IMPRCS: 			// IMPRCS
					rpi++;
					printf("%s", &C[(* (int *) & S[ls-te])] );
					ls -= te;
					break;

				case BIFS: 				// BIFS desp
					rpi++;
					rpi += (int) P[rpi];
					break;
	
				case BIFF: 				// BIFF tipo desp
					rpi++;
					switch((int)P[rpi++])
					{
						case 0: 
							if(S[ls-tc] == 0) rpi += (int) P[rpi];
								ls -= tc;   
							break;
						case 1: 
							if((* (int *) &S[ls-te]) == 0) rpi += (int) P[rpi];
								ls -= te;
							break;
						case 2: 
							if((* (float *) &S[ls-tf]) == 0) rpi += (int) P[rpi];
								ls -= tf;
							break;
					}
					break;

				case ENBL:				// ENBL k
				{			
					int k; 
					rpi++;
					k = (int)P[rpi++]; 
					(* (int *) &S[ls])= D[k];
					ls += te;
					D[k] = ls;
					break;
				}

				case FINB: 				// FINB k
					rpi++;
					ls -= te;
					D[ (int)P[rpi++] ] = (* (int *) &S[ls]);
					break;
			}								// del switch (cod_op)
		}									// del if ls < TAM_STACK
		else
		{
			printf("\nError: Overflow del Stack de Ejecucion\n");
			exit(1);
		}
	}										// del while
}											// de la función interprete

void impr_codgen()
{
	int i = 0;
	while(P[i]!= PARAR)
	{
		switch((int) P[i])
		{
			case CRCT:
				printf("CRCT ");
				i++;
				printf("%d, ", (int) P[i++]);
				if(P[i-1] == 0)
					printf("%c\n", (char) P[i++]);
            else
					if(P[i-1] == 1)
						printf("%d\n", (int) P[i++]);
               else
						printf("%.2f\n", (float) P[i++]);
				break;

			case CRVL:
				printf("CRVL ");
				i++;
				printf("%d, ", (int) P[i++]);
				printf("%d, ", (int) P[i++]);
				printf("%d\n", (int) P[i++]);
				break;

			case SUM:
				printf("SUM ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case SUB:
				printf("SUB ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case MUL:
				printf("MUL ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case DIV:
				printf("DIV ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case INV:
				printf("INV ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case AND:
				printf("AND ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case OR:
				printf("OR ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case NEG:
				printf("NEG ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMMA:
				printf("CMMA ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMME:
				printf("CMME ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMIG:
				printf("CMIG ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMAI:
				printf("CMAI ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMEI:
				printf("CMEI ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CMNI:
				printf("CMNI ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CAST:
				printf("CAST ");
				i++;
				printf("%d, ", (int) P[i++]);
				printf("%d\n", (int) P[i++]);
				break;

			case ALM:
				printf("ALM ");
				i++;
				printf("%d, ", (int) P[i++]);
				printf("%d, ", (int) P[i++]);
				printf("%d\n", (int) P[i++]);
				break;

			case LEER:
				printf("LEER ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case IMPR:
				printf("IMPR ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case BIFS:
				printf("BIFS ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case BIFF:
				printf("BIFF ");
				i++;
				printf("%d, ", (int) P[i++]);
				printf("%d\n", (int) P[i++]);
				break;

			case INPP:
				printf("INPP \n");
				i++;
				break;

			case PARAR:
				printf("PARAR \n");
				i++;
				break;

			case ALOC:
				printf("ALOC ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case DMEM:
				printf("DMEN ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case IMPRCS:
				printf("IMPRCS \n");
				i++;
				break;

			case ENBL:
				printf("ENBL ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case FINB:
				printf("FINB ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;

			case CRCTS:
				printf("CRCTS ");
				i++;
				printf("%d\n", (int) P[i++]);
				break;
		}
	}        
	printf("PARAR");
}
