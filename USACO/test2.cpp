#include<bits/stdc++.h>
using namespace std;
vector<int>a;
int n;
int main(){
    int k;
    cin >> n >> k;
    a.resize(n);
    for(int i = 0 ; i < n; i++) cin >> a[i];
    for(int i = 0 ;i < k ; i++){
        int x;
        cin >> x;
        int ans=lower_bound(a.begin(),a.end(),x)-a.begin();
        cout << ans+1;
        cout << endl;
    }
    return 0;
}