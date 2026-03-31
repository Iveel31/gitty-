#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

bool simulate_infection(int N, int T, const vector<int>& initial_state, const vector<tuple<int, int, int>>& interactions, int patient_zero, int K) {
    // Initialize infected state and shake count
    vector<bool> infected(N, false);
    vector<int> shakes_count(N, 0);
    infected[patient_zero] = true;  // Set patient zero as infected
    
    // Process the interactions in order
    for (const auto& interaction : interactions) {
        int t, x, y;
        tie(t, x, y) = interaction;
        if (infected[x] && shakes_count[x] < K) {
            infected[y] = true;
        }
        if (infected[y] && shakes_count[y] < K) {
            infected[x] = true;
        }
        shakes_count[x]++;
        shakes_count[y]++;
    }
    
    // Check if the final infection state matches the input state
    for (int i = 0; i < N; ++i) {
        if (initial_state[i] == 1 && !infected[i]) {
            return false;
        }
        if (initial_state[i] == 0 && infected[i]) {
            return false;
        }
    }
    return true;
}

int main() {
      freopen("tracing.in", "r", stdin);
      freopen("tracing.out", "w", stdout);
    int N, T;
    cin >> N >> T;
    
    vector<int> initial_state(N);
    for (int i = 0; i < N; ++i) {
        cin >> initial_state[i];
    }
    
    vector<tuple<int, int, int>> interactions(T);
    for (int i = 0; i < T; ++i) {
        int t, x, y;
        cin >> t >> x >> y;
        interactions[i] = {t, x - 1, y - 1};  // Convert to 0-indexed
    }
    
    vector<int> possible_patient_zeros;
    int min_k = INT_MAX;
    int max_k = -1;
    
    // Try every cow as the potential patient zero
    for (int patient_zero = 0; patient_zero < N; ++patient_zero) {
        for (int K = 0; K <= T; ++K) {
            if (simulate_infection(N, T, initial_state, interactions, patient_zero, K)) {
                possible_patient_zeros.push_back(patient_zero);
                min_k = min(min_k, K);
                max_k = max(max_k, K);
            }
        }
    }
    
    // Output the result
    cout << possible_patient_zeros.size() << " " << min_k << " ";
    if (max_k == T) {
        cout << "Infinity" << endl;
    } else {
        cout << max_k << endl;
    }
    
    return 0;
}
