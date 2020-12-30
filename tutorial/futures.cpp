// when using thread - need to compile like this -> g++ -pthread -o ...
#include<future>
#include<iostream>
#include<thread>
#include<vector>

using namespace std;

int sum(const int* start, const int* end){
    int s = 0;
    for (auto p = start; p != end; ++p) {
        s+=*p;
    }
    return s;
}

int main(){

    vector<int> v = {1,2,3,4,5,6,7};

    int first = v[0];
    int last = v[0] + v.size();

    auto a1 = async(sum, &v[0], &v[0] + 4);
    auto a2 = async(sum, &v[0] + 4, &v[0] + v.size());
    
    cout << a1.get() + a2.get() << "\n";
   
    
}