#include <bits/stdc++.h>
using namespace std;

// ---------- Sparse Table for Range Minimum ----------
struct SparseTableMin {
    vector<vector<int>> table;
    vector<int> logValues;

    SparseTableMin() = default;

    SparseTableMin(const vector<int> &arr) {
        int n = arr.size();
        if (n == 0) return;
        logValues.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i)
            logValues[i] = logValues[i / 2] + 1;

        int levels = logValues[n] + 1;
        table.assign(levels, vector<int>(n));
        table[0] = arr;

        for (int k = 1; k < levels; ++k) {
            int len = 1 << k;
            int half = len / 2;
            for (int i = 0; i + len <= n; ++i) {
                table[k][i] = min(table[k - 1][i], table[k - 1][i + half]);
            }
        }
    }

    int queryMin(int left, int right) const {
        if (left > right)
            return numeric_limits<int>::max();
        int length = right - left + 1;
        int k = logValues[length];
        return min(table[k][left], table[k][right - (1 << k) + 1]);
    }
};

// ---------- Sparse Table for Range Maximum ----------
struct SparseTableMax {
    vector<vector<int>> table;
    vector<int> logValues;

    SparseTableMax() = default;

    SparseTableMax(const vector<int> &arr) {
        int n = arr.size();
        if (n == 0) return;
        logValues.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i)
            logValues[i] = logValues[i / 2] + 1;

        int levels = logValues[n] + 1;
        table.assign(levels, vector<int>(n));
        table[0] = arr;

        for (int k = 1; k < levels; ++k) {
            int len = 1 << k;
            int half = len / 2;
            for (int i = 0; i + len <= n; ++i) {
                table[k][i] = max(table[k - 1][i], table[k - 1][i + half]);
            }
        }
    }

    int queryMax(int left, int right) const {
        if (left > right)
            return numeric_limits<int>::min();
        int length = right - left + 1;
        int k = logValues[length];
        return max(table[k][left], table[k][right - (1 << k) + 1]);
    }
};

// ---------- Hash Function ----------
struct PairHash {
    size_t operator()(long long key) const noexcept {
        unsigned long long z = static_cast<unsigned long long>(key) + 0x9e3779b97f4a7c15ULL;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        return static_cast<size_t>(z);
    }
};

static inline long long packCoords(int x, int y) {
    return (static_cast<long long>(x) << 32) ^ (static_cast<long long>(y) & 0xffffffffLL);
}

// ---------- Main ----------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int gridSize, timeSteps;
    if (!(cin >> gridSize >> timeSteps)) {
        return 0;
    }

    string movement;
    cin >> movement;

    vector<string> grid(gridSize);
    for (int i = 0; i < gridSize; ++i) {
        cin >> grid[i];
    }

    int startX = -1, startY = -1;
    vector<pair<int, int>> rocks;
    rocks.reserve(300);

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            char cell = grid[i][j];
            int x = j + 1;
            int y = gridSize - i;
            if (cell == '@') {
                startX = x;
                startY = y;
            } else if (cell == '#') {
                rocks.emplace_back(x, y);
            }
        }
    }

    if (startX == -1 || startY == -1) {
        cout << 0 << '\n';
        return 0;
    }

    vector<int> prefixX(timeSteps + 1, 0), prefixY(timeSteps + 1, 0);
    for (int i = 1; i <= timeSteps; ++i) {
        char dir = movement[i - 1];
        prefixX[i] = prefixX[i - 1];
        prefixY[i] = prefixY[i - 1];
        if (dir == 'U') ++prefixY[i];
        else if (dir == 'D') --prefixY[i];
        else if (dir == 'R') ++prefixX[i];
        else if (dir == 'L') --prefixX[i];
    }

    SparseTableMin minX(prefixX), minY(prefixY);
    SparseTableMax maxX(prefixX), maxY(prefixY);

    const int minBoundX = 1 - startX;
    const int maxBoundX = gridSize - startX;
    const int minBoundY = 1 - startY;
    const int maxBoundY = gridSize - startY;

    vector<int> boundaryReach(timeSteps + 2, 0);

    auto withinBounds = [&](int l, int r) -> bool {
        int curMinX = minX.queryMin(l, r);
        if (curMinX < prefixX[l - 1] + minBoundX) return false;
        int curMaxX = maxX.queryMax(l, r);
        if (curMaxX > prefixX[l - 1] + maxBoundX) return false;
        int curMinY = minY.queryMin(l, r);
        if (curMinY < prefixY[l - 1] + minBoundY) return false;
        int curMaxY = maxY.queryMax(l, r);
        if (curMaxY > prefixY[l - 1] + maxBoundY) return false;
        return true;
    };

    for (int l = 1; l <= timeSteps; ++l) {
        int low = l, high = timeSteps, best = l - 1;
        while (low <= high) {
            int mid = (low + high) >> 1;
            if (withinBounds(l, mid)) {
                best = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        boundaryReach[l] = best;
    }

    const int INF = timeSteps + 1;
    vector<int> rockReach(timeSteps + 2, INF);

    vector<pair<int, int>> rockOffsets;
    rockOffsets.reserve(rocks.size());
    for (auto [rx, ry] : rocks) {
        rockOffsets.emplace_back(rx - startX, ry - startY);
    }

    unordered_map<long long, int, PairHash> nextPosition;
    nextPosition.reserve((timeSteps + 1) * 2u);

    for (int i = timeSteps; i >= 0; --i) {
        int nearest = INF;
        for (auto [dx, dy] : rockOffsets) {
            long long key = packCoords(prefixX[i] + dx, prefixY[i] + dy);
            auto it = nextPosition.find(key);
            if (it != nextPosition.end()) {
                nearest = min(nearest, it->second);
            }
        }
        if (i < timeSteps) {
            rockReach[i + 1] = min(rockReach[i + 1], nearest);
        }
        long long currKey = packCoords(prefixX[i], prefixY[i]);
        auto it = nextPosition.find(currKey);
        if (it == nextPosition.end() || i < it->second) {
            nextPosition[currKey] = i;
        }
    }

    long long totalSafeMoves = 0;
    for (int l = 1; l <= timeSteps; ++l) {
        int obstacleBound = rockReach[l];
        if (obstacleBound == INF)
            obstacleBound = timeSteps + 1;
        int maxRight = min(boundaryReach[l], obstacleBound - 1);
        if (maxRight >= l) {
            totalSafeMoves += (long long)(maxRight - l + 1);
        }
    }

    cout << totalSafeMoves << '\n';
    return 0;
}
