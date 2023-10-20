
/* ***************************************************************
       DEFINICIONES Y CONSTANTES DEL SISTEMA DE EJECUCIÓN
****************************************************************** */


/* ================= DEFINICIÓN DE CONSTANTES ==================== */

/* ---------------- TAMAÑOS PARA LAS ESTRUCTURAS ----------------- */

# define TAM_PROG    5000
# define TAM_STACK  15000
# define TAM_CTES     500
# define TAM_DISP      15
    
/* ----------------- CÓDIGOS PARA LAS INSTRUCCIONES ------------- */

/* -------------------- Instrucciones básicas ------------------- */
# define CRCT   1     // cargar una constante
# define CRVL   2     // cargar el valor de una variable

/* --------------- Instrucciones para las operaciones ----------- */
# define SUM    3     // suma de enteros
# define SUB    4     // resta de enteros
# define MUL    5     // multiplicación de enteros
# define DIV    6     // división de enteros
# define INV    7     // cambio de signo de un entero
# define AND    8     // operador lógico and
# define OR     9     // operador lógico or
# define NEG    10    // operador lógico not
# define CAST   101   // conversión de tipo
# define CMMA   11    // operador relacional aritmético por mayor
# define CMME   12    // operador relacional aritmético por menor
# define CMIG   13    // operador relacional aritmético por igual
# define CMAI   14    // operador relacional aritmético por mayor o igual
# define CMEI   15    // operador relacional aritmético por menor o igual
# define CMNI   16    // operador relacional aritmético por no igual

/* -------------------- Instrucciones básicas ------------------- */
# define ALM    17    // almacenar un valor en una variable

/* ------------------ Instrucciones para E / S ------------------ */
# define LEER   18    // lee un char, int o float
# define IMPR   19    // imprime un entero o char o float o constantes de los tipos anteriores

/* ----------- Instrucciones para control de secuencia ---------- */
# define BIFF   20    // bifurca por falso
# define BIFS   21    // bifurca siempre

/* -------------------- Instrucciones básicas ------------------- */
# define INPP   22    // inicia el programa
# define PARAR  23    // termina el programa
# define ALOC   24    // asigna memoria para variables y las inicializa
# define DMEM   25    // libera memoria

/* ----------- Instrucciones para manipular subrutinas ---------- */
# define CRDI   26    // carga la dirección de una variable
# define CRVLI  27    // cargar valor indirecto
# define ALMI   28    // almacenar indirecto
# define ENPR   29    // guardar el viejo D[k]
# define CHPR   30    // invocar a un procedimiento
# define RTPR   31    // retorno de un procedimiento
# define ENBL   102   // Entrada a Bloque
# define FINB   103   // Salida de Bloque

/* ------------ Instrucciones para manipular strings ------------ */
# define IMPRCS 49    // imprime una constante string
# define CRCTS  52    // carga la posición de comienzo de una constante string

/* ------------ Instrucciones para manipular arreglos ----------- */
# define CRVV   32    // cargar valor de un elemento del vector
# define ALMV   33    // almacenar valor de un elemento del vector


/* ================== DEFINICIÓN DE FUNCIONES ==================== */

int cargar_codgen(char *);
int guardar_codgen(float [], char [], char *);
void interprete();
