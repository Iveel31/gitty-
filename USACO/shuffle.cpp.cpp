#include <iostream>
#include <vector>
using namespace std;

int main() {
     freopen("shuffle.in", "r", stdin);
    freopen("shuffle.out", "w", stdout);
    int n;
    cin >> n;

    vector<int> shuffle(n + 1), cows(n + 1);

    // Read shuffle positions
    for (int i = 1; i <= n; i++) {
        cin >> shuffle[i];
    }

    // Read cow IDs after three shuffles
    for (int i = 1; i <= n; i++) {
        cin >> cows[i];
    }

    // Reverse the shuffle process three times
    for (int t = 0; t < 3; t++) {
        vector<int> temp(n + 1);
        for (int i = 1; i <= n; i++) {
            
            temp[i] = cows[shuffle[i]];
        }
        cows = temp;
    }

    // Output the initial cow order
    for (int i = 1; i <= n; i++) {
        cout << cows[i] << "\n";
    }

    return 0;
}
