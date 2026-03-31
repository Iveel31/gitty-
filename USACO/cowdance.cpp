#include <iostream>
using namespace std;

int n, k;
int a[100001];

int bs(int x) {
    int l = -1, r = n;
    while (l + 1 < r) {
        int m = (l + r) / 2;
        if (a[m] <= x) l = m;
        else r = m;
    }
    return l;
}

int main() {
    cin >> n >> k;
    for (int i = 0; i < n; i++) cin >> a[i];
    int j=bs(n);
	cout << j;
//    for (int i = 0; i < k; i++) {
//        int x;
//        cin >> x;
//        int j = bs(x);
//        
//        if (j >= 0) {
//            cout << j + 1 << "\n"; 
//        } else {
//            cout << 0 << "\n"; 
//        }
//    }

    return 0;
}
