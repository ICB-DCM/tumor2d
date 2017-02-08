#include "Montecarlo.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#define __MAIN__

int main( int argc, char **argv)
{
	std:srand(time(0));
	int rand_seed = rand();
	double InitialRadius = 12.0;
	double InitialQuiescentFraction = 0.75;
	double MaxCellDivisionRate = 0.0417;
	double ECMThresholdQuiescence = 0.010;
	double ECMProductionRate = 0.005;
	double ECMDegradationRate = 0.0008;
	double DivisionDepth = 100;
	double EndTime     = 1000;
	double OutputRate = 24;
	double profileTime  = 800/24;
	int profileDepth = 1000;
	char gc_tmpout[] =  { "outdir/rawOut2_gc.dat" };
	char ecm_tmpout[] =  { "outdir/rawOut2_ecm.dat" };
	char prolif_tmpout[] =  { "outdir/rawOut2_prolif.dat" };
	std::vector<double> gc_out;  
	std::vector<double> ecm_out;
	std::vector<double> prolif_out;
	gc_out.reserve((int)(EndTime / OutputRate));  
	ecm_out.reserve(profileDepth);
	prolif_out.reserve(profileDepth);
	double epsilon = montecarlo(InitialRadius, InitialQuiescentFraction, MaxCellDivisionRate, DivisionDepth,
							ECMThresholdQuiescence, ECMProductionRate, ECMDegradationRate,
							EndTime, OutputRate, profileTime, profileDepth, rand_seed,
							gc_out, ecm_out, prolif_out);
	fprintf(stdout, "%e\n", epsilon);
	FILE *fp_raw = fopen( ecm_tmpout, "w");
	for( int j=0; j<profileDepth; j++){
		fprintf( fp_raw, "%i %e \n", j, ecm_out[j]);
	}
	fclose(fp_raw);
	
	// Dennis proliferation
	fp_raw = fopen(prolif_tmpout, "w");
	for( int j=0; j<profileDepth; j++){
		fprintf( fp_raw, "%i %e\n", j, prolif_out[j]);
	}
	fclose(fp_raw);
	
	
	fp_raw = fopen( gc_tmpout, "a+");
	for (int i = 0; i < (int)(EndTime / OutputRate)  ;i++) {
		fprintf(fp_raw, "%e %e\n", (i+1)*OutputRate,  gc_out[i]);
	}
	fclose(fp_raw);
	return 0;
}


