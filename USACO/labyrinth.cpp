#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<string> grid;
bool visited[1000][1000];
pair<int,int> parent[1000][1000];
char move_dir[1000][1000];

int dx[4] = {0, 0, -1, 1};
int dy[4] = {-1, 1, 0, 0};
char dir[4] = {'L', 'R', 'U', 'D'};

bool check(int x, int y) {
    return x >= 0 && y >= 0 && x < n && y < m && grid[x][y] != '#';
}

int main() {
    cin >> n >> m;
    grid.resize(n);

    int si, sj, ei, ej;

    for (int i = 0; i < n; i++) {
        cin >> grid[i];
        for (int j = 0; j < m; j++) {
            if (grid[i][j] == 'A') si = i, sj = j;
            if (grid[i][j] == 'B') ei = i, ej = j;
        }
    }

    queue<pair<int,int>> q;
    q.push({si, sj});
    visited[si][sj] = true;

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();

        for (int k = 0; k < 4; k++) {
            int nx = x + dx[k];
            int ny = y + dy[k];

            if (check(nx, ny) && !visited[nx][ny]) {
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
                move_dir[nx][ny] = dir[k];
                q.push({nx, ny});
            }
        }
    }

    if (!visited[ei][ej]) {
        cout << "NO";
        return 0;
    }

    cout << "YES\n";

    vector<char> path;
    int x = ei, y = ej;

    while (x != si || y != sj) {
        path.push_back(move_dir[x][y]);
        auto p = parent[x][y];
        x = p.first;
        y = p.second;
    }

    reverse(path.begin(), path.end());

    cout << path.size() << "\n";
    for (char c : path) cout << c;
}
