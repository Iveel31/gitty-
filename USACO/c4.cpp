#include <bits/stdc++.h>
using namespace std;

struct Tank {
    int id;
    long long x, y;
    char d;
};
bool collideRL(const Tank& R, const Tank& L) {
    if (R.d!='R' || L.d!='L') return false;
    if (R.y != L.y) return false;
    if (R.x >= L.x) return false;
    long long dx = L.x - R.x;
    return dx % 2 == 0; // яг нэг мөчид уулзах
}

bool collideUD(const Tank& U, const Tank& D) {
    if (U.d!='U' || D.d!='D') return false;
    if (U.x != D.x) return false;
    if (U.y >= D.y) return false;
    long long dy = D.y - U.y;
    return dy % 2 == 0;
}

bool collideRU(const Tank& R, const Tank& U) {
    if (R.d!='R' || U.d!='U') return false;
    if (R.x + R.y != U.x + U.y) return false;
    long long t = U.x - R.x;
    return t >= 0;
}

bool collideRD(const Tank& R, const Tank& D) {
    if (R.d!='R' || D.d!='D') return false;
    if (R.x - R.y != D.x - D.y) return false;
    long long t = D.x - R.x;
    return t >= 0;
}

bool collideUL(const Tank& U, const Tank& L) {
    if (U.d!='U' || L.d!='L') return false;
    if (U.x - U.y != L.x - L.y) return false;
    long long t = L.y - U.y;
    return t >= 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int N;
    if (!(cin >> N)) return 0;
    vector<Tank> a(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i].x >> a[i].y >> a[i].d;
        a[i].id = i;
    }
    if (N == 2) {
        Tank A = a[0], B = a[1];
        bool destroyed =
            collideRL(A,B) || collideRL(B,A) ||
            collideUD(A,B) || collideUD(B,A) ||
            collideRU(A,B) || collideRU(B,A) ||
            collideRD(A,B) || collideRD(B,A) ||
            collideUL(A,B) || collideUL(B,A);

        if (!destroyed) {
            cout << A.id+1 << "\n" << B.id+1 << "\n";
        }
    }
    else{
    const long long INF = (1LL<<62);
    vector<long long> tmin(N, INF);

    // Helpers to update minimum collision time for two tanks
    auto collide = [&](int i, int j, long long t){
        if (t < 0) return; // safety, though construction should ensure non-negative
        tmin[i] = min(tmin[i], t);
        tmin[j] = min(tmin[j], t);
    };

    // Group by y for horizontal collisions L vs R
    {
        unordered_map<long long, vector<int>> rows;
        rows.reserve(N*2);
        for (int i = 0; i < N; ++i) {
            if (a[i].d == 'L' || a[i].d == 'R') rows[a[i].y].push_back(i);
        }
        for (auto &kv : rows) {
            auto &v = kv.second;
            sort(v.begin(), v.end(), [&](int i, int j){
                if (a[i].x != a[j].x) return a[i].x < a[j].x;
                return a[i].id < a[j].id;
            });
            vector<int> st; st.reserve(v.size());
            for (int idx : v) {
                if (a[idx].d == 'R') {
                    st.push_back(idx);
                } else { // L
                    if (!st.empty()) {
                        int j = st.back(); st.pop_back();
                        long long t = (a[idx].x - a[j].x) / 2;
                        collide(idx, j, t);
                    }
                }
            }
        }
    }

    // Group by x for vertical collisions U vs D
    {
        unordered_map<long long, vector<int>> cols;
        cols.reserve(N*2);
        for (int i = 0; i < N; ++i) {
            if (a[i].d == 'U' || a[i].d == 'D') cols[a[i].x].push_back(i);
        }
        for (auto &kv : cols) {
            auto &v = kv.second;
            sort(v.begin(), v.end(), [&](int i, int j){
                if (a[i].y != a[j].y) return a[i].y < a[j].y;
                return a[i].id < a[j].id;
            });
            vector<int> st; st.reserve(v.size());
            for (int idx : v) {
                if (a[idx].d == 'U') {
                    st.push_back(idx);
                } else { // D
                    if (!st.empty()) {
                        int j = st.back(); st.pop_back();
                        long long t = (a[idx].y - a[j].y) / 2;
                        collide(idx, j, t);
                    }
                }
            }
        }
    }

    // Group by S = x + y for perpendicular collisions
    auto process_diag = [&](char A, char B, bool stackOnA){
        // stackOnA: true means push A, match when seeing B; false means push B, match when seeing A
        unordered_map<long long, vector<int>> diag;
        diag.reserve(N*2);
        for (int i = 0; i < N; ++i) {
            if ((a[i].d == A) || (a[i].d == B)) {
                long long S = a[i].x + a[i].y;
                diag[S].push_back(i);
            }
        }
        for (auto &kv : diag) {
            auto &v = kv.second;
            sort(v.begin(), v.end(), [&](int i, int j){
                if (a[i].x != a[j].x) return a[i].x < a[j].x;
                return a[i].id < a[j].id;
            });
            vector<int> st; st.reserve(v.size());
            for (int idx : v) {
                if (stackOnA ? (a[idx].d == A) : (a[idx].d == B)) {
                    st.push_back(idx);
                } else {
                    if (!st.empty()) {
                        int j = st.back(); st.pop_back();
                        // Compute t based on pair types
                        long long t = 0;
                        char da = stackOnA ? A : B;
                        char db = stackOnA ? B : A;
                        int iA = (a[j].d == da) ? j : idx;
                        int iB = (a[j].d == db) ? j : idx;
                        // Cases:
                        // R-U: t = x_U - x_R
                        // R-D: t = x_D - x_R
                        // L-U: t = x_L - x_U
                        // L-D: t = x_L - x_D
                        if (a[iA].d == 'R' && a[iB].d == 'U') t = a[iB].x - a[iA].x;
                        else if (a[iA].d == 'R' && a[iB].d == 'D') t = a[iB].x - a[iA].x;
                        else if (a[iA].d == 'L' && a[iB].d == 'U') t = a[iA].x - a[iB].x;
                        else if (a[iA].d == 'L' && a[iB].d == 'D') t = a[iA].x - a[iB].x;
                        else continue; // shouldn't happen
                        if (t >= 0) collide(iA, iB, t);
                    }
                }
            }
        }
    };

    // R-U, R-D, L-U, L-D
    process_diag('R','U', true);
    process_diag('R','D', true);
    process_diag('L','U', false); // push U, match on L
    process_diag('L','D', false); // push D, match on L

    // Output survivors in input order
    for (int i = 0; i < N; ++i) {
        if (tmin[i] == INF) {
            cout << (i+1) << '\n';
        }
    }
}
    return 0;
}
