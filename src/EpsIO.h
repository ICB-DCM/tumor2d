/*
 * EpsIO.h
 *
 *  Created on: Dec 4, 2009
 *      Author: jagiella
 */

#ifndef EPSIO_H_
#define EPSIO_H_

#include <fstream>

class EpsIO {
public:
	static void PSwriteHeader( std::fstream *fs, double minx, double miny, double maxx, double maxy);
	static void PSwriteSphere( std::fstream *fs, double x, double y, double radius, char *color);
	static void PSwriteLine( std::fstream *fs, double x1, double y1, double x2, double y2, double radius, char *color);
	static void PSwriteCircle( std::fstream *fs, double x, double y, double majorRadius, double minorRadius, char *color);
	static void PSwritePolygon( std::fstream *fs,
			double *x, double *y, int length,
			double radius, char *color);

	static void PSfillPolygon( std::fstream *fs,
			double *x, double *y, int length,
			double radius, char *color);
};

#endif /* EPSIO_H_ */
