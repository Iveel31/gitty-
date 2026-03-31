#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<vector<char>> a;
bool visited[1000][1000];

void check(int i, int j) {
    if (i < 0 || i >= n || j < 0 || j >= m) return;
    if (a[i][j] == '#') return;
    if (visited[i][j]) return;
    visited[i][j] = true;
    check(i - 1, j);
    check(i + 1, j);
    check(i, j - 1);
    check(i, j + 1);
}

int main() {
    cin >> n >> m;

    a.assign(n, vector<char>(m));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
        }
    }

    int count = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {

            if (a[i][j] == 'A' && !visited[i][j]) {
                check(i, j);
            }

        }
    }

    cout << count;
    return 0;
}
