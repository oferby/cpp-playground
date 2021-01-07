#include<iostream>
#include<string>

using namespace std;


class Small {

};

class Large {

};

// pass by value for small objects
void f1(Small s);

/* 

pass const by reference for large objects that 
don't need update

*/
void f2(const Large& l);

// preffer that if you need to update l. 
// we can pass and check for nullptr 
void f3(Large* l); 

// to use in array of int
void f4(int* pi); 

// all are the same. in f7, the size is ignored.
void f5(int* i);
void f6(int i[]);
void f7(int i[1000]);

// the size of the array is unknown to the called function. add size as param.
void f8(int* arry, int size);

void f9(int (&r)[4]) {};

template<int N>
void f10(int (&r)[N]) {
    cout << "N: " << N << "\n";
};


// lists 

template<class T>
void f11(initializer_list<T>){};

template<class T, int N>
void f11(T (&r)[N]){};

struct  S
{
    int i;
    string s;
};

void f11(S s){};

void f11(int){};

// unspecified number of args
void f12(const char* c...){

    while ( *c != 0 )
    {
        cout << *c;
        ++c; 
    }

    cout << "\n";
    
    // va_list ap;
    // va_start(ap, c);

    // for (;;) {
    //     char* p = va_arg(ap,char*);
    //     if (p == nullptr) break;
    //     cout << p;
    // }

};

void f13(initializer_list<string> l) {

    for (auto &s : l) {
        cout << s << " ";
    };

    cout << "\n";

}

// default parameters
void f14(int i, int j =10){
    cout << "i: " << i << ", j: " << j << "\n";
}


// pointer to a function
void (*f15)(int,int);

int main() {

    int x1[] = {1,2,3,4};
    f9(x1);

    int x2[] = {1,2};
    // f9(x2); error 


    f10(x1);


    // use of lists for parameters

    f11({1,2,3,4}); // use initializer_list
    f11(x1);        // use array r with N elements
    f11({1,"my string"});
    f11({1});       // initializer_list takes priority over f11(int)

    // unspecified number of args
    f12("hello world!");
    f13({"hello","world"});


    // default parameters
    f14(2);
    f14(3,4);

    // pointer to a function
    f15 = &f14;
    f15(8,9);
}