# Parte 2 #
# Funcion main no definida #
# Parametro con & y [] al mismo tiempo #
# Invocaciones con tipos y cantidades distintas de parametros #

int a;
int b;

void func1(int &a[],int b){

}

void func2(int a[],int b[]){

}

void func3(int a){}

void func4(){
    int a, b[];
    char c;
    func2(b,b[1]);
    func3(c,a);
}

# Linea comentario #