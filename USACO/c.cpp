#include <bits/stdc++.h>
using namespace std;
const int MOD = 998244353;

long long mod_pow(long long a, long long b) {
    long long res = 1;
    while (b > 0) {
        if (b & 1) res = res * a % MOD;
        a = a * a % MOD;
        b >>= 1;
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<vector<int>> a(n, vector<int>(m));
    unordered_map<int, vector<pair<int, int>>> groups;
    groups.reserve(static_cast<size_t>(n) * m * 2 + 1);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int val;
            cin >> val;
            a[i][j] = val;
            groups[val].push_back({i, j});
        }
    }

    vector<int> uniq;
    uniq.reserve(groups.size());
    for (const auto &kv : groups) uniq.push_back(kv.first);
    sort(uniq.begin(), uniq.end());
    int k = static_cast<int>(uniq.size());

    vector<pair<int, int>> repeated_cells;
    bool has_repeat = false;
    bool multiple_repeats = false;
    for (const auto &kv : groups) {
        if (kv.second.size() > 1) {
            if (has_repeat) {
                multiple_repeats = true;
                break;
            }
            has_repeat = true;
            repeated_cells = kv.second;
        }
    }
    bool use_special = has_repeat && !multiple_repeats;

    if (k <= 20) {
        long long state_limit = 1LL << k;
        if (1LL * n * m * state_limit <= 100000000LL) {
            unordered_map<int, int> index;
            index.reserve(k * 2 + 1);
            for (int i = 0; i < k; ++i) index[uniq[i]] = i;

            vector<vector<int>> comp(n, vector<int>(m));
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    comp[i][j] = index[a[i][j]];
                }
            }

            int mask_cnt = 1 << k;
            vector<int> pop(mask_cnt, 0);
            for (int mask = 0; mask < mask_cnt; ++mask) pop[mask] = __builtin_popcount(mask);

            vector<vector<int>> dp(m, vector<int>(mask_cnt, 0));
            for (int i = 0; i < n; ++i) {
                vector<int> from_left(mask_cnt, 0);
                for (int j = 0; j < m; ++j) {
                    vector<int> cur(mask_cnt, 0);
                    for (int mask = 0; mask < mask_cnt; ++mask) {
                        int val = dp[j][mask];
                        if (val) cur[mask] = val;
                    }
                    for (int mask = 0; mask < mask_cnt; ++mask) {
                        int val = from_left[mask];
                        if (!val) continue;
                        int acc = cur[mask] + val;
                        if (acc >= MOD) acc -= MOD;
                        cur[mask] = acc;
                    }
                    if (i == 0 && j == 0) {
                        cur[0] += 1;
                        if (cur[0] >= MOD) cur[0] -= MOD;
                    }

                    vector<int> next(mask_cnt, 0);
                    int bit = 1 << comp[i][j];
                    for (int mask = 0; mask < mask_cnt; ++mask) {
                        int ways = cur[mask];
                        if (!ways) continue;
                        int new_mask = mask | bit;
                        int acc = next[new_mask] + ways;
                        if (acc >= MOD) acc -= MOD;
                        next[new_mask] = acc;
                    }

                    dp[j] = next;
                    from_left = next;
                }
            }

            long long ans = 0;
            for (int mask = 0; mask < mask_cnt; ++mask) {
                ans = (ans + 1LL * dp[m - 1][mask] * pop[mask]) % MOD;
            }
            cout << ans << '\n';
            return 0;
        }
    }

    int limit = n + m + 5;
    vector<long long> fact(limit, 1), inv_fact(limit, 1);
    for (int i = 1; i < limit; ++i) fact[i] = fact[i - 1] * i % MOD;
    inv_fact.back() = mod_pow(fact.back(), MOD - 2);
    for (int i = limit - 2; i >= 0; --i) inv_fact[i] = inv_fact[i + 1] * (i + 1) % MOD;

    auto comb = [&](int nn, int kk) -> long long {
        if (kk < 0 || kk > nn) return 0;
        return fact[nn] * inv_fact[kk] % MOD * inv_fact[nn - kk] % MOD;
    };

    if (use_special) {
        vector<vector<char>> is_repeat(n, vector<char>(m, 0));
        for (const auto &cell : repeated_cells) {
            is_repeat[cell.first][cell.second] = 1;
        }

        vector<vector<long long>> dp_no(n, vector<long long>(m, 0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (is_repeat[i][j]) continue;
                if (i == 0 && j == 0) {
                    dp_no[i][j] = 1;
                    continue;
                }
                long long val = 0;
                if (i) val += dp_no[i - 1][j];
                if (j) val += dp_no[i][j - 1];
                dp_no[i][j] = val % MOD;
            }
        }

        long long total_paths = comb(n + m - 2, n - 1);
        long long paths_without = dp_no[n - 1][m - 1] % MOD;
        long long paths_with = (total_paths - paths_without) % MOD;
        if (paths_with < 0) paths_with += MOD;

        long long sum_cnt = 0;
        for (const auto &cell : repeated_cells) {
            int i = cell.first;
            int j = cell.second;
            long long prefix = comb(i + j, i);
            long long suffix = comb((n - 1 - i) + (m - 1 - j), n - 1 - i);
            sum_cnt = (sum_cnt + prefix * suffix) % MOD;
        }

        long long len = n + m - 1;
        long long ans = total_paths * len % MOD;
        ans = (ans - sum_cnt) % MOD;
        ans = (ans + paths_with) % MOD;
        if (ans < 0) ans += MOD;
        cout << ans << '\n';
        return 0;
    }

    long long ans = 0;
    vector<long long> first_paths;

    for (auto &entry : groups) {
        auto &cells = entry.second;
        sort(cells.begin(), cells.end(), [](const pair<int, int> &lhs, const pair<int, int> &rhs) {
            int dl = lhs.first + lhs.second;
            int dr = rhs.first + rhs.second;
            if (dl != dr) return dl < dr;
            if (lhs.first != rhs.first) return lhs.first < rhs.first;
            return lhs.second < rhs.second;
        });

        first_paths.assign(cells.size(), 0);
        for (size_t idx = 0; idx < cells.size(); ++idx) {
            int i = cells[idx].first;
            int j = cells[idx].second;
            long long total = comb(i + j, i);
            long long subtract = 0;
            for (size_t prev = 0; prev < idx; ++prev) {
                int x = cells[prev].first;
                int y = cells[prev].second;
                int dx = i - x;
                int dy = j - y;
                if (dx < 0 || dy < 0) continue;
                long long ways_mid = comb(dx + dy, dx);
                subtract = (subtract + first_paths[prev] * ways_mid) % MOD;
            }
            long long first_here = (total - subtract) % MOD;
            if (first_here < 0) first_here += MOD;
            first_paths[idx] = first_here;

            int down = (n - 1) - i;
            int right = (m - 1) - j;
            long long tail = comb(down + right, down);
            ans = (ans + first_here * tail) % MOD;
        }
    }

    cout << ans % MOD << '\n';
    return 0;
}
