# Parte 2 #
# Lote con errores varios sobre arreglos y parametros en llamado a funciones #
# case 78 y 79; 90, 91 y 98  #


void func(int a[], int b){}

void func2(int a[], int &b){}

void func3(int a[]){}

void func4(int &a){}

void func5(int a, char b){}

void main(){
    int a[1];
    int b;
    int c = 0;
    char d, e[5];

     # Invocaciones Correctas #
     func(a,b);
     func(a,a[0]);
     func2(a,a[0]);
     func5(b,d);
     func3(a);
     func4(b);
     func4(a[0]);

    # Invocaciones Incorrectas #
    func(a,a);
    func(a,d);
    func(a[1],a);
    func2(a,a);
    func2(b,a);
    func3(b);
    func3(a[0]); # 91 98 #
    func3(2+3); # 91 98 #
    func4(a); # 91 #
    func4(2);
    func4(2+3); # 93 #
    func5(d,b);
    func5(d);
    func5(d,b,b);
    func5(e[1],d);

    if(c == 0){
        b[1] = 1;
        a = 0;
    }
}

# Linea comentario #