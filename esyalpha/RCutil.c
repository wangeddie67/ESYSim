#include "RC.h"
#include "flp.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "headers.h"

/* get the thermal resistance values. 'k' is the thermal conductivity
 * of the actual material (silicon or copper). Lb and Wb are bulk length
 * and width respectively. Ws is the source width. 't' is the thickness
 * The calculations are from "Heat Spreading and Conduction in Compressed 
 * Heatsinks", Jaana Behm and Jari Huttunen, in proceedings of the 10th
 * International Flotherm User Conference, May 2001.
 */

double getr(double k, double Wb, double Lb, double Ws, double t)
{
	double theta, Bi, As, Ab, epsilon, tau, a, b, lamda, phi, psi, Rsprd;
	theta = (Lb/2)/(k*Wb*t);
	As = Ws*t;
	Ab = Wb*t;
	a = sqrt(As/M_PI);
	b = sqrt(Ab/M_PI);
	epsilon = a/b;
	lamda = M_PI+1/(sqrt(M_PI)*epsilon);
	Bi = 1/(M_PI*k*b*theta);
	tau = Lb/(2*b);
	phi = (tanh(lamda*tau)+lamda/Bi)/(1+(lamda/Bi)*tanh(lamda*tau));
	psi = (epsilon*tau+fabs(1-epsilon)*phi)/sqrt(M_PI);
	Rsprd = psi/(sqrt(M_PI)*k*a);
	return Rsprd;
}

void iswap (int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void dswap (double *a, double *b)
{
	double t = *a;
	*a = *b;
	*b = t;
}

/*
 * LUP decomposition from the pseudocode given in the CLR 
 * 'Introduction to Algorithms' textbook. The matrix 'a' is
 * transformed into an in-place lower/upper triangular matrix
 * and the vector'p' carries the permutation vector such that
 *  Pa = lu, where 'P' is the matrix form of 'p'
 */
 
void lupdcmp(double**a, int n, int *p)
{
	int i, j, k, pivot=0;
	double max = 0;

	/* start with identity permutation	*/
	for (i=0; i < n; i++)
		p[i] = i;

	for (k=0; k < n-1; k++)	 {
		max = 0;
		for (i = k; i < n; i++)	{
			if (fabs(a[i][k]) > max) {
				max = fabs(a[i][k]);
				pivot = i;
			}
		}	
		if (eq (max, 0))
			fatal ("singular matrix in lupdcmp\n");

		/* bring pivot element to position	*/
		iswap (&p[k], &p[pivot]);
		for (i=0; i < n; i++)
			dswap (&a[k][i], &a[pivot][i]);

		for (i=k+1; i < n; i++) {
			a[i][k] /= a[k][k];
			for (j=k+1; j < n; j++)
				a[i][j] -= a[i][k] * a[k][j];
		}
	}
}

/* 
 * the matrix a is an in-place lower/upper triangular matrix
 * the following macros split them into their constituents
 */

#define LOWER(a, i, j)		((i > j) ? a[i][j] : 0)
#define UPPER(a, i, j)		((i <= j) ? a[i][j] : 0)

/* 
 * LU forward and backward substitution from the pseudocode given
 * in the CLR 'Introduction to Algorithms' textbook. It solves ax = b
 * where, 'a' is an in-place lower/upper triangular matrix. The vector
 * 'x' carries the solution vector. 'p' is the permutation vector
 */

void lusolve(double **a, int n, int *p, double *b, double *x)
{
	int i, j;
	double *y = vector (n);
	double sum;
	
	/* forward substitution	- solves ly = pb	*/
	for (i=0; i < n; i++) {
		for (j=0, sum=0; j < i; j++)
			sum += y[j] * LOWER(a, i, j);
		y[i] = b[p[i]] - sum;
	}	

	/* backward substitution - solves ux = y	*/
	for (i=n-1; i >= 0; i--) {
		for (j=i+1, sum=0; j < n; j++)
			sum += x[j] * UPPER(a, i, j);
		x[i] = (y[i] - sum) / UPPER(a, i, i);
	}	

	free_vector(y);
}

/* rk4: 4th order Runge-Kutta method 
 * 
 * Given values for y, advance the solution over an interval h, and the return
 * the solution in yout.
 */
void rk4(double **c, double *y, double *pow, int n, double h, double *yout)
{
	int i,j;
	double *yt, *k1, *k2, *k3, *k4;
	yt = vector(n);
	k1 = vector(n);
	k2 = vector(n);
	k3 = vector(n);
	k4 = vector(n);

	for (i = 0; i < n; i++) { 
		yt[i] = 0.0;
		for (j = 0; j < n; j++) 
			yt[i] += c[i][j]*y[j];
		k1[i] = h*(pow[i]-yt[i]);
	}
	for (i = 0; i < n; i++) {
		yt[i] = 0.0;
		for (j = 0; j < n; j++) 
			yt[i] += c[i][j]*(y[j]+0.5*k1[j]);
		k2[i] = h*(pow[i]-yt[i]);
	}
	for (i = 0; i < n; i++) {
		yt[i] = 0.0;
		for (j = 0; j < n; j++) 
			yt[i] += c[i][j]*(y[j]+0.5*k2[j]);
		k3[i] = h*(pow[i]-yt[i]);
	}
	for (i =0; i < n; i++) {
		yt[i]=0.0;
		for (j = 0; j < n; j++) 
			yt[i] += c[i][j]*(y[j]+k3[j]);
		k4[i] = h*(pow[i]-yt[i]);
	}

	for (i =0; i < n; i++) 
		yout[i] = y[i] + (k1[i] + 2*k2[i] + 2*k3[i] + k4[i])/6.0;

	free_vector(yt);
	free_vector(k1);
	free_vector(k2);
	free_vector(k3);
	free_vector(k4);
}

double *vector(int n)
{
	double *v;
   
	v=(double *)calloc(n, sizeof(double));
	if (!v) fatal("allocation failure in vector()");

	return v;
}

void free_vector(double *v)
{
	free(v);
}

void dump_vector (double *v, int n)
{
	int i;
	for (i=0; i < n; i++)
		fprintf(stderr, "%.5f\t", v[i]);
	fprintf(stderr, "\n");
}	

int *ivector(int n)
{
	int *v;
   
	v = (int *)calloc(n, sizeof(int));
	if (!v) fatal("allocation failure in ivector()");

	return v;
}

void free_ivector(int *v)
{
	free(v);
}

void dump_ivector (int *v, int n)
{
	int i;
	for (i=0; i < n; i++)
		fprintf(stderr, "%d\t", v[i]);
	fprintf(stderr, "\n\n");
}

/* allocate a matrix with subscript range m[1...nr][1...nc] */
double **matrix(int nr, int nc)
{
	int i;
	double **m;

    	m = (double **) calloc (nr, sizeof(double *));
	assert(m != NULL);

	for (i = 0; i < nr; i++) {
       		m[i] = (double *)calloc(nc, sizeof(double));
		assert(m[i] != NULL);
	}

	return m;
}

void free_matrix(double **m, int nr)
{
	int i;
	for (i = 0; i < nr; i++)
		free(m[i]);
	free(m);
}

void dump_matrix (double **m, int nr, int nc)
{
	int i;
	for (i=0; i < nr; i++)
		dump_vector(m[i], nc);
	fprintf(stderr, "\n");
}	

void copy_matrix (double **dst, double **src, int nr, int nc)
{
	int i,j;
	for (i=0; i < nr; i++) 
		for (j=0; j < nc; j++)
			dst[i][j] = src[i][j];
}

int **imatrix(int nr, int nc)
{
	int i;
	int **m;

    	m = (int **) calloc (nr, sizeof(int *));
	assert(m != NULL);

    	for (i = 0; i < nr; i++) {
    		m[i] = (int *)calloc(nc, sizeof(int));
		assert(m[i] != NULL);
	}

	return m;
}

void free_imatrix(int **m, int nr)
{
	int i;
	for (i = 0; i < nr; i++)
		free(m[i]);
	free(m);
}

void dump_imatrix (int **m, int nr, int nc)
{
	int i;
	for (i=0; i < nr; i++)
		dump_ivector(m[i], nc);
	fprintf(stderr, "\n");
}	

/* matmult: C = AB, A, B are n x n square matrices	*/
void matmult(double **c, double **a, double **b, int n) 
{
	int i, j, k;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {
			c[i][j] = 0;
			for (k = 0; k < n; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
}

/* mult of an n x n matrix and an n x 1 column vector	*/
void matvectmult(double *vout, double **m, double *vin, int n)
{
	int i, j;
	for (i = 0; i < n; i++) {
		vout[i] = 0;
		for (j = 0; j < n; j++)
			vout[i] += m[i][j] * vin[j];
	}
}

/* INV = M^-1, INV, M are n by n matrices */
void matinv(double **INV, double **M, int n)
{
	int *p, i, j;
	double *col, *x;

	p = ivector(n);
	col = vector(n);
	x = vector(n);
	
	lupdcmp(M, n, p);

	for (j = 0; j < n; j++) {
		for (i = 0; i < n; i++) col[i]=0.0;
		col[j] = 1.0;
		lusolve(M, n, p, col, x);
		for (i = 0; i < n; i++) INV[i][j]=x[i];
	}

	free_ivector(p);
	free_vector(col);
	free_vector(x);
}
