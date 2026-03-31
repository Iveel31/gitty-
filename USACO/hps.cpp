#include <bits/stdc++.h>
using namespace std;

// count wins if Bessie always plays gesture g in games [l, r)
int countWins(const vector<array<int,3>>& pref, char g, int l, int r) {
    int h = pref[r][0] - pref[l][0]; // FJ plays H
    int p = pref[r][1] - pref[l][1]; // FJ plays P
    int s = pref[r][2] - pref[l][2]; // FJ plays S
    if (g == 'H') return s; // Hoof beats Scissors
    if (g == 'P') return h; // Paper beats Hoof
    return p;               // Scissors beats Paper
}

int main() {
    freopen("hps.in", "r", stdin);
    freopen("hps.out", "w", stdout);

    int n;
    cin >> n;

    vector<char> fj(n);
    for (int i = 0; i < n; i++) cin >> fj[i];

    // prefix count: pref[i][0]=#H, pref[i][1]=#P, pref[i][2]=#S up to i
    vector<array<int,3>> pref(n+1);
    for (int i = 0; i < n; i++) {
        pref[i+1] = pref[i];
        if (fj[i] == 'H') pref[i+1][0]++;
        if (fj[i] == 'P') pref[i+1][1]++;
        if (fj[i] == 'S') pref[i+1][2]++;
    }

    vector<char> moves = {'H','P','S'};
    int ans = 0;

    // try every split position
    for (int split = 0; split <= n; split++) {
        for (char first : moves) {
            for (char second : moves) {
                int cur = countWins(pref, first, 0, split)
                        + countWins(pref, second, split, n);
                ans = max(ans, cur);
            }
        }
    }

    cout << ans << "\n";
    return 0;
}
