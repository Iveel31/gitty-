#include <bits/stdc++.h>
#define int long long
using namespace std;

vector<vector<int>> adj;
vector<bool> visited;

void dfs(int v){
    visited[v] = true;
    for(int u : adj[v]){
        if(!visited[u]){
            dfs(u);
        }
    }
}

bool countDFS(int v){
    return adj[v].size() >= 2;
}

signed main(){
    int p , n;

    while(cin >> p >> n){
        if(p == 0 && n == 0) return 0;

        int a , b;

        adj.assign(p, vector<int>());
        visited.assign(p,false);

        for(int i = 0 ; i < n; i++){
            cin >> a >> b;
            adj[a].push_back(b);
            adj[b].push_back(a);
        }
        dfs(0);
        bool isSeparated = false;
        for(int i = 0 ; i < p; i++){
            if(!visited[i]){ 
                isSeparated = true; 
                break;
            }
        }
        bool isTrue = false;

        if(isSeparated){
            cout << "Yes";
        }
        else{
            for(int i = 0 ; i < p; i++){
                if(!countDFS(i)){
                    isTrue = true;
                    break;
                }
            }
            if(isTrue) cout << "Yes";
            else cout << "No";
        }
        cout << "\n";
    }

    return 0;
}