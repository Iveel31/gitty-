#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >> n;
    while(n--){
        int t;
        cin >> t;
        vector<int>a(t);
        for(int i = 0 ;i < t; i++) {
            cin >> a[i];
        }
        bool isOk=1;
        sort(a.begin(),a.end());
        for(int i = 1 ; i < t; i++){
            if(a[i-1]==a[i]){
                isOk=0;
                break;
            }
        }
        if(isOk) cout << "YES";
        else cout <<"NO";
        cout << endl;
    }

    return 0;
}