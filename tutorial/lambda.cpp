#include<iostream>
#include<functional>
using namespace std;

class LambdaInClass {

    int x1;
    int x2;

    public:
        void doThis() {
            // get access by reference to all class members
            [this](int y) { 
                if (x1 > y | x2 < y ) { /* ... */ }
            };
        }

};


int main() {

    int x;
    int y;
    char c1;
    char c2;

    [](int v) { 
        // if (v == x) 
        // can't get access to x

    };

    [&](int& xx) { 
        if (xx == x ) {
            // OK, [&]  get access to all vars by reference
        }
    } ;

    [=](int xx) { 
        if (xx == x) {
            // OK, get access to all local vars by value
        }
    }; 

    [x,&y](int z) {
        if (z == x | z == y) {
            // OK, get access only to X by value and Y by reference
        }

    };

    // get access to all vars by reference other then the
    // ones in the list which will be accessed by value
    [&, x, c1](int z, char c) {
        if (y == z) {
            
        }

        if ( c1 == c ) {

        }

        // can't do that as accessed objects are mutable inside lambda body
        // x++;
    };

    // get access to all vars by value other then the
    // ones in the list which will be accessed by reference
    [=, &x, &c1](int z, char c) {
        if (y == z) {
            
        }

        if ( c1 == c ) {

        }

        // we can do that because it is passed by value
        x++;

    };

    // can do this inside the lambda
    [&x] () mutable  {  x++;  };


    // lambda with return value 
    auto ret1 = []{ cout << "lambda without parameters "; return "done!";};
    
    // use as a function

    function<int(int, int)> f1 = [](int x, int y) { return x > y ? x : y; };

    auto ret2 = f1(2,3);

    // name the lambda
    auto do_this = [](int x, int y) { return x > y ? x : y; };
    cout << "MAX: " << do_this(4,5) << "\n";

    // pointer to lambda function. should not capture anything in []
    double (*p1)(double) = [](double d) { return ++d; };

    cout << "p1: " << p1(4.2) << "\n";

}