#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    freopen("triangles.in", "r", stdin);
	freopen("triangles.out", "w", stdout);
    int n;
    cin >> n;
    
    vector<pair<int, int>> a(n); 
    
    for (int i = 0; i < n; i++) {
        cin >> a[i].first >> a[i].second;
    }

    int result = 0;
    
    for (int i = 0; i < n; i++) {
        int resultX = 0, resultY = 0;
        
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            int x = abs(a[i].first - a[j].first);
            int y = abs(a[i].second - a[j].second);

            if (x == 0) resultY = max(resultY, y);
            if (y == 0) resultX = max(resultX, x);
        }
        
        result = max(result, resultX * resultY);
    }
    
    cout << result << endl;
    
    return 0;
}
