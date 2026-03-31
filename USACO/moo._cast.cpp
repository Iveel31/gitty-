#include <bits/stdc++.h>
using namespace std;
int main(){
    int t;
    int s=1;
    cin >> t;
    for(int i = 0 ;i < t ; i++){
        int n;
        cin >> n;
        vector<int>a(n);
        for(int i = 0 ;i < n; i++){
            cin >> a[i];
        }
        if(n==5){ cout << a[0]*a[1]*a[2]*a[3]*a[4]; return 0;}
        sort(a.begin(),a.end());
        int k=a[0]*a[1]*a[n-1]*a[n-2]*a[n-3];
        int l=a[0]*a[1]*a[2]*a[3]*a[n-1];
        int h=a[n-1]*a[n-2]*a[n-3]*a[n-4]*a[n-5];
        int g=a[0]*a[1]*a[2]*a[3]*a[4];
        cout << max(k,max(h,max(l,g)));
        cout << endl;

    }
    cout << s;
    return 0;
}