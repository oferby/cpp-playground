#include<iostream>

using namespace std;


char fn() {


}


class C1 {
    int value;
    public:
        int get() { 

            // we can do that
            value++;    
            return value; 
        };
};


class C2 {
    int value;
    public:
        
        C2(int val) {value = val;}
        
        int get() const { 
            
            // can't do that because of const
            // value += 6;
            return value; }

};



int main() {

    char* pc; 

    const char* pc1;  // pointer to constant char
    char* const pc2 = nullptr; // constant pointer to a char
    const char* const pc3 = nullptr; // constant pointer to constant char


}