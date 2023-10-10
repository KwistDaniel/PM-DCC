:: Limpiar y generar el ejecutable
make clean
make
:: Compilacion de los lotes de prueba
:: Lotes de la catedra:
:: Sin errores:
::ucc -c prog1se.c
::ucc -c prog2se.c
::ucc -c prog3se.c
::ucc -c prog4se.c
::ucc -c prog5se.c
::ucc -c prog6se.c
::ucc -c prog7se.c
::ucc -c prog8se.c
:: Con errores:
::ucc -c lote_error_decl_asig_param_double.c
::ucc -c lote_error_declaraciones_asignaciones_parametros.c
::ucc -c lote_errores_ES.c
::ucc -c lote_errores_expresiones_while_if.c

:: Lotes propios:
:: Sin errores:
::ucc -c lote1se.c
::ucc -c lote2se.c
:: Con errores:
::ucc -c CElote1.c
::ucc -c CElote2.c
::ucc -c CElote3.c

:: Lotes propios tpm2:
:: Con errores:
ucc -c CE2lote1.c
ucc -c CE2lote2.c
ucc -c CE2lote3.c
ucc -c CE2lote4.c
ucc -c CE2lote5.c
ucc -c CE2lote6.c
ucc -c CE2lote7.c
ucc -c CE2lote8.c


:: Ejecucion de los lotes de prueba
:: ucc -o prog3se.o
:: ucc -o lote1se.o
:: ucc -o lote2se.o


PAUSE

DEL ucc.exe