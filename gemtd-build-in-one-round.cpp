#include <bits/stdc++.h>
using namespace std;


enum COLOR
{
    R,Y,Q,E,B,D,P,G
};

//=================
// param
int player_level = 5; // 1~5, player's current level
int build_times = 5;

// what tower you want to build. default example is BHome
vector<pair<int, int>> target =
{
    // color(0-8), level(1-5)
    {B, 1},
    {B, 2},
    {B, 3},
    {B, 4},
    {B, 5},
};
//=================

double level_prob[5][5] =
{
    {1, 0, 0, 0, 0},
    {0.8, 0.2, 0.0, 0.0, 0.0},
    {0.6, 0.3, 0.1, 0.0, 0.0},
    {0.4, 0.3, 0.2, 0.1, 0.0},
    {0.1, 0.3, 0.3, 0.2, 0.1}
};


double dp[6][1<<5];
double cache[6][1<<5];

double calc(int t, int s)
{
    if (s == (1<<target.size())-1)
        return 1;
    if (t >= build_times)
        return 0;
    double& c = cache[t][s];
    if (c != -1)
        return c;
    c = 0;
    pair<int,int> last = make_pair(-1,-1);
    double other_prob = 1.0;
    for(int i = 0; i < target.size(); i++)
    {
        if(s>>i&1) continue;
        if (target[i] == last) continue;
        last = target[i];
        int color = target[i].first;
        int level = target[i].second;
        assert(level >= 1 && level <= 5);
        c += calc(t+1, s | (1<<i)) * (1.0/8) * level_prob[player_level-1][level-1];
        other_prob -= (1.0/8)*level_prob[player_level-1][level-1];
    };
    c += calc(t+1, s) * other_prob;
    return c;
}

int main()
{
    assert(player_level >= 1 && player_level <= 5);
    assert(target.size() <= build_times);
    assert(build_times <= 5);
    sort(target.begin(), target.end());
    for(int i = 0; i < 6; i++)
    for(int j = 0; j < 1<<5; j++)
        cache[i][j] = -1;
    cout<<"Probability is: "<<fixed<<setprecision(9)<<calc(0, 0)*100<<"%"<<endl;
    return 0;
}
