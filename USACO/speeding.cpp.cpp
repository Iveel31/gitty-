
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
    freopen("speeding.in","r",stdin);
    freopen("speeding.out","w",stdout);

    int n,m,i,maxi=0,first,second,minlength;
    cin >> n>>m;
    vector<pair<int,int>>a,b;
    for(i=0;i<n;i++){
        cin >> first>>second;
        a.push_back({first,second});
    }
    for(i=0;i<m;i++){
        cin >>first>>second;
        b.push_back({first,second});
    }
    int j=0,d=0;
    while (j < n && d < m){
        minlength= min(a[j].first, b[d].first);
         if(a[j].second<=b[d].second){
                maxi=max(maxi,b[d].second-a[j].second);
            }
            a[j].first -= minlength;
            b[d].first -= minlength;
            if(a[j].first==0) j++;
            if(b[d].first==0)d++;
    }
    cout << maxi;
    return 0;
}