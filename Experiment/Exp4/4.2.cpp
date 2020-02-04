#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
double left_h, right_h, a, b;
int n;
double f(double x)
{
    return (right_h - left_h) / (b - a) * (x - a) + left_h;
}
double Trap(double left_endpt,double right_endpt,int trap_count,double base_len){
	double estimate,x;
	int i;
	estimate=(f(left_endpt)+f(right_endpt))/2.0;
	for(i=1;i<trap_count-1;i++){
		x=left_endpt+i*base_len;
		estimate+=f(x);
	}
	estimate=estimate*base_len;
	return estimate;
}
int main(int argc, char *argv[])
{
	left_h = atof(argv[1]);
    right_h = atof(argv[2]);
    a = atof(argv[3]);
    b = atof(argv[4]);
    n = atoi(argv[5]);
    int my_rank, comm_sz, local_n;
    double h, local_a, local_b;
    double local_int, total_int;
    int source;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	
	h=(b-a)/n;
	local_n=n/comm_sz;
	
	local_a=a+my_rank*local_n*h;
	local_b=local_a+local_n*h;
	local_int = Trap(local_a,local_b,local_n,h);
	
	MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if(my_rank==0){
		printf("with n= %d trapezoids, our estimate\n", n);
        printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
	}
	MPI_Finalize();
	return 0;
}
