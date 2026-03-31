#include <bits/stdc++.h>
using namespace std;
int main() {
    int t;
    if(!(cin >> t)) return 0;
    while(t--) {
        int n; cin >> n;
        vector<int>a(n);
        for(int i=0;i<n;i++) cin >> a[i];
        const int INF = 1e9;
        // dp[i][p] where p=0 friend, p=1 you
        vector<array<int,2>> dp(n+5);
        dp[n] = {0,0};
        dp[n+1] = {0,0};
        for(int i = n-1; i >= 0; --i) {
            // friend's turn p=0
            int cost1 = a[i] + dp[i+1][1]; // kill one
            int cost2 = INF;
            if(i+1 < n) cost2 = a[i] + a[i+1] + dp[i+2][1]; // kill two
            dp[i][0] = min(cost1, cost2);
            // your turn p=1
            int my1 = 0 + dp[i+1][0];
            int my2 = INF;
            if(i+1 < n) my2 = 0 + dp[i+2][0];
            dp[i][1] = min(my1, my2);
        }
        cout << dp[0][0] << '\n';
    }
    return 0;
}
