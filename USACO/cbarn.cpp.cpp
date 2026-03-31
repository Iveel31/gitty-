#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;
int main(){
//    freopen("cbarn.in", "r", stdin);
//    freopen("cbarn.out", "w", stdout);
    int n,i,j,walks=0,minimum=1000000000;
    cin >>n;
    int s=n;
    vector<int>a(1000);
    for(i=0;i<n;i++){
        cin>>a[i];
    }
    for(i=0;i<n;i++){
        for( j=1;j<n;j++){
            walks+=a[j]*j;
            cout << walks << " " ;
        }
        cout << endl;
        minimum=min(minimum,walks);
        int t=a[0];
        for(j=1;j<n;j++){
            a[j-1]=a[j];
        }
        a[n-1]=t;
        walks=0;
    }
    cout << minimum;
    return 0;
}