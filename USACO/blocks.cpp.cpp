#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

vector<int> a(26), letter(26), b(26);

int main() {
    freopen("blocks.in", "r", stdin);
    freopen("blocks.out", "w", stdout);
    int n;
    cin >> n;
    string word1, word2;
    vector<pair<string, string>> word;

    for (int i = 0; i < n; i++) {
        cin >> word1 >> word2;
        word.push_back({word1, word2});
    }

    for (int i = 0; i < n; i++) {
        for (char c : word[i].first) {
            a[c - 'a']++;
        }
        for (char c : word[i].second) {
            b[c - 'a']++;
        }
        for (int j = 0; j < 26; j++) {
            letter[j] += max(a[j], b[j]);
        }
        fill(a.begin(), a.end(), 0);
        fill(b.begin(), b.end(), 0);
    }

    for (int i = 0; i < 26; i++) {
        cout << letter[i] << "\n";
    }

    return 0;
}
