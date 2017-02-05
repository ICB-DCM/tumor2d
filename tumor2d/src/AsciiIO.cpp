/*
 * AsciiIO.cpp
 *
 *  Created on: Dec 5, 2014
 *      Author: jagiella
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strcpy
#include <math.h> // pow


#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#include "statistics.hpp"
#include "matrix.hpp"
#include "AsciiIO.h"

comparison_t create_comparison(){
	comparison_t c;
	c.size = c.dim = 0;
	c.x = c.m = c.s = 0;
	c.y = 0;
	return c;
}

int readFile( const char* filename, double **&cols, int ncol)
{
	//printf( "[ %i ]\n", sizeof(cols));
	if( !cols){
		cols = (double**)realloc( cols, sizeof(double*)*ncol);
		for( int i=0; i<ncol; i++)
			cols[i] = 0;
	}
	int nrow = 0;

	FILE *fp = fopen( filename, "r");
	char buffer[1024], *ptr;

	while( fgets( buffer, 1024, fp ))if(buffer[0] != '#'){
		ptr = buffer;
		nrow++;

		for( int i=0; i<ncol; i++){
			cols[i] = (double*)realloc( cols[i], sizeof(double)*nrow);
			cols[i][nrow-1]  = strtof( ptr, &ptr) ;
		}
	}
	fclose(fp);

	return nrow;
}
int readFileColumn( const char* filename, double *&col, int icol)
{
	int nrow = 0;

	FILE *fp = fopen( filename, "r");
	char buffer[1024], *ptr;

	while( fgets( buffer, 1024, fp ))
	if(buffer[0] != '#'){
		ptr = buffer;
		nrow++;

		for( int i=0; i<icol; i++)
			strtof( ptr, &ptr) ;

		col = (double*)realloc( col, sizeof(double)*nrow);
		col[nrow-1]  = strtof( ptr, &ptr) ;
	}
	fclose(fp);

	return nrow;
}

int readFileColumn( char** filename, int n, double **&col, int icol)
{
	int nrow = 0;
	double **old=col;
	col = (double**)realloc( col, sizeof(double*)*n);
	for( int i=0; i<n; i++){
		if( old != col) col[i] = 0;
		nrow = readFileColumn( filename[i], col[i], icol);
	}

	return nrow;
}

#define inrange( a, x, b) ( a<=x && x<=b )

void interpolate( double *xi, double *yi, int ni, double *xo, double *yo, int no){

	// init
	gsl_interp_accel *acc = gsl_interp_accel_alloc ();
	gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, ni);
	gsl_spline_init (spline, xi, yi, ni);

	// interpolate
	//fprintf(stderr, "interpolate\n");
	for( int i=0; i<no; i++ ) // for all data points
	if( xo[i] >= xi[0] && xo[i] <= xi[ni-1] )
		// interpolate
		yo[i] = gsl_spline_eval (spline, xo[i], acc);
	else
		yo[i] = 1./0.;

	// free
	gsl_spline_free (spline);
	gsl_interp_accel_free (acc);

}


double compare( comparison_t d1, comparison_t d2, char mode ){
	double negloglik = 0;

	if( d1.m == 0){
		d1.m = (double*)malloc( d1.dim * sizeof(double));
		mean( d1.y, d1.size, d1.dim, d1.m, 2);
	}

	switch( mode){
	case mean_vs_mean:{
		// extract mean if necessary
		if( d2.m == 0){
			d2.m = (double*)malloc( d2.dim * sizeof(double));
			mean( d2.y, d2.size, d2.dim, d2.m, 2);
		}

		// interpolate
		double d2_m[ d1.dim];
		interpolate(
				d2.x, d2.m, d2.dim, // map d2.m onto d1.x  |
				d1.x, d2_m, d1.dim);//                     v

		// compare
		for( int i=0; i<d1.dim; i++ ) // for all data points
			if( inrange( d2.x[0], d1.x[i], d2.x[d2.dim-1]) && d1.s[i]>0)
			negloglik += 0.5 * pow( (d1.m[i] - d2_m[i])/d1.s[i], 2);
	}break;

	case mean_vs_single:{
		double d2_y[ d1.dim];


		for( int j=0; j<d2.size; j++){
		interpolate(
				d2.x, d2.y[j], d2.dim, // map d2.y onto d1.x  |
				d1.x, d2_y,    d1.dim);//                     v


		for( int i=0; i<d1.dim; i++ ) // for all data points
			if( inrange( d2.x[0], d1.x[i], d2.x[d2.dim-1])  && d1.s[i]>0)
			negloglik += 0.5 * pow( (d1.m[i] - d2_y[i])/d1.s[i], 2);
		}
	}break;
	}

	return negloglik;
}
