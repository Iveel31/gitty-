#include<bits/stdc++.h>
#define int long long
using namespace std;
signed main(){
    int n;
    cin >> n;
    vector<pair<int,int>> tasks(n);
    for(int i=0;i<n;i++){
        cin >> tasks[i].first >> tasks[i].second;
    }
    sort(tasks.begin(),tasks.end());
    int sum=0;
    int maxi_reward=0;
    for(int i = 0 ;i < n; i++){
        sum+=tasks[i].first;
        maxi_reward += tasks[i].second - sum;
    }
    cout << maxi_reward << endl;
    return 0;
}