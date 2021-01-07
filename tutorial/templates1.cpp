#include <iostream>

using namespace std;

template<typename T>
class Vector {
    T* element;
    int sz;

    public:
        Vector(int);
        ~Vector() { delete[] element; };
        
        int get_size() const { return sz; };
        
        T& operator[](int i);
        const T& operator[](int i) const;
        
        // to use for each - we need begin and end:
        T* begin(Vector<T>& x) {
            return &element[0];
        };

        T* end(Vector<T>& x) {
            return &element[0] + sz;
        };
        T* begin() {
            return &element[0];
        };

        T* end() {
            return &element[0] + sz;
        };

};

template<typename T>
Vector<T>::Vector(int s) {
    if ( s < 0 ) {
        throw out_of_range{"negetive number provided."};
    }
    sz = s;
    element = new T[sz];

    for (int i = 0; i < sz ; i++ ){
        element[i] = i;
    }

};

template<typename T>
const T& Vector<T>::operator[](int i) const{
    return element[i];
}

template<typename T>
T& Vector<T>::operator[](int i){
    return element[i];
}

template<typename T>
void printme(Vector<T>& v) {
    for (auto& x : v) {
        cout << x << " ";
    }
    cout << "\n";
}


// function template
template<typename T>
class LessThan {
    const T val;
    public:
        LessThan(T v) : val {v} {};
        bool operator()(const T&x) const { return x < val; };
};


// run predicate on each value
template<typename V, typename P>
int count(V& v, P pred) {

    int i = 0;
    for (auto &x : v) {
        if (pred(x)) i++;
    }

    return i;
};


template<typename T>
void print_head(T head) {
    cout << " head: " << head << "\n";
}


// to end recursion
void print_all(){};


// Variadic Template
template<typename T, typename ... Tail>
void print_all(T head, Tail ... tail) {

    print_head(head);
    print_all(tail...);

}

// aliasing
template <typename Key, typename Value>
class Map {

};

// template<typename Value>
// using stringMap = Map<string,Value>;


int main() {

    Vector<int> vi {5};
    printme(vi);

    Vector<double> vd {4};
    printme(vd);

// using function template

    LessThan<int> lt5(5);
    LessThan<double> lt1_2(1.2);

    cout << "number of members less than 5: " << count(vi, lt5) << "\n";
    cout << "number of members less than 1.2: " << count(vd, lt1_2) << "\n";

    // 
    cout << "Lambda expression:\n";
    cout << "number of int less than 5: " << count(vi, [&](const int& x){return x < 5;}) << "\n";

    print_all("HEAD", "TAIL1", "TAIL2", "TAIL3");


    // Using alias - Map for <string,int>
    // stringMap sm<int>({"s",12});


    

}