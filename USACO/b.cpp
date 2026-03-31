#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) return 0;

    vector<vector<int>> rank(N);
    for (int i = 1; i < N; ++i) {
        rank[i].resize(i);
        for (int j = 0; j < i; ++j) cin >> rank[i][j];
    }

    vector<int> lineup;
    lineup.reserve(N);
    lineup.push_back(0);
    vector<char> good(N, 0);

    for (int student = 1; student < N; ++student) {
        fill(good.begin(), good.begin() + student, 0);
        int half = (student + 1) / 2;
        for (int j = 0; j < half; ++j) {
            good[rank[student][j]] = 1;
        }

        bool placed = false;
        if (good[lineup.front()]) {
            lineup.insert(lineup.begin(), student);
            placed = true;
        } else if (good[lineup.back()]) {
            lineup.push_back(student);
            placed = true;
        } else {
            for (size_t pos = 0; pos + 1 < lineup.size(); ++pos) {
                int left = lineup[pos];
                int right = lineup[pos + 1];
                if (good[left] && good[right]) {
                    lineup.insert(lineup.begin() + static_cast<long long>(pos) + 1, student);
                    placed = true;
                    break;
                }
            }
        }

        if (!placed) {
            lineup.push_back(student);
        }
    }

    for (int i = 0; i < N; ++i) {
        cout << lineup[i] << (i + 1 == N ? '\n' : ' ');
    }
    return 0;
}