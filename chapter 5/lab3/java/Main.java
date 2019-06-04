import java.io.*;
import java.util.Scanner;
public class Main {
    final private static int MAX_NUM_ROUTERS=20;
    private static int MAX_ITERATION=200;
    private static int INFINITY=9999;
    private static int SELF=-1;
    private static int UNKNOWN=0;
    static boolean  converge(int [][][]table, int step) {
        return table[step-1] == table[step];
    }

    static void update(int [][][]t_line, int [][][] t_d, int router, int line, int delay, int n, int step)
    {
        for(int i=1; i<=n; ++i) {
            if(t_d[step][router][i] > t_d[step-1][line][i] + delay) {
                t_d[step][router][i] = t_d[step-1][line][i] + delay;
                t_line[step][router][i] = line;
            }
        }
    }

    public static void main(String[] args) {
        // redirect stdin to file.
        //freopen("test.txt", "r", stdin);
        //freopen("test.out", "w", stdout);

        int n;  // number of routers
        int m;  // number of edges
        int[][] A = new int[MAX_NUM_ROUTERS][MAX_NUM_ROUTERS];    // adjacent matrix

        // routing tables, three dimensional array.
        int[][][] table_d = new int[MAX_NUM_ROUTERS][MAX_NUM_ROUTERS][MAX_NUM_ROUTERS];      // indicate the delay/distance.
        int[][][] table_line = new int[MAX_NUM_ROUTERS][MAX_NUM_ROUTERS][MAX_NUM_ROUTERS];   // indicate forward the packet via which line.
        Scanner input = new Scanner(System.in);
        n = input.nextInt();
        m = input.nextInt();


        // initialize adjacent matrix.
        // !!! subscript starts from 1.
        for (int i = 1; i <= n; ++i)
            for (int j = 1; j <= n; ++j)
                A[i][j] = 0;

        int t1, t2, w;
        for (int i = 0; i < m; ++i) {
            t1 = input.nextInt();
            t2 = input.nextInt();
            w = input.nextInt();
            A[t1][t2] = w;
            A[t2][t1] = w;
        }

        // initialize routing tables.
        for (int i = 1; i <= n; ++i)
            for (int j = 1; j <= n; ++j) {
                if (i == j) {
                    table_d[0][i][j] = 0;
                    table_line[0][i][j] = SELF;
                } else if (A[i][j] > 0) {
                    table_d[0][i][j] = A[i][j];
                    table_line[0][i][j] = j;
                } else {
                    table_d[0][i][j] = INFINITY;
                    table_line[0][i][j] = UNKNOWN;
                }
            }

        // distance vector algorithm.
        int step = 0;
        do {
            step += 1;
            table_line[step] = table_line[step - 1];
            table_d[step] = table_d[step - 1];

            for (int i = 1; i <= n; ++i) {
                // every adjacent point can update "my" routing table.
                for (int j = 1; j <= n; ++j)
                    if (A[i][j] > 0)
                        update(table_line, table_d, i, j, A[i][j], n, step);
            }
        } while ((!converge(table_line, step) && step <= MAX_ITERATION));

        // output the result.
        // output the result.
        if (step <= MAX_ITERATION)
            System.out.println("Converge in " + step + "steps.");
        else
            System.out.println("Fail to Converge in " + step + "steps.");
        System.out.println("");

        System.out.println("Final routing table(line): " + step);
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j)
                System.out.println(table_line[step][i][j] + "\t");
            System.out.println("");
        }
        System.out.println("");

        System.out.println("Final routing table(delay)");
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j)
                System.out.println(table_d[step][i][j] + " ");
            System.out.println("");
        }
    }
}
