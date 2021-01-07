#include <iostream>

using namespace std;

struct  Vector
{
    int sz;
    double* element;
};

void f(Vector v, Vector& refv, Vector* pv) 
{
    int i1 = v.sz; // use . to access member
    int i2 = refv.sz;
    int i3 = pv->sz; // use -> to access through pointer 
}


int main() {

    // user-defined types


    Vector v;

    v.element = new double[5];
    v.sz = 5;


    // casting

    auto x1 = double{2}; // x1 == 2.0
    
    int x3 = 2;
    auto x2 = double{ x3 }/4; // x2 == 0.5
    cout << x2 << "\n";

    /* 
    
    error - {} does not do nerrowing from double to int

    int x4 = int { 1.2 };

    */

   // nerrowing double to int
   int x4 = static_cast<int>(2.3);

    // can be done but not safe
   int x5 = int(4.5); 
   
    

}

