#include <iostream>
#include <vector>
#include <string>
using namespace std;
int main()
{
	freopen("breedflip.in", "r", stdin);
    freopen("breedflip.out", "w", stdout);
    int n;
    cin >> n;
    string a , b;
    cin >> a >> b;
    vector<int>k;
    for(int i = 0 ; i < a.size() ; i++){
        if(a[i] != b[i]) k.push_back(i);
    }
    int count=0;
    for(int i = 1 ; i < k.size() ; i++){
        if(k[i]-1!=k[i-1]) count++;
    }
    if(k.size()==0) cout << 0;
    else cout << count+1;
    return 0;
}