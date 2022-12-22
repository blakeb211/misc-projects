/* NRUTIL: from "Numerical Recipes in C", pp. 705-709 */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/* nrerror() := Numerical Recipes standard error handler */
void nrerror(char *error_text)
{
	fprintf(stderr, "Numerical Recipes run-time error...\n");
	fprintf(stderr, "%s\n", error_text);
	fprintf(stderr, "...now exiting to system...\n");
	exit(1);
}

/* *vector() := Allocates a float vector with range [nl..nh] */
float *vector(int nl, int nh)
{
	float *v;

	v = (float *) malloc( (unsigned)(nh-nl+1) * sizeof(float) );
	if (!v)
		nrerror("\nallocation failure in vector()");
	return v-nl;
}

float Square(float x)
{
	return x * x;
}

int *ivector(int nl, int nh)
{
	int *v;
	v = (int *)malloc((unsigned) (nh-nl+1)*sizeof(int));
	if (!v) nrerror("\nallocation failure in ivector()");
	return v-nl;
}

long *lvector(int nl, int nh)
{
	long *v;
	v = (long *)malloc((unsigned) (nh-nl+1)*sizeof(long));
	if (!v) nrerror("\nallocation failure in ivector()");
	return v-nl;
}

/* *dvector() := Allocates a double vector with range [nl..nh] */
double *dvector(int nl, int nh)
{
	double *v;

	v = (double *) malloc( (unsigned)(nh-nl+1) * sizeof(double) );
	if (!v)
		nrerror("allocation failure in dvector()");
	return v-nl;
}

double **dmatrix(int nrl, int nrh, int ncl, int nch)
{
	int i;
	double **m;

	m = (double **) malloc( (unsigned)(nrh - nrl + 1) * sizeof(double *) );
	if (!m)
		nrerror("allocation failure 1 in dmatrix()");
	m -= nrl;

	for (i = nrl; i <= nrh; i++)
	{
		m[i] = (double *) malloc( (unsigned)(nch - ncl + 1)*sizeof(double) );
		if (!m[i])
			nrerror("allocation failure 2 in dmatrix()");
		m[i] -= ncl;
	}
	return m;
}

float **matrix(int nrl, int nrh, int ncl, int nch)
{
	int i;
	float **m;

	m = (float **) malloc( (unsigned)(nrh - nrl + 1) * sizeof(float *) );
	if (!m)
		nrerror("allocation failure 1 in matrix()");
	m -= nrl;

	for (i = nrl; i <= nrh; i++)
	{
		m[i] = (float *) malloc( (unsigned)(nch - ncl + 1)*sizeof(float) );
		if (!m[i])
			nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}

/* free_vector() := Frees a float vector allocated by vector() */
void free_vector(float *v, int nl, int nh)
{
	free( (char *)(v + nl) );
}

void free_ivector(int *v, int nl, int nh)
{
	free( (char *)(v + nl) );
}

void free_lvector(long *v, int nl, int nh)
{
	free( (char *)(v + nl) );
}

void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch)
{
	int i;

	for (i = nrh; i >= nrl; i--)
		free( (char *)(m[i] + ncl) );
	free( (char *)(m + nrl) );
}

/* free_dvector() := Frees a double vector allocated by dvector() */
void free_dvector(double *v, int nl, int nh)
{
	free( (char *)(v + nl) );
}

void free_matrix(float **m, int nrl, int nrh, int ncl, int nch)
{
	int i;

	for (i = nrh; i >= nrl; i--)
		free( (char *)(m[i] + ncl) );
	free( (char *)(m + nrl) );
}

