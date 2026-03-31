#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    int count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int sum = 0;
            int k = 0;
            for (int l = i; l <= j; l++) {
                sum += a[l];
                k++;
            }
            if (sum % k == 0) {
                int avg = sum / k;
                bool found = false;
                for (int l = i; l <= j; l++) {
                    if (a[l] == avg) {
                        found = true;
                        break;
                    }
                }
                if (found) count++;
            }
        }
    }

    cout << count;
    return 0;
}
