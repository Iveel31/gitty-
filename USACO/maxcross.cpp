#include <bits/stdc++.h>
#define int long long
using namespace std;

signed main()
{
    freopen("maxcross.in", "r", stdin);
    freopen("maxcross.out", "w", stdout);
	int n, k, b;
	cin >> n >> k >> b;
	vector<int>a(b),light(n+1,0);
	for(auto &i : a) cin >> i;
	int m=0;
	for(int i = 1 ; i <= n ; i++) {
		if(a[m]==i && m < b) {
			light[a[m]]=1;
			m++;
		}
	}
	vector<int>prefix_sums(n+1);
	prefix_sums[0]=0;
	for(int i = 1 ; i <= n ; i++) {
		prefix_sums[i]=light[i]+prefix_sums[i-1];
	}
	int r=k;
	int min_num_of_signals=1e9;
	for(int i = 1 ; i <= n-k+1 ;i++){
	    min_num_of_signals=min(min_num_of_signals,prefix_sums[r]-prefix_sums[i-1]);
	    r++;
	}
	cout << min_num_of_signals ;
	return 0;
}