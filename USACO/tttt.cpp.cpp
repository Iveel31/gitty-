#include <iostream>
#include <set>
#include <vector>

using namespace std;

int main() {
    freopen("tttt.in", "r", stdin);
    freopen("tttt.out", "w", stdout);
    vector<string> board(3);
    for (int i = 0; i < 3; i++) {
        cin >> board[i];
    }

    int individual_cows = 0, two_cows = 0;
    set<char> individual_winners;
    set<pair<char, char>> team_winners;

    // All possible winning lines
    int lines[8][3][2] = {
        {{0, 0}, {0, 1}, {0, 2}}, // Top row
        {{1, 0}, {1, 1}, {1, 2}}, // Middle row
        {{2, 0}, {2, 1}, {2, 2}}, // Bottom row
        {{0, 0}, {1, 0}, {2, 0}}, // Left column
        {{0, 1}, {1, 1}, {2, 1}}, // Middle column
        {{0, 2}, {1, 2}, {2, 2}}, // Right column
        {{0, 0}, {1, 1}, {2, 2}}, // Main diagonal
        {{0, 2}, {1, 1}, {2, 0}}  // Anti-diagonal
    };

    for (int i = 0; i < 8; i++) {
        set<char> cows;
        for (int j = 0; j < 3; j++) {
            int row = lines[i][j][0];
            int col = lines[i][j][1];
            cows.insert(board[row][col]);
        }

        if (cows.size() == 1) { 
            // A single cow occupies all 3 positions
            individual_winners.insert(*cows.begin());
        } else if (cows.size() == 2) { 
            // Exactly two cows occupy the line
            auto it = cows.begin();
            char first = *it;
            char second = *next(it);
            team_winners.insert({min(first, second), max(first, second)});
        } else { 
            // More than two cows in the line, so it's not a valid win condition
            continue;
        }
    }

    cout << individual_winners.size() << endl;
    cout << team_winners.size() << endl;

    return 0;
}
