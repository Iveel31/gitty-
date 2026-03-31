#include <bits/stdc++.h>
using namespace std;

using i64 = long long;
const int INF = 1e9+7;

// Энгийн sieve: 31623 хүртэлх анхны тоонууд
vector<int> build_primes(int limit = 31623) {
    vector<bool> is_prime(limit+1, true);
    is_prime[0] = is_prime[1] = false;
    for (int p = 2; p * p <= limit; ++p) {
        if (is_prime[p]) {
            for (int x = p * p; x <= limit; x += p) is_prime[x] = false;
        }
    }
    vector<int> primes;
    for (int p = 2; p <= limit; ++p) if (is_prime[p]) primes.push_back(p);
    return primes;
}

// ai-ийн квадратгүй үлдэгдэл (squarefree kernel) тооцоолох
i64 squarefree_kernel(i64 x, const vector<int>& primes) {
    i64 res = 1;
    i64 t = x;
    for (int p : primes) {
        if (1LL * p * p > t) break;
        if (t % p == 0) {
            int cnt = 0;
            while (t % p == 0) {
                t /= p;
                cnt++;
            }
            if (cnt & 1) res *= p; // зөвхөн сондгой зэрэгтэйг үлдээнэ
        }
    }
    // Хэрвээ үлдсэн нь >1 бол анхны тоо (эсвэл анхныны үлдэгдэл), зэрэг нь 1
    if (t > 1) res *= t;
    return res;
}

// Range-Min сегмент мод
struct SegTree {
    int n;
    vector<int> st;
    SegTree() {}
    SegTree(const vector<int>& a) { build(a); }
    void build(const vector<int>& a) {
        n = (int)a.size();
        st.assign(4*n, INF);
        build(1, 0, n-1, a);
    }
    void build(int node, int l, int r, const vector<int>& a) {
        if (l == r) { st[node] = a[l]; return; }
        int m = (l + r) >> 1;
        build(node<<1, l, m, a);
        build(node<<1|1, m+1, r, a);
        st[node] = min(st[node<<1], st[node<<1|1]);
    }
    int query(int L, int R) { return query(1, 0, n-1, L, R); }
    int query(int node, int l, int r, int L, int R) {
        if (R < l || r < L) return INF;
        if (L <= l && r <= R) return st[node];
        int m = (l + r) >> 1;
        return min(query(node<<1, l, m, L, R),
                   query(node<<1|1, m+1, r, L, R));
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T; 
    if (!(cin >> T)) return 0;
    auto primes = build_primes();

    while (T--) {
        int N; cin >> N;
        vector<i64> a(N);
        for (int i = 0; i < N; ++i) cin >> a[i];

        // b_i = squarefree kernel
        vector<i64> b(N);
        for (int i = 0; i < N; ++i) {
            b[i] = squarefree_kernel(a[i], primes);
        }

        // next[i] = i-ээс хойш b[i]-тэй тэнцүү хамгийн бага индекс, байхгүй бол +INF
        vector<int> nextIdx(N, INF);
        unordered_map<i64, int> last; 
        last.reserve(N * 2);
        last.max_load_factor(0.7f);

        for (int i = N-1; i >= 0; --i) {
            auto it = last.find(b[i]);
            if (it != last.end()) nextIdx[i] = it->second;
            last[b[i]] = i;
        }

        // Сегмент мод барих
        SegTree st(nextIdx);

        int Q; cin >> Q;
        while (Q--) {
            int L, R; cin >> L >> R;
            // Оролт 1-ээс эхэлсэн тул 0-индекс рүү
            --L; --R;
            int mnNext = st.query(L, R);
            if (mnNext <= R) cout << "YES\n";
            else cout << "NO\n";
        }
    }
    return 0;
}
