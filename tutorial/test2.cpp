#include<iostream>

class C {
    public:
        int i;
};


int main(){

C c1 {1};
C c2 = c1;

c1.i = 12;

std::cout << &c1 << " " << &c2 << "\n";
std::cout << c1.i << " " << c2.i << "\n";

}