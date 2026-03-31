#include <bits/stdc++.h>
using namespace std;

int reverseNum(int n) {
    // if (n < 10) return n;

    int digits = log10(n);         
    int lastDigit = n % 10;

    return lastDigit * pow(10, digits) + reverseNum(n / 10);
}
int main() {
    int n;
    cin >> n;
    cout << reverseNum(n);
    return 0;
}
