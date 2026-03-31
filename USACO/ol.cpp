#include <bits/stdc++.h>
using namespace std;
const int MOD = 998244353;

struct VectorHash {
    size_t operator()(const vector<int>& v) const noexcept {
        size_t seed = v.size();
        for (int x : v) {
            seed ^= (static_cast<size_t>(x) + 0x9e3779b97f4a7c15ULL) + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

inline bool cmpByVar(int x, int y) {
    int ax = abs(x), ay = abs(y);
    if (ax != ay) return ax < ay;
    return x < y;
}

bool mergeAssignments(const vector<int>& a, const vector<int>& b, vector<int>& out) {
    out.clear();
    out.reserve(a.size() + b.size());
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        int x = a[i], y = b[j];
        int ax = abs(x), ay = abs(y);
        if (ax == ay) {
            if (x != y) return false;
            out.push_back(x);
            ++i;
            ++j;
        } else if (ax < ay || (ax == ay && x < y)) {
            out.push_back(x);
            ++i;
        } else {
            out.push_back(y);
            ++j;
        }
    }
    while (i < a.size()) out.push_back(a[i++]);
    while (j < b.size()) out.push_back(b[j++]);
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int p, t;
    if (!(cin >> p >> t)) return 0;

    vector<long long> pow2(p + 1, 1);
    for (int i = 1; i <= p; ++i) {
        pow2[i] = (pow2[i - 1] * 2) % MOD;
    }

    vector<vector<int>> constraints;
    constraints.reserve(t);

    for (int i = 0; i < t; ++i) {
        int a;
        cin >> a;
        unordered_map<int, int> seen;
        seen.reserve(a * 2 + 1);
        bool tautology = false;
        for (int j = 0; j < a; ++j) {
            int r;
            cin >> r;
            int idx = abs(r);
            int sign = (r > 0) ? 1 : -1;
            auto it = seen.find(idx);
            if (it == seen.end()) {
                seen[idx] = sign;
            } else if (it->second != sign) {
                tautology = true;
            }
        }
        if (tautology) continue; // this friend always happy
        if (seen.empty()) {
            cout << 0 << "\n";
            return 0;
        }
        vector<int> key;
        key.reserve(seen.size());
        for (auto &kv : seen) {
            int idx = kv.first;
            int sign = kv.second;
            key.push_back(sign == 1 ? -idx : idx);
        }
        sort(key.begin(), key.end(), cmpByVar);
        constraints.push_back(std::move(key));
    }

    unordered_map<vector<int>, int, VectorHash> dp;
    dp.reserve(constraints.size() * 2 + 1);
    vector<int> merged;
    merged.reserve(p);

    for (const auto& constr : constraints) {
        unordered_map<vector<int>, int, VectorHash> delta;
        delta.reserve(dp.size() + 1);
        delta[constr] = (delta[constr] + 1) % MOD;

        for (const auto& entry : dp) {
            if (!entry.second) continue;
            if (mergeAssignments(entry.first, constr, merged)) {
                int add = (MOD - entry.second) % MOD;
                auto it = delta.find(merged);
                if (it == delta.end()) {
                    delta.emplace(merged, add);
                } else {
                    int nv = it->second + add;
                    if (nv >= MOD) nv -= MOD;
                    it->second = nv;
                }
            }
        }

        for (auto &kv : delta) {
            int val = kv.second % MOD;
            if (val < 0) val += MOD;
            if (val == 0) {
                auto it = dp.find(kv.first);
                if (it != dp.end() && it->second == 0) dp.erase(it);
                continue;
            }
            auto it = dp.find(kv.first);
            if (it == dp.end()) {
                dp.emplace(kv.first, val);
            } else {
                int nv = it->second + val;
                if (nv >= MOD) nv -= MOD;
                it->second = nv;
                if (it->second == 0) dp.erase(it);
            }
        }
    }

    long long bad = 0;
    for (const auto& entry : dp) {
        if (!entry.second) continue;
        int forced = static_cast<int>(entry.first.size());
        if (forced > p) continue;
        long long cnt = pow2[p - forced];
        bad = (bad + 1LL * entry.second * cnt) % MOD;
    }

    long long total = pow2[p];
    long long res = (total - bad + MOD) % MOD;
    cout << res << "\n";
    return 0;
}
