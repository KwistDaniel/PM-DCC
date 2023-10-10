# Parte 2 #
# ACA SE VERAN LAS COERCIONES #
# ver cuantas asignaciones tengo, creo que SOLO EN EXPRESION #



void main(){
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
}

# Linea comentario #