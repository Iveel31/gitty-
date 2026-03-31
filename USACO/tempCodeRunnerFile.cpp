#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >> n;
    while(n--){
    int a, b, c;
    cin >> a >> b >> c;
    if(a==(b+c) || (a+c)==b || (a+b)==c) cout << "YES" << endl;
    else cout << "NO" << endl; 

    }

    return 0;
}