#include <math.h>
#include <stdio.h>
#include "Mathematix.h"
#include "VesselNetwork.h"
#include "VoronoiDiagramExtended.h"
#include "CellProcesses.h"
//#include "../couple.h"


//double DistanceToInitialCell = 10.;
//int CountVessel = 1;
//double BranchingProbability = 0.00;
//int BranchingLength = 20;

void printToPovray( const char *filename, VoronoiDiagram *voronoiDiagram, double angle, double setoffX, double setoffY, double setoffZ)
{
	int i, j;

	FILE* fp;

	fp = fopen( filename, "w+" );

	//fprintf( stderr, "INFO: write to file %s\n", filename);

	// PRINT GRID TO POV-FILE
	/*fprintf( fp, "#include \"finish.inc\"\n"\
				"#include \"colors.inc\"\n"\
				"#include \"textures.inc\"\n"\
				"background { color White }\n"\
				"camera {location <%lf, 30, %lf> look_at  <30, 30, 30>}\n"\
				"light_source {<%lf, 30, %lf> color rgb <1, 1, 1>}\n", 30+sin(angle)*90, 30+cos(angle)*90, 30+sin(angle)*60, 30+cos(angle)*60);
	*/
	printf( "DIMENSIONS=%i\n", DIMENSIONS);

	// PRINT HEADER
	fprintf( fp, "#include \"finish.inc\"\n"\
				"#include \"colors.inc\"\n"\
				"#include \"textures.inc\"\n"\
				"background { color White }\n"\
				"camera {location <%lf, %lf, %lf> look_at  <%lf, %lf, %lf>}\n", 
#if DIMENSIONS == 3
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])*1.5, (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2., 
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.);
#endif
#if DIMENSIONS == 2
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMax[1]-voronoiDiagram->xMin[1])*1.,
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., 0.);
#endif

	// PRINT "PETRIDISH"
	if( false){
#if DIMENSIONS == 3
	fprintf( fp, "sphere {< %lf, %lf, %lf>,%lf texture {pigment { color Blue transmit 0.9} finish {phong 0.2}}}\n",
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.,
				1.4*(voronoiDiagram->xMax[0]-voronoiDiagram->xMin[0])/2.);
#endif
#if DIMENSIONS == 2
	fprintf( fp, "sphere {< %lf, %lf, %lf>,%lf texture {pigment { color Blue transmit 0.9} finish {phong 0.2}}}\n",
				(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., 0.,
				1.*(voronoiDiagram->xMax[0]-voronoiDiagram->xMin[0])/2.);

#endif
	}

	// AMBIENT LIGHT
	fprintf( fp, "global_settings { ambient_light max_trace_level 100 }\n");

	// LIGHT SOURCES
	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])*1.5, (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.);
	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            -(voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])*1.5, (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.);

/*	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])*1.5, (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.);
	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., -(voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])*1.5, (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.);
	
	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])*1.5);
	fprintf( fp,"light_source {<%lf, %lf, %lf> color rgb <1, 1, 1>}\n", 
	            (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2., (voronoiDiagram->xMin[1]+voronoiDiagram->xMax[1])/2., -(voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])*1.5);
*/	
	//center of mass
	int count_cells = 0;
	double center_x = 0.;
	double center_z = 0.;
	for( i=0; i<voronoiDiagram->countVoronoiCells; i++){
		if( voronoiDiagram->voronoiCells[i]->getState() == NECROTIC || voronoiDiagram->voronoiCells[i]->getState() == ACTIVE || voronoiDiagram->voronoiCells[i]->getState() == NONACTIVE){
			center_x += voronoiDiagram->voronoiCells[i]->position[0];
			center_z += voronoiDiagram->voronoiCells[i]->position[2];
			count_cells++;
		}
	}
	center_z /= (double)count_cells;
	fprintf( stderr, "count_cells=%i\n", count_cells);

	// all points
	for( i=0; i<voronoiDiagram->countVoronoiCells; i++){
	//	double dist2Center = sqrt( pow( voronoiDiagram->voronoiCells[i]->position[0] - 30, 2.) + pow( voronoiDiagram->voronoiCells[i]->position[2]-30, 2.));
	/*if( sin( 
			( voronoiDiagram->voronoiCells[i]->position[0] - 30 < 0 
			  ? -acos((voronoiDiagram->voronoiCells[i]->position[2]-30)/dist2Center) 
			  : acos((voronoiDiagram->voronoiCells[i]->position[2]-30)/dist2Center) 
			) - angle
		) * dist2Center + 30<=setoffX ||
		voronoiDiagram->voronoiCells[i]->position[1]<=setoffY ||
		cos( ( voronoiDiagram->voronoiCells[i]->position[0] - 30 < 0 
		       ? -acos((voronoiDiagram->voronoiCells[i]->position[2]-30)/dist2Center) 
		       : acos((voronoiDiagram->voronoiCells[i]->position[2]-30)/dist2Center) ) - angle) * dist2Center + 30<=setoffZ)*/


	/* CUT QUARTER OF DOMAIN!!!
	if(	(int)(voronoiDiagram->voronoiCells[i]->position[0]) < (int)((voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2.+1.) 
	  ||(int)(voronoiDiagram->voronoiCells[i]->position[2]) < (int)((voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2.+1.)
	  )*/
	{
		
	if( voronoiDiagram->voronoiCells[i]->getState() == COMPARTMENT){
			fprintf( fp, "sphere {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %.3lf", voronoiDiagram->voronoiCells[i]->position[j]);
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">, 0.62 ");
			//fprintf( fp, ">,0.62 texture {pigment { color Blue }}}\n");
			/*fprintf( fp, "box {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %i", (int)(voronoiDiagram->voronoiCells[i]->position[j]));
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">,<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %i", (int)(voronoiDiagram->voronoiCells[i]->position[j] + 1.));
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, "> ");*/
			fprintf( fp, "texture {pigment { color rgbt< %.3lf, %.3lf, %.3lf, %.3lf> }}}\n",
			             (GetAgent(voronoiDiagram->voronoiCells[i])->isGrowing() /*cellCount == GetAgent(voronoiDiagram->voronoiCells[i])->maxCellCount*/ ? 0. : 1.),
			             1.,
			             0.,
			             1. - (double)GetAgent(voronoiDiagram->voronoiCells[i])->cellCount/GetAgent(voronoiDiagram->voronoiCells[i])->maxCellCount);
			//fprintf( fp, ">, texture {pigment { color Tan }}}\n");
			//fprintf( fp, "> texture {pigment { color rgbt< %.3lf, %.3lf, %.3lf, %.3lf> }}}\n", 229./256, 198./256, 163./256, 1. - (double)GetAgent(voronoiDiagram->voronoiCells[i])->cellCount/GetAgent(voronoiDiagram->voronoiCells[i])->maxCellCount);
			//fprintf( fp, "> color rgbt< %.3lf, %.3lf, %.3lf, %.3lf> }\n", 229./256, 198./256, 163./256, 1. - (double)GetAgent(voronoiDiagram->voronoiCells[i])->cellCount/GetAgent(voronoiDiagram->voronoiCells[i])->maxCellCount);

	}else{
		// neighbors
		/*for( int n=0; n<voronoiDiagram->voronoiCells[i]->countNeighborCells; n++)
		if( voronoiDiagram->voronoiCells[i]->index < voronoiDiagram->voronoiCells[i]->neighborCells[n]->index)
		{
			fprintf( fp, "cylinder{<%lf,%lf,%lf>, <%lf,%lf,%lf>, %lf}\n",
					voronoiDiagram->voronoiCells[i]->position[0],
					voronoiDiagram->voronoiCells[i]->position[1],
					voronoiDiagram->voronoiCells[i]->position[2],
					voronoiDiagram->voronoiCells[i]->neighborCells[n]->position[0],
					voronoiDiagram->voronoiCells[i]->neighborCells[n]->position[1],
					voronoiDiagram->voronoiCells[i]->neighborCells[n]->position[2],
					0.1);
		}*/

		// cells
		if( voronoiDiagram->voronoiCells[i]->getState() == NECROTIC ){
			//fprintf( stderr, "INFO: active cell found! write to file %s\n", filename);
			fprintf( fp, "sphere {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %.3lf", voronoiDiagram->voronoiCells[i]->position[j]);
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">,0.62 texture {pigment { color Blue }}}\n");
		}
		if( voronoiDiagram->voronoiCells[i]->getState() == ACTIVE ){
			//fprintf( stderr, "INFO: active cell found! write to file %s\n", filename);
			fprintf( fp, "sphere {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %.3lf", voronoiDiagram->voronoiCells[i]->position[j]);
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">,0.62 texture {pigment { color Flesh transmit 0.3} finish {phong 0.3}}}\n");
			//fprintf( fp, ">,0.62 texture {pigment { color Green }}}\n");
		}
		if( voronoiDiagram->voronoiCells[i]->getState() == NONACTIVE ){
			//fprintf( stderr, "INFO: active cell found! write to file %s\n", filename);
			fprintf( fp, "sphere {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %.3lf", voronoiDiagram->voronoiCells[i]->position[j]);
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">,0.62 texture {pigment { color Yellow }}}\n");
		}
	}	
	}
		
		// vessel nodes
		if( voronoiDiagram->voronoiCells[i]->getState() == VESSEL ){
			fprintf( fp, "sphere {<");
			for( j=0; j<DIMENSIONS; j++){
				fprintf( fp, " %.3lf", voronoiDiagram->voronoiCells[i]->position[j]);
				if( j+1!=DIMENSIONS)
					fprintf( fp, ",");
			}
			fprintf( fp, ">,0.62 texture {pigment { color Red }}}\n");
		}
		if( voronoiDiagram->voronoiCells[i]->getState() == VESSEL ){
			// vessel segments
			for( j=0; j<voronoiDiagram->voronoiCells[i]->countNeighborCells; j++){
				if( voronoiDiagram->voronoiCells[i]->neighborCells[j]->getState() == VESSEL){
					fprintf( fp, "  cylinder {<%lf, %lf, %lf>, <%lf, %lf, %lf>, 0.62 pigment { color %s }}\n",
	    				voronoiDiagram->voronoiCells[i]->position[0], 
						voronoiDiagram->voronoiCells[i]->position[1], 
						voronoiDiagram->voronoiCells[i]->position[2], 
						voronoiDiagram->voronoiCells[i]->neighborCells[j]->position[0], 
						voronoiDiagram->voronoiCells[i]->neighborCells[j]->position[1], 
						voronoiDiagram->voronoiCells[i]->neighborCells[j]->position[2], 
						"Red\n");
				}
			}
		}
	}
	fclose( fp);

	char filename2[512];
	sprintf(filename2, "%s.morphogen.dat", filename);
	fp = fopen( filename2, "w+" );
	for( i=0; i<voronoiDiagram->countVoronoiCells; i++){
		if( (int)voronoiDiagram->voronoiCells[i]->position[1] == 0)
			fprintf( fp, "\n");
		fprintf( fp, "%lf %lf %e\n",
				voronoiDiagram->voronoiCells[i]->position[0],
				voronoiDiagram->voronoiCells[i]->position[1],
				voronoiDiagram->voronoiCells[i]->morphogen);
	}
	fclose( fp);
}
/*****************************************************************************/



void SetInitialVesselNetwork( VoronoiDiagram *voronoiDiagram, AgentList* agentArray, ActionTree *probTree, VoronoiCell *initialCell, double distanceToInitialCell, int countVessel, double branchingProbability, int branchingLength){
	
	int i;

	// place vessels around initial cell
	double angleBetweenVessels = 2. * PI/(double)countVessel;
	double tempAngle = 0.;
/*
	for( i=0; i<countVessel; i++){
		SetVessel( voronoiDiagram, probTree,
		           GetClosestGridPoint( voronoiDiagram, (xminDOMAIN+xmaxDOMAIN)/2. + cos(tempAngle)*INIT_DISTANCE_TUMOR_VESSEL, 
		                                             yminDOMAIN, 
		                                             (zminDOMAIN+zmaxDOMAIN)/2. + sin(tempAngle)*INIT_DISTANCE_TUMOR_VESSEL), 
		           GetClosestGridPoint( voronoiDiagram, (xminDOMAIN+xmaxDOMAIN)/2. + cos(tempAngle)*INIT_DISTANCE_TUMOR_VESSEL, 
		                                             ymaxDOMAIN, 
		                                             (zminDOMAIN+zmaxDOMAIN)/2. + sin(tempAngle)*INIT_DISTANCE_TUMOR_VESSEL),
		           branchingProbability, branchingLength);
		tempAngle += angleBetweenVessels;*/
	
	if( !voronoiDiagram->domainSet)
		voronoiDiagram->setDomain();
		//setDomain( voronoiDiagram);
	
	for( i=0; i<countVessel; i++){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2. + cos(tempAngle)*distanceToInitialCell, 
		                                             voronoiDiagram->xMin[1], 
		                                             (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2. + sin(tempAngle)*distanceToInitialCell), 
		           GetClosestGridPoint( voronoiDiagram, (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])/2. + cos(tempAngle)*distanceToInitialCell, 
		                                             voronoiDiagram->xMax[1], 
		                                             (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])/2. + sin(tempAngle)*distanceToInitialCell),
		           branchingProbability, branchingLength);
		tempAngle += angleBetweenVessels;

	}
}
/*****************************************************************************/


void SetRegularInitialVesselNetwork( VoronoiDiagram *voronoiDiagram, AgentList* agentArray, ActionTree *probTree, VoronoiCell *initialCell, double distanceBetweenVessels[DIMENSIONS], double branchingProbability, int branchingLength){
	
	//int i, ii;

	// place vessels around initial cell
	//double angleBetweenVessels = 2. * PI/(double)countVessel;
	//double tempAngle = 0.;
	double vesselOffSet[DIMENSIONS];
	
	for( int i=0; i<DIMENSIONS; i++)
		{
		//this provoque a bug in my angiogenesis function that I am not able to understand.
		//if( i==0)
		//vesselOffSet[i] = (voronoiDiagram->xMax[i]-voronoiDiagram->xMin[i])*0.5+distanceBetweenVessels[i] * myRand();
		//else
		vesselOffSet[i] = distanceBetweenVessels[i] * myRand();
		
		//vesselOffSet[i] = 0.;
		#ifdef __myDEBUG__
		vesselOffSet[i] = 0.;
		#endif
		}
	if( !voronoiDiagram->domainSet)
		voronoiDiagram->setDomain();
	
	// vessels in z-direction
	/*for( double x = voronoiDiagram->xMin[0]+vesselOffSet[0]; x < voronoiDiagram->xMax[0]; x+=distanceBetweenVessels[0])
	for( double y = voronoiDiagram->xMin[1]+vesselOffSet[1]; y < voronoiDiagram->xMax[1]; y+=distanceBetweenVessels[1]){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x, y, voronoiDiagram->xMin[2]), 
		           GetClosestGridPoint( voronoiDiagram, x, y, voronoiDiagram->xMax[2]),
		           branchingProbability, branchingLength);
		//tempAngle += angleBetweenVessels;

	}*/

	// vessels in y-direction
	/*for( double x = voronoiDiagram->xMin[0]+vesselOffSet[0]; x < voronoiDiagram->xMax[0]; x+=distanceBetweenVessels[0])
	for( double z = voronoiDiagram->xMin[2]+vesselOffSet[2]; z < voronoiDiagram->xMax[2]; z+=distanceBetweenVessels[2]){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x, voronoiDiagram->xMin[1], z), 
		           GetClosestGridPoint( voronoiDiagram, x, voronoiDiagram->xMax[1], z),
		           branchingProbability, branchingLength);
		//tempAngle += angleBetweenVessels;

	}*/

	// vessels in x-direction
	/*for( double y = voronoiDiagram->xMin[1]+vesselOffSet[1]; y < voronoiDiagram->xMax[1]; y+=distanceBetweenVessels[1])
	for( double z = voronoiDiagram->xMin[2]+vesselOffSet[2]; z < voronoiDiagram->xMax[2]; z+=distanceBetweenVessels[2]){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, (voronoiDiagram->xMax[0]+voronoiDiagram->xMin[0])*0.5, y, z), 
		           GetClosestGridPoint( voronoiDiagram, voronoiDiagram->xMax[0], y, z),
		           branchingProbability, branchingLength);
		//tempAngle += angleBetweenVessels;

	}*/
	
	for( double x = voronoiDiagram->xMin[0]+vesselOffSet[0]; x < voronoiDiagram->xMax[0]; x+=distanceBetweenVessels[0])
	for( double y = voronoiDiagram->xMin[1]+vesselOffSet[1]; y < voronoiDiagram->xMax[1]; y+=distanceBetweenVessels[1])
	for( double z = voronoiDiagram->xMin[2]+vesselOffSet[2]; z < voronoiDiagram->xMax[2]; z+=distanceBetweenVessels[2]){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x,                           y, z), 
		           GetClosestGridPoint( voronoiDiagram, x+distanceBetweenVessels[0], y, z),
		           branchingProbability, branchingLength);
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x, y                          , z), 
		           GetClosestGridPoint( voronoiDiagram, x, y+distanceBetweenVessels[1], z),
		           branchingProbability, branchingLength);
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x, y, z                          ), 
		           GetClosestGridPoint( voronoiDiagram, x, y, z+distanceBetweenVessels[2]),
		           branchingProbability, branchingLength);
		//tempAngle += angleBetweenVessels;

	}
	/* // THIN BUNDLE
	for( double x = (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])*0.5-4.-8.; x < (voronoiDiagram->xMin[0]+voronoiDiagram->xMax[0])*0.5+4.-8.; x+=4.)
	for( double z = (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])*0.5-4.; z < (voronoiDiagram->xMin[2]+voronoiDiagram->xMax[2])*0.5+4.; z+=4.){
		SetVessel( voronoiDiagram, agentArray, probTree,
		           GetClosestGridPoint( voronoiDiagram, x, voronoiDiagram->xMin[1], z), 
		           GetClosestGridPoint( voronoiDiagram, x, voronoiDiagram->xMax[1], z),
		           branchingProbability, branchingLength);
		//tempAngle += angleBetweenVessels;

	}*/
}
/*****************************************************************************/


VoronoiCell *GetClosestGridPoint( VoronoiDiagram *voronoiDiagram, double x, double y, double z){

	int i;
	double tempDist,
	       minDist = sqrt( pow( voronoiDiagram->voronoiCells[0]->position[0] - x, 2.) + pow( voronoiDiagram->voronoiCells[0]->position[1] - y, 2.) + pow( voronoiDiagram->voronoiCells[0]->position[2] - z, 2.)); 
        VoronoiCell *startPoint,
	     *nextPoint = voronoiDiagram->voronoiCells[0];

	do{
		startPoint = nextPoint;
		for( i=0; i<startPoint->countNeighborCells; i++){
			tempDist = sqrt( pow( startPoint->neighborCells[i]->position[0] - x, 2.) + pow( startPoint->neighborCells[i]->position[1] - y, 2.) + pow( startPoint->neighborCells[i]->position[2] - z, 2.));             
			if( minDist > tempDist){
				minDist = tempDist;
 				nextPoint = startPoint->neighborCells[i];
			}
		}
	}while( startPoint != nextPoint);

	return startPoint;
}
/*****************************************************************************/



void SetVessel( VoronoiDiagram *voronoiDiagram, AgentList* agentArray, ActionTree *probTree, VoronoiCell *startCell, VoronoiCell *endCell, double branchingProbability, int branchingLength){

		// set vessel cells
		//fprintf( stderr, "INFO: set vessel cells\n");
		int i;
		VoronoiCell *nextCell = startCell;
		double minDist, tempDist;
		do{
			// Main Vessel
			startCell = nextCell;
			//fprintf( stderr, "INFO: Turn cell %i into a vessel segment!\n", startCell->nr);
			if( startCell->getState() == FREE)
				addedVesselAndUpdateSurrounding( probTree, agentArray, startCell, voronoiDiagram);
			//else
				//fprintf( stderr, "INFO: cell %i is already declared as %s!\n", startCell->index, cellTypeToString(startCell->getState()));
			
			minDist = sqrt( pow( startCell->position[0] - endCell->position[0], 2.) + pow( startCell->position[1] - endCell->position[1], 2.) + pow( startCell->position[2] - endCell->position[2], 2.));
			// look for best neighbor
			for( i=0; i<startCell->countNeighborCells; i++){
				//if( startCell->neighborCells[i]->getState() != VESSEL)
				{
					tempDist = sqrt( pow( startCell->neighborCells[i]->position[0] - endCell->position[0], 2.) 
					               + pow( startCell->neighborCells[i]->position[1] - endCell->position[1], 2.) 
					               + pow( startCell->neighborCells[i]->position[2] - endCell->position[2], 2.)); 
					double neighborDist = sqrt( pow( startCell->neighborCells[i]->position[0] - startCell->position[0], 2.) 
						                  + pow( startCell->neighborCells[i]->position[1] - startCell->position[1], 2.) 
						                  + pow( startCell->neighborCells[i]->position[2] - startCell->position[2], 2.)); 
					            
					if( minDist > tempDist && neighborDist < 2.){
						minDist = tempDist;
						nextCell = startCell->neighborCells[i];
					}
				}
			}

			// Branching / Sub-vessels
			if(myRand() < branchingProbability){
				//printf("INFO:  Branching!\n");
				MakeRandomBranch( voronoiDiagram, agentArray, probTree, startCell, branchingLength);
				//printf("INFO:  Branching ended!\n");
			}
		}while( nextCell != startCell ); 
}
/*****************************************************************************/


void MakeRandomBranch( VoronoiDiagram *voronoiDiagram, AgentList* agentArray, ActionTree *probTree, VoronoiCell *startCell, int branchingLength){

	int i, ii;

	if( branchingLength != 0){
		// chose next vessel segment
		int countPossibleCandidates = 0;
		VoronoiCell *possibleCandidates[MAX_NNS];
		for( i=0; i<startCell->countNeighborCells; i++){ // for each candidate
			VoronoiCell *nextCell = startCell->neighborCells[i];	
			if( nextCell->getState()!=VESSEL){ // check that is not already a vessel
				int valid = 1;
				for( ii=0; ii<nextCell->countNeighborCells; ii++){ // for all neighbors of candidate
					if( nextCell->neighborCells[ii]->getState()==VESSEL && nextCell->neighborCells[ii]!=startCell) // check that no contact to vessels
						valid = 0;
				}
				if( valid){
					possibleCandidates[ countPossibleCandidates] = nextCell;
					countPossibleCandidates++;
				}
			}
		}

		i = (int)(myRand() * (double)countPossibleCandidates);
		if( countPossibleCandidates!=0){
		//double probToBeChoosen = 1./(double)countPossibleCandidates;
		//for( i=0; i<countPossibleCandidates; i++){
			//if( myRand()<=probToBeChoosen){
				// next segment choosen
				VoronoiCell *nextCell = possibleCandidates[ i];
				//fprintf( stderr, "INFO: Turn cell %i into a vessel segment!\n", nextCell->nr);
				addedVesselAndUpdateSurrounding( probTree, agentArray, nextCell, voronoiDiagram);
				
				//fprintf( stderr, "INFO: Turn cell %i into a vessel segment!\n", startCell->nr);
				//printf( "INFO: Subbranching: %lf < %lf < %lf, %lf < %lf < %lf, %lf < %lf < %lf\n", 
				//        xminDOMAIN, nextCell->position[0], xmaxDOMAIN, yminDOMAIN, nextCell->position[1], ymaxDOMAIN, zminDOMAIN, nextCell->position[2], zmaxDOMAIN);
				double BoundaryLayer = 1.;
				if( nextCell->position[0] > voronoiDiagram->xMin[0]+BoundaryLayer && nextCell->position[0] < voronoiDiagram->xMax[0]-BoundaryLayer &&
				    nextCell->position[1] > voronoiDiagram->xMin[1]+BoundaryLayer && nextCell->position[1] < voronoiDiagram->xMax[1]-BoundaryLayer &&
				    nextCell->position[2] > voronoiDiagram->xMin[2]+BoundaryLayer && nextCell->position[2] < voronoiDiagram->xMax[2]-BoundaryLayer){

					// continue this branche
					//printf("INFO: Subbranching\n");
					MakeRandomBranch( voronoiDiagram, agentArray, probTree, nextCell, branchingLength-1);
				
				}
				return;
		//	}
		}
	}

	return;		
}

