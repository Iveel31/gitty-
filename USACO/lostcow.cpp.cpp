
    
    
    #include <iostream>
#include <vector>
#include <cmath>
using namespace std;
int main() {
    freopen("lostcow.in", "r", stdin);   // Open input file
    freopen("lostcow.out", "w", stdout); // Open output file
    
    int x, y;
    cin >> x >> y; 
    
    int distance = 0;
    int step = 1;
    int current = x;
    
    while (true) {
        int next = x + step;
        if ((x < y && next >= y) || (x > y && next <= y)) {
            distance += abs(y - current);
            break;
        } else {
            distance += abs(next - current);
        }
        current = next;
        step *= -2;
    }
    
    cout << distance << endl; 
    return 0;
}
