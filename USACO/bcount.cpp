#include <iostream>
#include <vector>
using namespace std;

int main()
{
    // freopen("bcount.in", "r", stdin);
    // freopen("bcount.out", "w", stdout);
    int n, q, i;
    cin >> n >> q;
    vector<int> hol(n + 1), gue(n + 1), jer(n + 1);
    for (i = 0; i < n; i++)
    {
        hol[i + 1] = hol[i];
        gue[i + 1] = gue[i];
        jer[i + 1] = jer[i];
        int id;
        cin >> id;
        if (id == 1)
            hol[i + 1]++;
        if (id == 2)
            gue[i + 1]++;
        if (id == 3)
            jer[i + 1]++;
    }
    vector<int> start(n + 1), end(n + 1);
    for (i = 0; i < q; i++)
    {
        cin >> start[i] >> end[i];
    }
    for (i = 0; i < q; i++)
    {
        cout << hol[end[i]] - hol[start[i] - 1] << ' ' << gue[end[i]] - gue[start[i] - 1] << ' ' << jer[end[i]] - jer[start[i] - 1] << '\n';
    }
    return 0;
}
