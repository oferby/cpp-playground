#include <iostream>

using namespace std;

int main() {

    char c = 'Y';

    // right side & stands for - address of the var
    char* pointer_to_c = &c; 

    // right side * is derefference operator: value of the pointer to the var
    char copy_of_c = *pointer_to_c; 


    cout << "char C is: ";
    cout << copy_of_c;
    cout << "\n";

    char& y = c; // y is a reference to c
    ++y;
    cout << "char C is: " << c << "\n"; 

    // null pointer
    char* Z = nullptr;

    // ***   type of pointers  ***


    // pointer to int
    int i = 10;
    int* pi = &i;

    // pointer to pointer of char
    char** ppc;

    // array of 15 pointers to ints
    int* api[15];

    // pointer to function taking a char* argument, return an int
    // source function
    int fp(char*);
    // the pointer to the function
    int (*fp)(char*);

    // function taking char* and returning pointer to int
    int* fpi(char*);
    
    // pointer to obj with unknown type
    void* ut;
    ut = pi;
    // dereferencing void pointer
    int* dpi = static_cast<int*>(ut);






}