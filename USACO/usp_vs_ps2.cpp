#include<bits/stdc++.h>
#define int long long
using namespace std;
signed main(){
    int n,a,b,c;
    cin >> a >> b >> c;
    cin >> n;
    vector<pair<int,int>>mouses(n);
    for(int i = 0 ;i < n;i++){
        int x;
        string y;
        cin >> x >> y;
        if(y=="USB"){
            mouses[i]={x,1};
        }
        else{
            mouses[i]={x,0};
        }
    }
    sort(mouses.begin(),mouses.end());
    int total_cost=0;
    int used=0;
    for(int i = 0 ;i < n; i++){
        if(mouses[i].second==1) {
            if(a>0){
            used++;
            total_cost+=mouses[i].first;
            a--;
            }
            else if(c>0){
                used++;
                total_cost+=mouses[i].first;
                c--;
            }
        }
        else{
            if(b>0){
            used++;
            total_cost+=mouses[i].first;
            b--;
            }
            else if(c>0){
                used++;
                total_cost+=mouses[i].first;
                c--;
            }
        }
        cout << total_cost << endl;
        
    }
    cout << used << " " << total_cost << endl;
    return 0;
}