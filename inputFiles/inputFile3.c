float pi;
int radius;

void areaCalc() {
    float area;
    area = pi * radius * 2;
}

void perimeterCalc() {
    int p;
    p = radius + 10;
    q = p + 5;   // semantic error: undeclared variable q
}
