# Parte 2 #
# Lote con errores varios sobre arreglos y parametros en llamado a funciones #
# case 78 y 79, 90 y 91 #


void func(int a[]){

}

void func2(int b){
}

void func3(int a[]){
}

void main(){
    int a[1];
    int b;
    int c = 0;
    char d;
    func2(d);
    func(a);
    func(a[]);
    func2();
    func2(b);
    func2(b,c);
    if(c == 0){
        b[1] = 1;
        a = 0;
    }
}

# Linea comentario #