int g;
char c;

void test() {
    int g;   // local redeclaration allowed
    float z;
    g = c + 20;
    z = g * 1.5;
}

void another() {
    int k;
    k = g + 100;
    g = k - 50;
}
