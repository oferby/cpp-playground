// g++ -o test1 test1.cpp


#include <iostream>

using namespace std;


class TestClass {

    double* element;

    public:
        TestClass(): element{new double[10]} {
            cout << "created\n";
        }

        int get_size() { return 10;}

        ~TestClass() {
            cout << "deleted\n";
            delete[] element;
         }

};

bool operator==(TestClass& a, TestClass& b) {
    return a.get_size() == b.get_size();
}

int main() {
    std::cout << "Hello World!\n";

    int v[10] {0,1,2,3,4,5,6,7,8,9};

    for (int i = 0; i < 10; i++)
    {
        cout << v[i];
        cout << "\n";
    }
    
    cout << "another foor method:\n";

    int v2[] {11,22,33,44,55};

    for (auto x: v2) {
        cout << x << "\n";
    }

    cout << "yet another foor method:\n";
    for (auto x : {21,22,23,24,25}) {
        cout << x << "\n";
    }

    cout << "increment values in array\n";

    for (auto& x: v) {
        ++x;
    }
    for (auto x: v) {
        cout << x << "\n";
    }

    TestClass a;
    TestClass b;
    a == b;
}