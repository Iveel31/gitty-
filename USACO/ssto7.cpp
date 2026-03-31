#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
#define int long long
signed main()
{
	 freopen("div7.in", "r", stdin);
     freopen("div7.out", "w", stdout);
    int n;
    cin >> n;
    vector<int>a(n),prefix_sum(n+1,0);
    for(int i = 0 ; i < n ; i++) {
        cin >> a[i];
        prefix_sum[i]=prefix_sum[i-1]+a[i];
        
    }
    vector<int>first_seen(7,-1);
    int ans=0;
    for(int i = 0 ; i < n; i++){
       int r = prefix_sum[i]%7;
       if(first_seen[r]==-1){
           first_seen[r]=i;
       }
       else{
           ans=max(ans,i-first_seen[r]);
       }
    }
    cout << ans;
    return 0;
}