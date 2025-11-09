char grade;

void display() {
    int marks;
    marks = grade + 5;
}

void compute() {
    int marks; 
    float avg;
    marks = 90;
    avg = marks / 2;
    grade = avg + 1; // type mismatch warning
}
