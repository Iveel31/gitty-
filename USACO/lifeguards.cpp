#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
 vector<int>times(1000);
int main() {
    freopen("lifeguards.in", "r", stdin);
    freopen("lifeguards.out", "w", stdout);
	int n,i,maxi=0,j;
	cin >> n;
	vector<int> start(n),end(n);
	for(i=0; i<n; i++) {
		cin >> start[i] >> end[i];
		for(j=start[i];j<end[i];j++){
		    times[j]++;
		}
	}
	for(i=0; i<n; i++) {
        for(j=start[i];j<end[i];j++){
           times[j]--;
        }
        int count=0;
        for(j=0;j<times.size();j++){
            if(times[j]>0) count++;
        }
        maxi=max(maxi,count);
        for(j=start[i];j<end[i];j++){
            times[j]++;
        }
	}
	cout << maxi;
	return 0;
}
