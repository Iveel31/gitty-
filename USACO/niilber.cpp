#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >> n;
    vector<int>a(n);
    for(int i = 0 ;i < n; i++){
        cin >> a[i];
    }
    int r , l;
    int count=1;
    vector<int>b(n);
    b=a;
    sort(b.begin(),b.end());
    bool isOk=1;
    for(int i = 0 ;i < n-1; i++){
        if(a[i]>a[i+1]){ l=i; break;}
    }
    for(int i = n-1 ;i > 1; i--){
        if(a[i]<a[i-1]){ r=i; break;}

    }
    cout << r-l+1;
    
    return 0;
}