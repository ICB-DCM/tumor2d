/*
 * EpsIO.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: jagiella
 */

#include "EpsIO.h"



void EpsIO::PSwriteHeader(std::fstream *fs, double minx, double maxx, double miny,	double maxy) {
	//printf( "\%%!PS-Adobe-3.0 EPSF-3.0\n");
	(*fs) << "%!PS-Adobe-3.0 EPSF-3.0\n";
	(*fs) << "%%BoundingBox: "<<minx<<" "<<miny<<" "<<maxx<<" "<<maxy<<" "<<"\n";
}

void EpsIO::PSwriteSphere(std::fstream *fs, double x, double y, double radius,
		char *color) {
	//printf( "sphere { <%lf, %lf>, %lf texture {pigment { color %s }}}\n", x, y, radius, color);
	//(*fs) << "sphere { <"<<x<<","<<y<<">, "<<radius<<" texture {pigment { color "<<color<<" }}}\n";
}

void EpsIO::PSwriteLine(std::fstream *fs, double x1, double y1, double x2, double y2,
		double radius, char *color) {
	//printf( "0.01 setlinewidth\n %s setrgbcolor\n newpath %lf %lf moveto %lf %lf lineto stroke\n", color, x1, y1, x2, y2);
	(*fs) << "newpath\n";
	(*fs) << " 0.01 setlinewidth\n ";
	(*fs) << x1 << " " << y1 << " moveto\n ";
	(*fs) << x2 << " " << y2 << " lineto\n ";
	(*fs) << color << "\n";
	(*fs) << "stroke\n";
}

void EpsIO::PSwritePolygon( std::fstream *fs,
		double *x, double *y, int length,
		double radius, char *color) {

	(*fs) << "newpath\n";
	(*fs) << "0.01 setlinewidth\n ";
	(*fs) << " "<<x[0]<<" "<<y[0]<<" moveto\n";

	for(int i=1; i<length; i++)
	(*fs) << " "<<x[i]<<" "<<y[i]<<" lineto\n";

	(*fs) << " closepath\n " << color << "\n";
}

void EpsIO::PSfillPolygon( std::fstream *fs,
		double *x, double *y, int length,
		double radius, char *color) {

	(*fs) << "newpath\n";
	(*fs) << " "<<x[0]<<" "<<y[0]<<" moveto\n";

	for(int i=1; i<length; i++)
	(*fs) << " "<<x[i]<<" "<<y[i]<<" lineto\n";

	(*fs) << " closepath\n " << color << "\n fill\n";
}

void EpsIO::PSwriteCircle(std::fstream *fs, double x, double y, double majorRadius,
		double minorRadius, char *color) {
	//printf( "0.01 setlinewidth\n %s setrgbcolor %lf %lf %lf 0 360 arc stroke\n", color, x, y, majorRadius);
	(*fs) << "0.01 setlinewidth\n ";
	(*fs) << color << "\n ";
	(*fs) << x << " "<< y << " " << majorRadius << " 0 360 arc stroke\n";
	//torus {1.0,0.25 scale <1,1,1> rotate<0,0,0> translate<0,0,0>
}

