# Parte 2 #
# Coerciones #
int b[10];

void fun1(int x){
    x = x+1;
}

void fun(int y[],int z){
    fun1(y[z]);

}

void main(){
    int j[10];
    int k;
    float a;
    int b;
    char c;
    # Las siguientes asignaciones son correctas #
    a = b;
    a = c;
    b = c;
    # Las siguientes asignaciones son incorrectas #
    b = a;
    c = a;
    c = b;
    b = a;
    fun(j,10);
}

# Linea comentario #