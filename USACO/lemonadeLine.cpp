#include<bits/stdc++.h>
#define int long long 
using namespace std;
signed main(){
    freopen("lemonade.in", "r", stdin);
    freopen("lemonade.out", "w", stdout);
    int n;
    cin >> n;
    vector<int>a(n);
    for(int i=1;i<=n;i++){ cin >> a[i];
    sort(a.rbegin(),a.rend());
    int count=0;

    for(int i = 0 ;i < n; i++){
        if(a[i]>=count){
            count++;
        }
    }
    cout << count;
    return 0;
}