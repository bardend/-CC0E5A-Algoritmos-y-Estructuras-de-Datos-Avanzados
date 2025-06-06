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
    int n = random(3, 100);
    for(int i = 0; i < n; i++)
        xd += ('a' + random(0, 27));
    return xd;
}
ofstream out("../Proyect2/ScapegoatTree.Tests/random_case.txt");
ofstream ou("../Proyect2/ScapegoatTree.Tests/worst_case.txt");

vector<string> op = {"INSERT", "SEARCH", "DELETE"};

int main() {

    int n;
    cin >> n;

    map<int, string>mp;
    vector<int>vec;
    vec.push_back(0);


    map<int, string>m;
    vector<int>ve;
    ve.push_back(0);

    out << n <<  endl;
    ou << n << endl;


    for(int i = 0; i < n; i++) {
        string id = op[random(0, 2)];
        
        if(id == op[0]) {
            //INSERT KEY VALUE
            int tmp1 = random(1, 300000);
            string tmp2 = randomstr();

            mp[tmp1] = tmp2;
            vec.push_back(tmp1);

            m[(i+1)] = tmp2;
            ve.push_back(i+1);

            out << id << " " << tmp1 << " " << tmp2 << endl;
            ou << id << " " << (i+1) << " " << tmp2 << endl;

        }
        else if(id == op[1]) {
            /*
             * SEARCH key value, if there is :)
             * SEARCH key NO, if there no
             */
            int tmp1 = vec[random(0, (int)vec.size()-1)];
            out << id << " " << tmp1 << " ";

            int tp1 = ve[random(0, (int)ve.size()-1)];
            ou << id << " " << tp1 << " ";


            if(mp.count(tmp1)) 
                out << mp[tmp1] << endl;
            else 
                out << "NO" << endl;

            if(m.count(tp1))
                ou << m[tp1] << endl;
            else 
                ou << "NO" << endl;

        }
        else {
            /*
             * DELETE key YES if there is :)
             * DELETE key NO if there isn't
             */

            int tmp1 = vec[random(0, (int)vec.size()-1)];
            out << id << " " << tmp1 << " ";
            if(mp.count(tmp1))  {
                out << "YES" << endl;
                mp.erase(tmp1); 
            }
            else 
                out << "NO" << endl;

            int tp1 = ve[random(0, (int)ve.size()-1)];
            ou << id << " " << tp1 << " ";
            if(m.count(tp1)) {
                ou << "YES" << endl;
                m.erase(tp1);
            }
            else 
                ou << "NO" << endl;

        }
    }
}
