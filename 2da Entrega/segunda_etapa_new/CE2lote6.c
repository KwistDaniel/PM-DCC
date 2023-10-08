# Parte 2 #
# Lote con errores de main #
# definida mas de 1 vez #

void main(){
    int a;
    int b;
    int c = 0;
    c = a + b;
}

void main(){
    int a = 1;
    if(a == 1){
        int a;
        a = 0;
    }
}

# Linea comentario #