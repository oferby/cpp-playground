#include<iostream>
#include<string>
#include<vector>
#include<list>
#include<map>
#include<unordered_map>

#include<queue>
#include<deque>
#include<stack>


using namespace std;

void func(int n);

struct Entry
{
    string name;
    int number;

};


template<typename C, typename V>
vector<typename C::iterator> find_all_for_type(C& c, V v){

    vector<typename C::iterator> res;

    for (auto p = c.begin(); p!=c.end(); ++p ) {
        if (*p==v)
            res.push_back(p);
    };

    return res;
}



vector<string::iterator> find_all(string& s, char c) {
    vector<string::iterator> res;

    for (auto p = s.begin(); p!=s.end(); ++p ) {
        if (*p==c)
            res.push_back(p);
    };

    return res;
}

int main() {
    
    vector<Entry> v = {{"ofer", 123}};

    v.push_back({"smadar", 234});


    v.begin();

    map<string, int> m = {
        {"o", 123},
        {"s", 234}
    };
    

    string s = {"a man walked into a trap and fell down..."};
    auto itr = find_all(s,'o');

    for (auto i : itr) {
        cout << *i << " ";
    } 
    cout << "\n";


    vector<int> i = {1,2,3,1,4,5,1,7,3};
    auto itr2 = find_all_for_type(i, 1);
    for (auto x : itr2) {
        cout << *x << " ";
    } 
    cout << "\n";

    func(10);

    

}    

void func(int n) {
        int an[n];
        an[0] = n;
        cout << "an[0]=" << an[0] << "\n";
        vector<int>vi(n);
        vi[0] = n;
        cout << "vi[0]=" << vi[0] << "\n";
};

// array on static storage
int a1[10];

void other() {
    // array on stack
    int a2[20];
    
    // array on free store ( heap )
    int* pa3 = new int[20];

}

int high_value(initializer_list<int> val) {

    int high = numeric_limits<int>::min();

    if ( val.size() == 0) return high;

    for (auto v : val)
        if (v > high ) 
            high = v;

    return high;

}