#include <bits/stdc++.h>
#include "mvp_formation.h"
using namespace std;

int main()
{
    CMvpFormation m;
    for(int i = 0; i <= 10; i++)
    {
        for(int j = 0; j <= 10; j++)
        {
            cout << m.calc(5, i, j) << ' ';
        }
        cout<<endl;
    }
    return 0;
}
