#include<bits/stdc++.h>
#define int long long
using namespace std;
signed main(){
    int n;
    cin >> n;
    vector<int>a(n);
    map<int,int> last;
    for(int i = 0 ; i < n; i++) cin >> a[i] ;
    int l = 0 , ans = 0;
    for(int r = 0; r < n; r++){
        int song=a[r];
        if(last.count(song)){
            l=max(l,last[song]+1);
        }
        last[song]=r;
        ans=max(ans,r-l+1);
    }
    cout << ans;
    return 0;
}
