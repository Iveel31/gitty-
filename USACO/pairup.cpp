#include<bits/stdc++.h>
using namespace std;
int main(){
	freopen("pairup.in", "r", stdin);
     freopen("pairup.out", "w", stdout);
	int n;
	cin >> n;
	vector<pair<int,int>>a(n);
	for(int i = 0 ; i < n; i++){
		cin >> a[i].second >> a[i].first;
	}
	sort(a.begin(),a.end());
	int i = 0, j=n-1,maxi=0;
	while(i<j){
		maxi=max(a[i].first+a[j].first,maxi);
		if(a[i].second==1) i++;
		else a[i].second--;
		if(a[j].second==1) j--;
		else a[j].second--;
	}
	cout << maxi;
	return 0;
}