#include <bits/stdc++.h>
using namespace std;


struct SFormation
{
    int m[5][5] = {};
    SFormation(int a[5][5])
    {
        for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            m[i][j] = a[i][j];
    }
    operator=(const SFormation& s) const
    {
        for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (m[i][j] != s.m[i][j])
                return false;
        return true;
    }
    operator<(const SFormation& s) const
    {
        for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            if (m[i][j] < s.m[i][j])
                return true;
            else if (m[i][j] > s.m[i][j])
                return false;
        }
        return false;
    }
};

class CMvpFormation
{
public:
    int m[5][5] = {};
    // mvps: count of mvp towers, also buffed by other mvps
    // buffed: count of full buffed towers
    // sub_buffed: count of towers buffed by mvp towers - 1


    set<SFormation> visited;
    set<SFormation> ans_set;

    void dfs(int mvps, int buffed, int sub_buffed)
    {
        SFormation v_state(m);
        if (visited.count(v_state))
            return;
        visited.insert(v_state);

        vector<pair<int,int>> v_mvps;
        for (int i = 1; i <= 3; i++)
        {
            for (int j = 1; j <= 3; j++)
            {
                if (m[i][j] == 2)
                    v_mvps.push_back(make_pair(i,j));
            }
        }

        if (mvps > 0)
        {
            for (int i = 1; i <= 3; i++)
            {
                for (int j = 1; j <= 3; j++)
                {
                    int ok = 1;
                    for (auto& mvp : v_mvps)
                    {
                        int dx = abs(mvp.first - i);
                        int dy = abs(mvp.second - j);
                        if (dx > 2 || dy > 2 || dx + dy > 3)
                            ok = 0;
                    }
                    if(m[i][j] == 0 && ok)
                    {
                        m[i][j] = 2;
                        dfs(mvps-1, buffed, sub_buffed);
                        m[i][j] = 0;
                    }
                }
            }
        }
        else if (buffed > 0 || sub_buffed > 0)
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    int badcnt = 0;
                    for (auto& mvp : v_mvps)
                    {
                        int dx = abs(mvp.first - i);
                        int dy = abs(mvp.second - j);
                        if (dx > 2 || dy > 2 || dx + dy > 3)
                            badcnt++;
                    }
                    if (m[i][j] == 0)
                    {
                        if (buffed > 0 && badcnt == 0)
                        {
                            m[i][j] = 3;
                            dfs(mvps, buffed - 1, sub_buffed);
                            m[i][j] = 0;
                        }
                        else if (buffed == 0 && sub_buffed > 0 && badcnt <= 1)
                        {
                            m[i][j] = 4;
                            dfs(mvps, buffed, sub_buffed - 1);
                            m[i][j] = 0;
                        }
                    }
                }
            }
        }
        else
        {
            ans_set.insert(v_state);
        }
    }

    int calc(int mvps, int buffed, int sub_buffed)
    {
        ans_set.clear();
        dfs(mvps, buffed, sub_buffed);
        visited.clear();
        return ans_set.size();
    }
};
