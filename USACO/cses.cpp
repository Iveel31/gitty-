#include <bits/stdc++.h>
using namespace std;

const long long INF = 1e18;

int main() {

    int n, m;
    cin >> n >> m;

    vector<vector<pair<int,int>>> adj(n);

    for(int i = 0; i < m; i++){
        int a,b,c;
        cin >> a >> b >> c;
        --a; --b;
        adj[a].push_back({b,c});
    }

    vector<long long> dist(n, INF);
    dist[0] = 0;

    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;

    pq.push({0,0});

    while(!pq.empty()){

        auto top = pq.top();
        pq.pop();

        long long d = top.first;
        int u = top.second;

        if(d > dist[u]) continue;

        for(auto &p : adj[u]){

            int v = p.first;
            int w = p.second;

            if(dist[u] + w < dist[v]){
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    for(int i=0;i<n;i++){
        cout << dist[i] << " ";
    }

}