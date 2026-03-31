#include <bits/stdc++.h>
using namespace std;

vector<int> adj[3005];
bool open_barn[3005];
bool visited[3005];

void dfs(int v){
    visited[v] = true;
    for(int u : adj[v]){
        if(open_barn[u] && !visited[u]){
            dfs(u);
        }
    }
}

int main(){
    freopen("closing.in", "r", stdin);
    freopen("closing.out", "w", stdout);
    int N, M;
    cin >> N >> M;

    for(int i = 0; i < M; i++){
        int a, b;
        cin >> a >> b;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    vector<int> order(N);
    for(int i = 0; i < N; i++){
        cin >> order[i];
    }

    for(int i = 1; i <= N; i++) open_barn[i] = true;

    vector<string> ans;

    for(int step = 0; step < N; step++){
        memset(visited, 0, sizeof(visited));
        int start = -1;
        for(int i = 1; i <= N; i++){
            if(open_barn[i]){
                start = i;
                break;
            }
        }

        if(start == -1){
            ans.push_back("YES"); 
        } else {
            dfs(start);

            bool ok = true;
            for(int i = 1; i <= N; i++){
                if(open_barn[i] && !visited[i]){ 
                    ok = false; 
                    break;
                }
            }

            ans.push_back(ok ? "YES" : "NO");
        }

        // Одоо дараагийн саравчийг хаана
        open_barn[order[step]] = false;
    }

    // Хариуг хэвлэнэ
    for(string &s : ans){
        cout << s << "\n";
    }
}
  