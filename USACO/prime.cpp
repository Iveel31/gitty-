#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >> n;
    vector<bool>visited(n,0);
    vector<int>prime;
    for(int i = 2; i <= n; i++){
        if(visited[i]==0) prime.push_back(i);
        else continue;
        for(int j = i; j <= n; j+=i){
            if(visited[j]==0) visited[j]=1;
        }
    }
    for(int i = 0;i < prime.size(); i++){
        cout << prime[i] << " " ;
        }
    return 0;
}