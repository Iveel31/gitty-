#include<bits/stdc++.h>
using namespace std;
vector<int>a;
int n;
int solve(int x){
    int l = 0 , r=n-1;
    int ans=-1;
    while(l<=r){
        int m=l+(r-l)/2;
        if(a[m]>=x){ ans=m; r=m-1;}
        else{ l=m+1; }
    }
    return ans;
}
int main(){
    int k;
    cin >> n >> k; 
    a.resize(n);
    for(int i = 0 ;i  < n ;i++){
        cin >> a[i];
    }
    for(int i = 0 ;i < k; i++){
        int k;
        cin >> k;
        int ans=solve(k);
        if(ans==-1) cout << n+1;
        else cout << ans+1;
        cout << endl;
    }
    return 0;
}