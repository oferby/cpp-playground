#include<iostream>
#include<string>

using namespace std;

union Value
{
    char c;
    int i;
};

enum Type {
    CHAR,
    INT
};

// declaration to be defined later. if we want to define later, 
// we have to state the type
enum AnotherType : char ;

enum class Warning {
    red, yello, green
};

// unnamed enum. use as a set of constants
enum { right_dir, left_dir=4};

struct Address {
    const int id;
    string name;
    string street;

    // using union
    Type t;
    Value v; 

    // anonymous union, need to check the type before using
    union 
    {   
        int num;
        char cc;
    };
    
    Warning w = Warning::green;

    // unnamed enum used as a set of constants
    int direction = right_dir;

};

void print_me(const Address& addr ) {
    cout << "Name: " << addr.name << ", id: " << addr.id << "\n";

    // use enumaration int value
    if (addr.t == 1) {
        cout << "V: " << addr.v.i << "\n";
        cout << "num: " << addr.num;
    } else
    {
        cout << "V: " << addr.v.c;
    }

    cout << "\n'";
    
    // with class enum we have to do that to use int value 
    if ( static_cast<int>(addr.w) == 1) { 
        //...
    }



}

struct A {
    int b;

    // create default contstructor
    A() = default;

    // prevent copy
    A & operator = (const A &) = delete;
    A (const A &) = delete; 

};


int main() {

    Address add1 {123};
    add1.name = "ofer";

    Address add2 = {
        234,
        "smadar",
        "Keshet"
    };

    // pointer to struct
    Address* ap = &add1;

    // member accessed with pointer using ->
    cout << "Name: " << ap->name << ", id: " << ap->id << "\n";

    add2.t = Type::INT;
    add2.v.i = 5; 
    print_me(add2);

    Address add3 = add2;
    add3.num = 7;
    print_me(add3);

    cout << &add2 << " " << &add3 << "\n";
    
    // error
    cout << add3.v.c << "\n";
    cout << add3.cc << "\n";

    // use default constructor
    A a1;

    // can't do that - copy deleted
    // A b {a1};
    // or 
    // A b = a1;
    // or that
    // A b;
    // b = a1;


}


enum AnotherType : char {
    RIGHT, LEFT
};