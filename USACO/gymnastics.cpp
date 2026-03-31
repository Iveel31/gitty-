#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
	freopen("gymnastics.in", "r", stdin);
    freopen("gymnastics.out", "w", stdout);
	int k,n,i,j,l;
	cin >> k >> n;
	int a[20][20],b[20][20];
	for(i=0; i<k; i++) {
		for(j=0; j<n; j++) {
			cin >> a[i][j];
			a[i][j]--;
		}
	}
	for(i=0; i<n; i++) {
		for(j=0; j<n; j++) {
			b[i][j]=0;
		}
	}
	for(i=0; i<k; i++) {
		for(j=0; j<n; j++) {
			for(l=j+1; l<n; l++) {
				b[a[i][j]][a[i][l]]++;
				// cout << b[a[i][j]][a[i][l]]<<" ";
			}
		}
	}
	int count=0;
	for(i=0; i<n; i++) {
		for(j=0; j<n; j++) {
			if(b[i][j]==k) {
				count++;
			}
		}
	}
	cout << count;
	return 0;
}