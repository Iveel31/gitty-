#include <bits/stdc++.h>
using namespace std;
int print_digits(int x , int n){

    if(n==2){
        vector<int>a;
        while(x>0){
            a.push_back(x%2);
            x/=2;
        }
        int s=0;
        for(int i=n-1;i>=0;i--){
            s=s*10+a[i];
        }
        return s;
    }
    else return x;
  
}
int main() {
    // int fib[7] = {1, 1, 2, 3, 5, 8, 13};
    // int n;
    // cin >> n;
    // for (int i = 0; i < n; i++) {
    //     cout << fib[i % 7] << " ";
    // }
    int x , n;
    cin >> x >> n;
    cout << print_digits(x,n);

    return 0;
}
