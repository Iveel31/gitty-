#include <bits/stdc++.h>
using namespace std;

void hanoi(int n, int from, int aux, int to) {
    if (n == 0) return;
    hanoi(n - 1, from, to, aux);
    cout << from << " " << to << '\n';
    hanoi(n - 1, aux, from, to);
}

int main() {
    int n;
    cin >> n;
    cout << (1LL << n) - 1 << '\n';
    hanoi(n, 1, 2, 3);
    return 0;
}
