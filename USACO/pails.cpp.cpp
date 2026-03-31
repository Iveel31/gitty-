#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(){
    int n,i,j,num;
    cin >> n;
    char letter;
    vector<int>G,L,all;
    for(i=0;i<n;i++){
        cin >> letter >> num;
        if(letter=='G'){
            G.push_back(num);
        }
        if(letter=='L'){
            L.push_back(num);
        }
        all.push_back(num);
    }
    sort(G.begin(),G.end());
    sort(L.begin(),L.end());
    int lie=0,mini=n;
    for(i=0;i<all.size();i++){
        int lie=0;
        for(j=0;j<L.size();j++){
            if(all[j]>L[j]){
                lie++;
            }
        }
        for(j=0;j<G.size();j++){
            if(all[j]<G[j]){
                lie++;
            }
        }
        mini=min(mini,lie);
    }
    cout << mini;
    return 0;
}
