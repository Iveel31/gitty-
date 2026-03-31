#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> a;
vector<bool> visited;

void dfs(int v) {
    visited[v] = true;
    for (int u : a[v]) {
        if (!visited[u]) {
            dfs(u);
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;

    a.resize(n);
    visited.assign(n, false);

    for (int i = 0; i < m; i++) {
        int c, b;
        cin >> c >> b;
        c--; b--;               
        a[c].push_back(b);
        a[b].push_back(c);
    }

    vector<int> city_reps;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            city_reps.push_back(i);
            dfs(i);
        }
    }

    cout << city_reps.size() - 1 << endl;
    for (int i = 0; i + 1 < city_reps.size(); i++) {
        cout << city_reps[i] + 1 << " " << city_reps[i + 1] + 1 << endl;
    }

    return 0;
}
