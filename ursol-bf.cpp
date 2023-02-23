
#include <bits/stdc++.h>
using namespace std;


bool CHECK_4MVP = true; //是否要管4max


const int N_BASE5 = 27;
int D[8][2] = {{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
/*
地图
0:路; 1:石头; 2:max塔(当然也要吃满max); 3:要吃满max的塔; 4:要吃4max的塔;
*/
const int N = 7;
const int M = 8;
int maze[N][M] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 3, 2, 3, 1, 4, 1, 0},
    {0, 0, 3, 2, 1, 1, 0, 0},
    {1, 0, 2, 2, 2, 1, 0, 1},
    {0, 0, 3, 1, 1, 4, 0, 0},
    {0, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
};

uint64_t POW[28] = {0};

struct State
{
    State()
    {
        for (int i = 0; i < 3; i++)
            data[i] = 0;
    }
    State(const State& s)
    {
        for (int i = 0; i < 3; i++)
            data[i] = s.data[i];
    }

    State& operator=(const State& s)
    {
        for (int i = 0; i < 3; i++)
            data[i] = s.data[i];
        return *this;
    }

    int get(int i, int j) const
    {
        int x = i*M+j;
        int c = 0;
        while (x >= N_BASE5)
        {
            x -= N_BASE5;
            c ++;
        }
        return (data[c] / POW[x]) % 5;
    }
    void set(int i, int j, int v)
    {
        int x = i*M+j;
        int c = 0;
        while (x >= N_BASE5)
        {
            x -= N_BASE5;
            c ++;
        }
        int b = (data[c] / POW[x]) % 5;
        data[c] = data[c] + (v-b) * POW[x];
    }
    uint64_t data[3];

    bool operator==(const State& s) const {
        for (int i = 0; i < 3; i++)
            if (data[i] != s.data[i])
                return false;
        return true;
    }
};

struct hashState
{
    size_t operator()(const State& s) const
    {
        uint64_t r = 0;
        for (int i = 0; i < 3; i++)
        {
            r = (r * 1031) ^ (s.data[0] >> 32);
            r = (r * 1031) ^ (s.data[0] & INT_MAX);
        }
        return (size_t)r;
    }
};

bool check(const State& s)
{
    vector<pair<int,int>> v_max;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (s.get(i,j) == 2)
            {
                v_max.push_back(make_pair(i,j));
            }
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            int type = s.get(i,j);
            if (type == 2 || type == 3 || type == 4)
            {
                int c = 0;
                for (const auto& mx : v_max)
                {
                    int dx = abs(mx.first-i);
                    int dy = abs(mx.second-j);
                    if (dx <= 2 && dy <= 2 && dx + dy <= 3)
                    {
                        c++;
                    }
                }
                if (type == 2 || type == 3)
                {
                    if (c < 5)
                        return false;
                }
                if (type == 4 && CHECK_4MVP)
                {
                    if (c < 4)
                        return false;
                }
            }
        }
    }
    return true;
}

int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (int i = 0; i < 28; i++)
    {
        POW[i] = i ? POW[i-1]*5:1;
    }

    State start{};
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            start.set(i,j,maze[i][j]);
        }
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            cout<<start.get(i,j)<<' ';
        }
        cout<<endl;
    }


    unordered_map<State, int, hashState> mp;
    unordered_map<State, State, hashState> mp_rev;
    unordered_map<State, pair<int,int>, hashState> mp_rev_action;
    mp[start] = 0;
    queue<State> q;
    q.push(start);
    while(true)
    {
        auto s = q.front();
        q.pop();
        int d = mp[s];
        for(int i = 1; i < N-1; i++)
        {
            for(int j = 1; j < M-1; j++)
            {
                if (s.get(i,j)==0)
                    continue;
                vector<pair<int,int>> v;
                for(int k = 0; k < 8; k++)
                {
                    int ix = i+D[k][0];
                    int jx = j+D[k][1];
                    if(s.get(ix,jx) != 0)
                    {
                        v.push_back(make_pair(ix,jx));
                    }
                }
                if (v.size() <= 1) continue;
                State ns = s;
                for(int i = 0; i < v.size(); i++)
                {
                    ns.set(v[(i+1)%v.size()].first, v[(i+1)%v.size()].second, s.get(v[i].first, v[i].second));
                }

                if(mp.count(ns))
                    continue;

                mp[ns] = d + 1;
                mp_rev[ns] = s;
                mp_rev_action[ns] = make_pair(i,j);



                if (check(ns))
                {
                    cout << "found result: " << d+1 << " steps." << endl;


                    vector<State> v_steps;
                    vector<State> v_actions;

                    State xs = ns;
                    while(true)
                    {
                        v_steps.push_back(xs);
                        if (!mp_rev.count(xs))
                            break;
                        xs = mp_rev[xs];
                    }

                    std::reverse(v_steps.begin(), v_steps.end());

                    for(int i = 0; i < v_steps.size(); i++)
                    {
                        const auto& state = v_steps[i];

                        cout << "Step " << i << ":" <<endl;

                        pair<int,int> ac(-1,-1);
                        if (i+1 < v_steps.size() && mp_rev_action.count(v_steps[i+1]))
                        {
                            ac = mp_rev_action[v_steps[i+1]];
                        }

                        char nx = ' ';
                        for(int i = 0; i < N; i++)
                        {
                            for(int j = 0; j < M; j++)
                            {
                                if (ac.first == i && ac.second == j)
                                {
                                    cout << "<" << state.get(i, j);
                                    if (j == M-1)
                                        cout<<'>';
                                    else
                                        nx = '>';
                                }
                                else
                                {
                                    cout << nx << state.get(i, j);
                                    nx = ' ';
                                }
                            }
                            cout<<endl;
                        }
                    }
                    goto END;
                }

                q.push(ns);
            }
        }
    }

END:
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout<<"time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << endl;
    return 0;
}
