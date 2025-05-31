#include <bits/stdc++.h>
using namespace std;
#define all(c) (c).begin(), (c).end()
#define pb push_back 
#define f first 
#define s second 
typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;

mt19937 rng(chrono::system_clock::now().time_since_epoch().count());
long long random(long long a, long long b){
    return a + rng() % (b - a + 1);
}

string randomstr() {
    string xd = "";
    int n = random(3, 50);
    for(int i = 0; i < n; i++)
        xd += ('a' + random(0, 27));
    return xd;
}
ofstream out("in.txt");

vector<string> op = {"INSERT", "SEARCH", "DELETE"};

int main() {

    int n = 100000;
    map<int, string>mp;
    mp[2] = "123";
    if(mp.count(2)) 
        cout << "OK" << endl;
    mp.erase(2);
    if(mp.count(2)) 
        cout << "OK" << endl;

}
