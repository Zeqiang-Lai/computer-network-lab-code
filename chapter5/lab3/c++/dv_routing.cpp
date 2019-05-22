// dv_routing.cpp
// Simulation of distance vector routing.

#include <iostream>
#include <array>
using namespace std;

#define MAX_NUM_ROUTERS 20
#define MAX_ITERATION 200
#define INFINITY 9999
#define SELF -1
#define UNKNOWN 0

typedef array<array<array<int, MAX_NUM_ROUTERS>, MAX_NUM_ROUTERS>, MAX_ITERATION> Table;

bool converge(Table& table, int step) {
    return table[step-1] == table[step];
} 

void update(Table& t_line, Table& t_d, int router, int line, int delay, int n, int step)
{
    for(int i=1; i<=n; ++i) {
        if(t_d[step][router][i] > t_d[step-1][line][i] + delay) {
            t_d[step][router][i] = t_d[step-1][line][i] + delay;
            t_line[step][router][i] = line;
        }
    }
}

int main()
{
    // redirect stdin to file.
    freopen("test.txt", "r", stdin);
    freopen("test.out", "w", stdout);

    int n;  // number of routers
    int m;  // number of edges
    int A[MAX_NUM_ROUTERS][MAX_NUM_ROUTERS];    // adjacent matrix

    // routing tables, three dimensional array.
    Table table_d;      // indicate the delay/distance.
    Table table_line;   // indicate forward the packet via which line.

    cin >> n;
    cin >> m;

    // initialize adjacent matrix.
    // !!! subscript starts from 1.
    for(int i=1; i<=n; ++i)
        for(int j=1; j<=n; ++j)
            A[i][j] = 0;
    
    int t1, t2, w;
    for(int i=0; i<m; ++i) {
        cin >> t1 >> t2 >> w; 
        A[t1][t2] = w;
        A[t2][t1] = w;
    }
    
    // initialize routing tables.
    for(int i=1; i<=n; ++i)
        for(int j=1; j<=n; ++j) {
            if(i == j) {
                table_d[0][i][j] = 0;
                table_line[0][i][j] = SELF;
            }
            else if(A[i][j] > 0) {
                table_d[0][i][j] = A[i][j];
                table_line[0][i][j] = j;
            }
            else {
                table_d[0][i][j] = INFINITY;
                table_line[0][i][j] = UNKNOWN;
            }
        }

    // distance vector algorithm.
    int step = 0;
    do{
        step += 1;
        table_line[step] = table_line[step-1];
        table_d[step] = table_d[step-1];

        for(int i=1; i<=n; ++i) {
            // every adjacent point can update "my" routing table.
            for(int j=1; j<=n; ++j) 
                if(A[i][j] > 0) 
                    update(table_line, table_d, i, j, A[i][j], n, step);            
        }
    }while((!converge(table_line, step) && step <= MAX_ITERATION));

    // output the result.
    if(step <= MAX_ITERATION) 
        cout << "Converge in " << step << "steps." << endl;
    else 
        cout << "Fail to Converge in " << step << "steps." << endl;
    cout << endl;

    cout << "Final routing table(line): " << step << endl;
    for(int i=1; i<=n; ++i) {
        for(int j=1; j<=n; ++j)
            cout << table_line[step][i][j] << "\t";
        cout << endl;
    }
    cout << endl;

    cout << "Final routing table(delay)" << endl;
    for(int i=1; i<=n; ++i) {
        for(int j=1; j<=n; ++j)
            cout << table_d[step][i][j] << " ";
        cout << endl;
    }
    
    return 0;
}