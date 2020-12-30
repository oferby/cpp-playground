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


}

