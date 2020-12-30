// when using thread - need to compile like this -> g++ -pthread -o ...

#include<thread>
#include<vector>
#include<iostream>
#include<mutex>
#include<condition_variable>

using namespace std;


mutex m;
condition_variable cond;


int main() {

    void find_max(const vector<int>& v, int* max, int* final_res); 

    vector<int> v1 = {1,22,424,434,424,563};
    vector<int> v2 = {53,53,645,2344,5463,2342,553,33};

    int max1 = 0;
    int max2 = 0;
    int final_res = 0;

    thread t1 { find_max, v1, &max1, &final_res };
    thread t2 { find_max, v2, &max2, &final_res };

    t1.join();
    t2.join();

    // find_max(v1, &max1);
    // find_max(v2, &max2);

 
    cout << "max1: " << max1 << "\n";
    cout << "max2: " << max2 << "\n";

    if (max1 > max2) {
        cout << "max1 is bigger: ";
        // final_res = max1;
    }
    else
    {
        cout << "max2 is bigger: ";
        // final_res = max2;
    }
    cout << final_res << "\n";


    // with condition



}

void find_max(const vector<int>& v, int* max, int* final_res) {

    for ( auto p = v.begin(); p!=v.end(); ++p) {
        if (*p > *max)
            *max = *p;
    }

    
    // m.lock();

    unique_lock<mutex> lck {m};
    
    if (*max > *final_res)
        *final_res = *max;
    
    // m.unlock();
}