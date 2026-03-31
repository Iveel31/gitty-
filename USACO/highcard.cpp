#include <bits/stdc++.h>
#define int long long
using namespace std;

signed main() {
    // freopen("highcard.in", "r", stdin);
    // freopen("highcard.out", "w", stdout);
    int n;
    cin >> n;
    vector<int> b(n);
    for (int i = 0; i < n; i++) cin >> b[i];
    
    sort(b.begin(), b.end());
    
    vector<int> a;
    vector<bool> used(2*n + 1, false);
    for (int i : b) used[i] = true;
    for (int i = 1; i <= 2*n; i++)
        if (!used[i]) a.push_back(i);

    sort(a.rbegin(), a.rend());
    sort(b.rbegin(), b.rend());

    int i = 0, j = 0, count = 0;
    while (i < n && j < n) {
        if (a[i] > b[j]) {
            count++;
            i++;
            j++;
        } else {
            j++;
        }
    }

    cout << count << "\n";
    return 0;
}
