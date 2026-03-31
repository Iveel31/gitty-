#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    int d1,d2,d3;
    cin >> d1 >> d2 >> d3;
    int k;
    cin >> k;
    int m=1;
    int a[1000];
    a[0]=d1;
    a[1]=d2;
    a[2]=d3;
    a[3]=(d1+d2+d3)%10;
    while(d1!=a[m] || a[m+1]!=d2 || a[m+2]!=d3){
        a[m+3]=(a[m]+a[m+1]+a[m+2])%10;
        m++;
    }
    for(int i = 0 ; i < m+3; i++){ cout << a[i]<< " ";}
    cout << m+2;
    return 0;
}
