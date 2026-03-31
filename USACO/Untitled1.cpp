#include <bits/stdc++.h>
#define int long long
using namespace std;
signed main()
{
    int n;
    cin >> n;
    vector<int>dice(n+1, 0);
    dice[0]=1;
    for(int i = n ; i >= 1; i--){
        for(int j = 1 ; j <= 6 ; j++){
            if(i-j>=0){
                dice[i]+=dice[i-j];
            }
        }
    }
    cout << dice[n];
    return 0;
}