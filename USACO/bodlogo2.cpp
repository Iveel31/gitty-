#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >> n;
    while(n--){
        int t;
        cin >> t;
        vector<int>a(t);
        int count=0;
        int maxi=0;
        for(int i = 0 ; i < t; i++){
            cin >> a[i]; 
            if(a[i]==0) count++;   
            else count=0;
            maxi=max(count,maxi);
        }
        cout << maxi << endl;
    }

    return 0;
}