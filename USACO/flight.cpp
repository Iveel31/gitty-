#include <bits/stdc++.h>
using namespace std;
vector<vector<int>> adj, radj; 
vector<bool> visited;
void dfs(int v , const vector<vector<int>>& graph){
    visited[v]=true;
    for(int u : graph[v]) {
        if(!visited[u]) dfs(u , graph);
    }
}
int main(){
    int n , m;
    cin >> n >> m;
    adj.assign(n,vector<int>());
    radj.assign(n,vector<int>());
    for(int i = 0 ;i < m; i++){
        int a ,b ;
        cin >> a >> b;
        --a; --b;
        adj[a].push_back(b);
        radj[b].push_back(a);
    }
    visited.assign(n,false);
    dfs(0 , adj);
    int unreachable=-1;
    for(int i = 0 ;i < n; i++){
        if(!visited[i]){
            unreachable=i;
            break;
        }
    }
    if(unreachable != -1){
        cout << "NO" << endl << 1 << " " << unreachable+1;
        return 0;
    }
    visited.assign(n,false);
    dfs(0 , radj);
    unreachable=-1;
    for(int i = 0 ;i < n; i++){
        if(!visited[i]){
            unreachable=i;
            break;
        }
    } if(unreachable != -1){
        cout << "NO" << endl << unreachable+1 << " " << 1;
        return 0;
    }
    cout << "YES";
    return 0;
}