#include <bits/stdc++.h>
using namespace std;
const int INF = 1e9;
vector<vector<int>> adj;
vector<bool>visited;
vector<vector<int>>groups;
void dfs(int u,int group){
    groups[group].push_back(u);
    visited[u]=true;
    for(int v : adj[u]){
        if(!visited[v]) dfs(v , group);
    }
}
int main(){
    freopen("fenceplan.in", "r", stdin);
    freopen("fenceplan.out", "w", stdout);
    int n , m;
    cin >> n >> m;
    adj.assign(n , vector<int>());
    visited.assign(n,false);
    groups.assign(n,vector<int>());
    vector<pair<int,int>>location(n);
    int a , b;
    for(int i = 0 ;i < n; i++){
        cin >> a >> b;
        location[i]={a,b};
    }
    for(int i = 0 ;i < m; i++){
        cin >> a >> b;
        --a; --b;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }
    int NumOfGroup=0;
    for(int i = 0 ; i < n; i++){
        if(!visited[i]){
            dfs(i,NumOfGroup);
            NumOfGroup++;
        }
    }
    int ans;
    for(int i = 0 ;i < NumOfGroup; i++){
        int max_X=0, min_X=INF;
        int max_Y=0, min_Y=INF;
        for(int j = 0 ;j < groups[i].size(); j++){
            max_X=max(location[groups[i][j]].first,max_X);
            min_X=min(location[groups[i][j]].first,min_X);
            max_Y=max(location[groups[i][j]].second,max_Y);
            min_Y=min(location[groups[i][j]].second,min_Y);
        }
        if(i == 0){
            ans=((max_X-min_X)+(max_Y-min_Y))*2;
        }else{
            ans=min(ans,((max_X-min_X)+(max_Y-min_Y))*2);
        }
    }
    cout << ans;
    return 0;
}