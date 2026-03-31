#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int n, count = 0;
    cin >> n;
    vector<int> p(n);
    
    for(int i = 0; i < n; i++) {
        cin >> p[i];
    }
    
    // Iterate over all subarrays (i, j)
    for(int i = 0; i < n; i++) {
        for(int j = i; j < n; j++) {
            int sum = 0;
            // Calculate sum of subarray from i to j
            for(int k = i; k <= j; k++) {
                sum += p[k];
            }
            
            // Calculate average of the subarray
            float avg = (float)sum / (j - i + 1);
            
            // Check if there exists a flower whose petals match the average
            for(int k = i; k <= j; k++) {
                if(p[k] == avg) {
                    count++;
                    break; // No need to check further flowers in this subarray
                }
            }
        }
    }

    cout << count << endl;
    return 0;
}
