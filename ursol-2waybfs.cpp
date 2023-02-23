
#include <bits/stdc++.h>
#include "mvp_formation.h"
using namespace std;


//bool CHECK_4MVP = true; //是否要管4max


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

const int NDATA = (N*M+N_BASE5-1)/N_BASE5;

uint64_t POW[N_BASE5+1] = {0};


struct State
{
    State()
    {
        for (int i = 0; i < NDATA; i++)
            data[i] = 0;
    }
    State(const State& s)
    {
        for (int i = 0; i < NDATA; i++)
            data[i] = s.data[i];
    }

    State& operator=(const State& s)
    {
        for (int i = 0; i < NDATA; i++)
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
    uint64_t data[NDATA];

    bool operator==(const State& s) const {
        for (int i = 0; i < NDATA; i++)
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
        for (int i = 0; i < NDATA; i++)
        {
            r = (r * 1031) ^ (s.data[0] >> 32);
            r = (r * 1031) ^ (s.data[0] & INT_MAX);
        }
        return (size_t)r;
    }
};

int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (int i = 0; i < 28; i++)
    {
        POW[i] = i ? POW[i-1]*5:1;
    }

    int mvps = 0, buffed = 0, sub_buffed = 0;

    State start{};
    State all_stone{};
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            assert(maze[i][j] >= 0 && maze[i][j] <= 4);
            start.set(i,j,maze[i][j]);

            if (maze[i][j] == 2)
                mvps++;
            if (maze[i][j] == 3)
                buffed++;
            if (maze[i][j] == 4)
                sub_buffed++;

            all_stone.set(i,j,maze[i][j] > 0 ? 1:0);
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
    unordered_map<State, State, hashState> mp_state;
    unordered_map<State, pair<int,int>, hashState> mp_action;

    unordered_map<State, int, hashState> mp_rev;
    unordered_map<State, State, hashState> mp_rev_state;
    unordered_map<State, pair<int,int>, hashState> mp_rev_action;
    mp[start] = 0;
    queue<State> q;
    q.push(start);

    CMvpFormation MF;
    int cnt = MF.calc(mvps, buffed, sub_buffed);
    cout << "target state count:" << cnt << endl;
    if (cnt == 0)
    {
        cout << "impossible to build such formation! mvps:" << mvps << ", buffed:" << buffed << ", sub-buffed" << sub_buffed << endl;
        return 0;
    }

    queue<State> q_rev;

    for (int i = 0; i < N-5; i++)
    {
        for (int j = 0; j < M-5; j++)
        {
            for (const SFormation& f : MF.ans_set)
            {
                State target_state = all_stone;
                int ok = 1;
                for (int x = 0; x < 5 && ok; x++)
                {
                    for (int y = 0; y < 5; y++)
                    {
                        if (f.m[x][y] == 0)
                            continue;
                        if (target_state.get(i+x, j+y) != 0)
                        {
                            target_state.set(i+x, j+y, f.m[x][y]);
                        } else {
                            ok = 0;
                            break;
                        }
                    }
                }
                if (ok)
                {
                    mp_rev[target_state] = 0;
                    q_rev.push(target_state);
                }
            }
        }
    }

    if (q_rev.empty())
    {
        cout << "impossible to fit such formation into exist stones! mvps:" << mvps << ", buffed:" << buffed << ", " << sub_buffed << endl;
        return 0;
    }

    int maxsteps = 0;
    int maxrevsteps = 0;

    int cnt_states = 0;
    int cnt_rev_states = 0;

    bool is_rev = false;
    while(true)
    {
        State s;
        int d;

        if (!is_rev)
        {
            s = q.front();
            if (mp[s] + 1 > maxsteps)
            {
                if (cnt_states > cnt_rev_states)
                {
                    is_rev = !is_rev;
                }
            }
        } else {
            s = q_rev.front();
            if (mp_rev[s] + 1 > maxrevsteps)
            {
                if (cnt_states <= cnt_rev_states)
                {
                    is_rev = !is_rev;
                }
            }
        }

        if (!is_rev)
        {
            s = q.front();
            q.pop();
            d = mp[s];
            cnt_states ++;
        } else {
            s = q_rev.front();
            q_rev.pop();
            d = mp_rev[s];
            cnt_rev_states ++;
        }


        if (!is_rev && d + 1> maxsteps)
        {
            maxsteps = d + 1;
            cout << "searching for " << d+1 << " steps..." << endl;
        }
        if (is_rev && d + 1> maxrevsteps)
        {
            maxrevsteps = d + 1;
            cout << "searching reverse for " << d+1 << " steps..." << endl;
        }
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

                if (is_rev)
                {
                    std::reverse(v.begin(), v.end());
                }

                //ursol rotate
                for(int i = 0; i < v.size(); i++)
                {
                    ns.set(v[(i+1)%v.size()].first, v[(i+1)%v.size()].second, s.get(v[i].first, v[i].second));
                }



                bool found = false;
                if (!is_rev)
                {
                    if(mp.count(ns))
                        continue;
                    if(mp_rev.count(ns))
                    {
                        found = true;
                    }
                    mp[ns] = d + 1;
                    mp_state[ns] = s;
                    mp_action[ns] = make_pair(i,j);
                    q.push(ns);
                } else {
                    if(mp_rev.count(ns))
                        continue;
                    if(mp.count(ns))
                    {
                        found = true;
                    }
                    mp_rev[ns] = d + 1;
                    mp_rev_state[ns] = s;
                    mp_rev_action[ns] = make_pair(i,j);
                    q_rev.push(ns);
                }


                if (found)
                {
                    cout << "found result: " << mp[ns] + mp_rev[ns] << " steps." << endl;
                    cout << mp[ns] <<' ' << mp_rev[ns]<<endl;


                    vector<State> v_steps;
                    vector<State> v_actions;

                    State xs = ns;
                    while(true)
                    {
                        v_steps.push_back(xs);
                        if (!mp_state.count(xs))
                            break;
                        xs = mp_state[xs];
                    }

                    std::reverse(v_steps.begin(), v_steps.end());

                    xs = ns;
                    while(true)
                    {
                        if (!mp_rev_state.count(xs))
                            break;
                        xs = mp_rev_state[xs];
                        v_steps.push_back(xs);
                    }

                    for(int i = 0; i < v_steps.size(); i++)
                    {
                        const auto& state = v_steps[i];

                        cout << "Step " << i << ":" <<endl;

                        pair<int,int> ac(-1,-1);
                        if (i+1 < v_steps.size() && mp_action.count(v_steps[i+1]))
                        {
                            ac = mp_action[v_steps[i+1]];
                        }
                        if (mp_rev_action.count(v_steps[i]))
                        {
                            ac = mp_rev_action[v_steps[i]];
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

            }
        }
    }

END:
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout<<"time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << endl;
    return 0;
}
