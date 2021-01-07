#include<iostream>
#include<vector>

using namespace std;

class PrintMe {
    public:
        void operator()(char* c) {
            cout << c << " ";
        }
};


int main(int argc, char* argv[]) {

    PrintMe p;
    for (int i = 0; i < argc; i++ ) {
        // cout << argv[i] << " ";
        p(argv[i]);
    }

    cout << "\n";


    // Lambda with a name
    auto PrintMeToo = [](char* c) { cout << c << " ";};


    for (int i = 0; i < argc; i++ ) {
        PrintMeToo(argv[i]);
    }

    cout << "\n";

    

}
