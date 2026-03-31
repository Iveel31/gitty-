#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main() {
    int n,start,end,bucket,d,i;
    cin >> n;
    vector<pair<int,int>> a(n);
    for(i=0;i<n;i++){
        cin >>start>>end>>bucket;
        a.push_back({start,bucket});
        a.push_back({end,-bucket});
    }
    sort(a.begin(),a.end());
    int maxi=0,current=0;
    for(i=0;i<a.size();i++){
        current+=a[i].second;
        maxi=max(maxi,current);
    }
    cout << maxi;
    return 0;
}
