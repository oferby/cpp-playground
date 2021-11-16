#include <iostream>

using namespace std;


// struct is a class whose members are public by default
struct MyStruct {
    int age;
    char* name; 

};


class MyClass {

    public:

        // delegating constractor
        MyClass() : MyClass(0) { /* this will call the next constructor */ };

        //constructor with members init
        MyClass(ulong s): sz{s}, element {new double[sz]} {
            cout << "creating " << this << "\n";
        }

        // see usage
        MyClass(initializer_list<double> lst): sz{lst.size()}, element{new double[lst.size()]} {
            copy(lst.begin(), lst.end(), element);
        } 
        
        double& operator[](int i) {return element[i];}

        // const indicate the the object is not to be changed
        ulong get_size() const { return sz;}

        ulong get_size_and_add() const {
            // num_of_tries is mutable
            num_of_tries++; 
 
            return sz;
        }

        // get reference to the class
        MyClass& get_ref() { return *this; };
        
        // destructor
        ~MyClass() {
            cout << "deleting " << this << "\n";
            delete[] element;
        }
    
    private:
        ulong sz;
        double* element;

        // in case the class is const, this still can be modified
        mutable int num_of_tries = 0;

};

bool operator==(MyClass& a, MyClass& b) { 

    return a.get_size() == b.get_size();

}

enum class MyEnum { red, yellow, green };

enum AlsoMyEnum { red, green, blue };

AlsoMyEnum& operator++(AlsoMyEnum& current) {
    switch (current)
    {
    case red:
        return current = AlsoMyEnum::green;
    
    default:
        return current = AlsoMyEnum::red;
        
    }   
} 

class Complex {
    double re, img;

    public:
        Complex(): re{0}, img(0) {}
        Complex(double re, double img): re{re},img{img} {}
        double get_real() {return re; }
        double get_img() { return img; }
        Complex& operator+=(Complex b) {
            re+=b.re;
            img+=b.img;
            return *this;
        }
};

// pass by value. a and b are copied
Complex operator+(Complex a, Complex b) { 
        cout << "a address inside: " << &a << "\n";
        return a+=b; 
    };

// pass by reference
bool operator==(Complex& a, Complex& b) {
    return a.get_img() == b.get_img() && a.get_real() == b.get_real();
};

bool operator!=(Complex& a, Complex& b){ return !(a==b); };


// Abstruct class - used as interface
class Container {
    public:
    // virtual means - maybe redefined later
        virtual double& operator[](int) = 0;
        virtual ulong size() const = 0;
        virtual ~Container(){}
};

class Vector: public Container {
    double* element;
    ulong sz;

    public:
        Vector(ulong s) : sz {s}, element { new double[s]} {} 
        Vector(initializer_list<double> lst);
        Vector(const Vector& v);
        Vector& operator=(const Vector& v);
        double& operator[](int i) { return element[i];};
        ulong size() const { return sz; };
};

// polymorphism
ulong get_container_size(Container& c){
    return c.size();
}

Vector::Vector(initializer_list<double> lst) : sz {lst.size()} , element { new double[lst.size()]} {
    for (auto i = 0; i < sz; i++)
    {
        copy(lst.begin(), lst.end(), element);
    }
    
}


// copy constructor - prevent same pointer member on 2 classes
Vector::Vector(const Vector& v): sz {v.sz}, element { new double[v.sz]} {
    for (int i = 0; i<sz ; i++) {
        element[i] = v.element[i];
    }
}

// copy assignment
Vector& Vector::operator=(const Vector& v) {
    this->sz = v.sz;
    this->element = new double[this->sz];
    for (auto i = 0; i < this->sz; i++)
        {
            this->element[i] = v.element[i];
        }
    return *this;
}


int main() {


    MyEnum en = MyEnum::red;

    AlsoMyEnum enm = AlsoMyEnum::red;
    ++enm;



    // assert used for compilation time errors
    const int SPEED = 3;    
    static_assert(SPEED==3, "Speed not 3!");


    MyClass clazz(5);
    double c3 = clazz[3];

// can be done because of initializer_list in constructor
    MyClass clazz2 = {1,2,3,4};
    cout << "clazz == clazz2? " << ( clazz == clazz2 ) << "\n";

    // new class with memberwise copy
    MyClass clazz3 = clazz2;
    cout << "clazz3 == clazz2? " << ( clazz3 == clazz2 ) << "\n";


    cout << "complex obj\n";

    Complex a(1.5,2.6);
    // this is the same
    Complex b {10,20};

    cout << "a address: " << &a << "\n";
    Complex c = a+b;

    cout << "a: re = " << a.get_real() << ", img = " << a.get_img() << "\n"; 
    cout << "c: re = " << c.get_real() << ", img = " << c.get_img() << "\n"; 
    
    a+=b;
    cout << "a: re = " << a.get_real() << ", img = " << a.get_img() << "\n"; 

    if (a==b){
        cout << "a==b\n";
    } else
    {
        cout << "a!=b\n"; 
    }
    
    Vector v {1,2,3,4,5};

    cout << "v size is: " << v.size() << "\n";

    Vector v1 {v};

    cout << "v1 size is: " << v1.size() << "\n";

    Vector v2 { 2, 2, 2, 2};

    cout << "v2 size is: " << v2.size() << "\n";

    v2 = v1;

    cout << "v2 size is now: " << v2.size() << "\n";
}