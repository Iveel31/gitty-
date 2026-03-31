#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
    freopen("cownomics.in", "r", stdin);
    freopen("cownomics.out", "w", stdout);
	int k,n,i,j,l,m,ans=0;
	cin >> n >> m;
	vector<string>spotty(n),plain(n);
	for(i=0; i<n; i++) {
		cin >> spotty[i];
	}
	for(i=0; i<n; i++) {
		cin >> plain[i];
	}
	for(i=0; i<m; i++) {
		bool a=1;
		for(j=0; j<n; j++) {
			for(l=0; l<n; l++) {
				if(spotty[j][i]==plain[l][i]) {
					a=0;
					break;
				}
			}
			if(a==0) {
				break;
			}
		}
		if(a==1) {
			ans++;
		}
	}

	cout << ans;
	return 0;
}