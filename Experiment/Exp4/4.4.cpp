#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
const int eps = 1e-12;
double a, b, c, d;
int n;


double f(double x)
{
    return exp(b * x) / sqrt(1 - exp(-c * x));
}
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len)
{
    if (left_endpt == 0)
        left_endpt = eps;
    double estimate, x;
    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (int i = 1; i <= trap_count - 1; i++)
    {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;
    return estimate;
}
int main(int argc, char *argv[])
{
    a = atof(argv[1]); //×ó±ß
    b = atof(argv[2]); //ÓÒ±ß
    c = atof(argv[3]);
    d = atof(argv[4]);
    n = atoi(argv[5]); //·Ö¿éÊýÁ¿
    int my_rank, comm_sz, local_n;
    double h, local_a, local_b;
    double local_int, total_int;
    int source;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    double startTime, endTime;
    MPI_Barrier(MPI_COMM_WORLD);
    startTime = MPI_Wtime();

    h = (b - a) / n;
    local_n = n / comm_sz;

    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;

    local_int = Trap(local_a, local_b, local_n, h);

    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    if (my_rank == 0)
    {
    	//printf("");
        cout << "Block Number n= " << n << endl;
        cout << "Int from " << a << " to " << b << " =" << total_int <<endl;
        cout << " TIme:" << endTime - startTime << endl;
    }
    MPI_Finalize();
    return 0;
}
