#include "Substrate.h"

#include <string.h>

Molecule Substrate::Oxygen;
Molecule Substrate::Glucose;
Molecule Substrate::Growthfactors;
Molecule Substrate::Lactate;
const double DataRecord::TimeFrequency = 3;

double ANGIOGENESIS_GROWTHFACTOR_THRESHOLD = 0.01;

double xminDOMAIN = 0.;
double yminDOMAIN = 0.;
double zminDOMAIN = 0.;
double xmaxDOMAIN = 0.;
double ymaxDOMAIN = 0.;
double zmaxDOMAIN = 0.;
double BoundaryLayer = 1.;	

double Oxygen_Diffusion=(6300000); //(6300000)
double Glucose_Diffusion=(378000); // (378000)
//double Lactate_Diffusion=813600; // \mu m^2 / s [SIDELL et al. 1987: Lactate Diffusion in Muscle Cytosol, T=25^C]
double Lactate_Diffusion=756000; // \mu m^2 / s [Rong et al., xenograft ]
double Waste_Diffusion=7560;

#ifndef __WithGUI__
Substrate::Substrate(int* sizeofaxes, int type, VoronoiDiagram *voronoiDiagram, AgentList *agentList, double Init_Oxy, double Init_Glu, double Work_Parameter, const char *thedirname, char *boundaryCondition, double customTimestep)
#else
Substrate::Substrate(int* sizeofaxes, int type, VoronoiDiagram *voronoiDiagram, AgentList *agentList, double Init_Oxy, double Init_Glu, double Work_Parameter, const char *thedirname, char *boundaryCondition, double customTimestep, Window *thegui)
#endif
	{

		

	#ifdef __myOMP__

	//openMp Test
	int nthreads, tid;

	/* Fork a team of threads with each thread having a private tid variable */
	#pragma omp parallel private(tid)
	{
	
		/* Obtain and print thread id */
		tid = omp_get_thread_num();
		printf("Test openMP: Hello World from thread = %d\n", tid);
		
		/* Only master thread does this */
		if (tid == 0) 
			{
			nthreads = omp_get_num_threads();
			printf("Test openMP:  Master said the Number of threads is %d\n", nthreads);
			}
		
	}  /* All threads join master thread and terminate */
	
	#endif

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	int i,j,k; //just indices
	outputpath = thedirname;


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Initialize the value of the initial or boundary conditions for glucose and oxygen
	Initial_Oxygen = Init_Oxy;
	Initial_Glucose =  Init_Glu;
	Initial_Growthfactors = INIT_GROWTHFACTORS;
	Initial_Lactate =  INIT_LACTATE;

	oxygen_dirichlet_value = Initial_Oxygen;
	glucose_dirichlet_value = Initial_Glucose;
	growthfactors_dirichlet_value = Initial_Growthfactors;
	lactate_dirichlet_value = Initial_Lactate;


	
	myVoronoiDiagram = voronoiDiagram;
	voronoiCells =  voronoiDiagram->voronoiCells;
	myAgentList = agentList;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Define the values for the domain and derive the number of bridges
	spacestep = 1.;
	
	axe_size[0] = 1;
	axe_size[1] = 1;
	axe_size[2] = 1;

	for(i = 0; i < DIMENSIONS; i++)
		{axe_size[i] = sizeofaxes[i];}


	
		

	xminDOMAIN = voronoiDiagram->xMin[0];
	xmaxDOMAIN = voronoiDiagram->xMax[0];
	if(DIMENSIONS > 0)
		{
		yminDOMAIN = voronoiDiagram->xMin[1];
		ymaxDOMAIN = voronoiDiagram->xMax[1];

		if(DIMENSIONS > 1)
			{
			zminDOMAIN = voronoiDiagram->xMin[2];
			zmaxDOMAIN = voronoiDiagram->xMax[2];
			}
		}

	BoundaryLayer = 1.;

	//The maximum number of bridges (they won't all be used) is 3*(m*m*m) = 3*agentnumber
	//it's not 6 because, bridges are shared between two voronoiCells.
	
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Find the Cube in the regular mesh where the Agent is.
	double x,y,z;
	int mx,my,mz;
	TheCubes = new VoronoiCell***[axe_size[0]];
	for(i = 0; i < axe_size[0]; i++)
		{
		TheCubes[i] = new VoronoiCell**[ axe_size[1] ];
		for(j = 0; j < axe_size[1]; j++)
			{
			TheCubes[i][j] = new VoronoiCell*[ axe_size[2] ];
			for(k = 0; k < axe_size[2]; k++)
				{
				TheCubes[i][j][k] = NULL;
				}
			}
		}
	 
			//fprintf( stderr, "agentnumber %i\n", agentnumber);
	for(i = 0; i < voronoiDiagram->countVoronoiCells; i++)
		{
		//fprintf( stderr, "\ragent %i (%lf,%lf,%lf)            \b", i,voronoiCells[i]->position[0],voronoiCells[i]->position[1],voronoiCells[i]->position[2]);
	
		x = voronoiCells[i]->position[0];
		y = voronoiCells[i]->position[1];
		z = voronoiCells[i]->position[2];
		
		mx = (int)(x/spacestep);
		my = (int)(y/spacestep);
		mz = (int)(z/spacestep);

		if(DIMENSIONS < 3){mz = 0;}
		
		TheCubes[mx][my][mz] = voronoiCells[i];

		}
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Check if there is an agent in every cube.
	#if  VERBOSE>= 1
	int count = 0;
	for(i = 0; i < axe_size[0]; i++)
		{
		for(j = 0; j < axe_size[1]; j++)
			{
			for(k = 0; k < axe_size[2]; k++)
				{
				if(TheCubes[i][j][k] == NULL)
				{count++;}
				}
			}
		}

	if(count > 0){exit(1);}
	#endif
	
	
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//IMPORTANT
	//In any case we are looking. Because we assume that the steady state of the diffusion is reached,
	//there will the initial concentration in all agent. Even in the first tumor cell.

	//Has to be called by the programme before each run
	//Initialization(type);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//just check the if all as been done correctly
	/*
	for(i = 0; i < bridgenumber; i++)
		{
		bool isitok = true;
		
		if(bridges[i].agent1->oxygen != Initial_Oxygen){isitok = false;}
		if(bridges[i].agent2->oxygen != Initial_Oxygen){isitok = false;}
		if(bridges[i].agent1->glucose != Initial_Glucose){isitok = false;}
		if(bridges[i].agent2->glucose != Initial_Glucose){isitok = false;}
		if(bridges[i].agent1->growthfactors != 0){isitok = false;}
		if(bridges[i].agent2->growthfactors != 0){isitok = false;}
		
		#if  VERBOSE>= 1
		if(isitok == false)
		{printf("William said: One agent in the bridge number %i has not been well initialized.",i);}
		#endif
		}*/

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Definition of the diffusion timestep
	//SPATIAL_UNIT is the unit of the mesh 1 = AgentDiameter*(Pi/6)^(1/3)


	//insurance
	if( customTimestep==0.)
		timestep = TIME_STEP;
	else
		timestep = customTimestep;
		

	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//don't forget to initialize values because it's an initialization function
	TimeSum = 0;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Store usefull values to avoid to compute them at each call
	



	double alpha_oxygen = Oxygen_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_glucose = Glucose_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_growthfactors = GrowthFactors_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_lactate = Lactate_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);


	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Sensitivity analysis
	if(Work_Parameter >= 0.)
		{
		ANGIOGENESIS_GROWTHFACTOR_THRESHOLD /= Work_Parameter;

		}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//ADI method initialization

	Molecule::SetDomain(axe_size[0],axe_size[1],axe_size[2],timestep,TheCubes);
	Molecule::SetAgents( agentList);
	

	string bc;
	if( strcmp( boundaryCondition, "") == 0 )
		if(type == 2 || type == 4 || type == 5){bc = "Neumann";}else{bc = "Dirichlet";}
	else{
		if( strcmp( boundaryCondition, "Neumann") == 0 || strcmp( boundaryCondition, "neumann") == 0){
			bc = "Neumann";
		}else if( strcmp( boundaryCondition, "Dirichlet") == 0 || strcmp( boundaryCondition, "dirichlet") == 0){
			bc = "Dirichlet";
		}else{
			fprintf( stderr, "ERROR: Unknown border condition \"%s\"\n", boundaryCondition);
			exit( 0);
		}
		//strcpy(bc, boundaryCondition, 512);
		
	}
	
	int aboutborders = BORDERAREBORDER;
	if(type == 2){aboutborders = BORDERAREBORDER;}
	if(type == 3){aboutborders = FREEAREBORDER;}
	if(type == 4 || type == 5){aboutborders = VESSELAREBORDER;}


	Oxygen.SetMethod(CONSUMPTION_DIFFUSION,aboutborders,bc);
	Oxygen.SetMemory();
	for(i = 0; i < axe_size[0]; i++){for(j = 0; j < axe_size[1]; j++){for(k = 0; k < axe_size[2]; k++){
	Oxygen.SetValuePointer(i, j, k, &(TheCubes[i][j][k]->oxygen));}}}
	Oxygen.SetMatrix(alpha_oxygen);
	Oxygen.Rate = &GiveMeTheOxygenRate;

	


	Glucose.SetMethod(CONSUMPTION_DIFFUSION,aboutborders,bc);
	Glucose.SetMemory();
	for(i = 0; i < axe_size[0]; i++){for(j = 0; j < axe_size[1]; j++){for(k = 0; k < axe_size[2]; k++){
	Glucose.SetValuePointer(i, j, k, &(TheCubes[i][j][k]->glucose));}}}
	Glucose.SetMatrix(alpha_glucose);
	Glucose.Rate = &GiveMeTheGlucoseRate;



	Lactate.SetMethod(PURE_DIFFUSION,BORDERAREBORDER,bc);
	Lactate.SetMemory();
	for(i = 0; i < axe_size[0]; i++){for(j = 0; j < axe_size[1]; j++){for(k = 0; k < axe_size[2]; k++){
	Lactate.SetValuePointer(i, j, k, &(TheCubes[i][j][k]->lactate));}}}
	Lactate.SetMatrix(alpha_lactate);


	Growthfactors.SetMethod(PURE_DIFFUSION,NECROTICAREBORDER,"Dirichlet");
	Growthfactors.SetMemory();	
	for(i = 0; i < axe_size[0]; i++){for(j = 0; j < axe_size[1]; j++){for(k = 0; k < axe_size[2]; k++){
	Growthfactors.SetValuePointer(i, j, k, &(TheCubes[i][j][k]->growthfactors));}}}
	Growthfactors.SetMatrix(alpha_growthfactors);


	//Done in the initialization function

	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Initialization of the openGL visualization in Qt.
	#ifdef __WithGUI__
	//We assume that we start from zero.
	gui_is_defined = false;
	gui = thegui;
	gui->glWidget->DefineSimulation( this , gui);

	if(type == 4 || type == 5)
		gui->glWidget->viewVessels = true;

	gui_is_defined = true;
	pause = false;
	#endif
	

	#ifdef __With_Outputs__
	outputnumber = 0;
	simulation_number = 0;
	max_output_number = 50000;
	y_viewfixpoint = 0;
	z_viewfixpoint = 0;
	if(DIMENSIONS > 1)
	{y_viewfixpoint = sizeofaxes[1]/2;}
	if(DIMENSIONS > 2)
	{z_viewfixpoint = sizeofaxes[2]/2;}

	myRecord.Init();
	
	OutputParameters();

	BigOutput_number = 0;
	#endif

	#if VERBOSE >= 1
	cerr << "william said: end of the Subtrate Initialisation\n";
	#endif
	}

void Substrate::RebuildMatrix()
	{

	#if  VERBOSE>= 1
	cerr << "William said: Rebuilding the matix. \n";
	#endif
	double alpha_oxygen = Oxygen_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_glucose = Glucose_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_growthfactors = GrowthFactors_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
	double alpha_lactate = Lactate_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);


	
	Molecule::ChangeTimeStep(timestep);

	Oxygen.SetMatrix(alpha_oxygen);

	Glucose.SetMatrix(alpha_glucose);

	Lactate.SetMatrix(alpha_lactate);

	Growthfactors.SetMatrix(alpha_growthfactors);
	

	}

Substrate::~Substrate()
	{
	#ifdef __WithGUI__
	gui->glWidget->KillVisualization(gui);
	#endif

	#ifdef __With_Outputs__
	SimpleOutput();
	
	if(outputnumber < max_output_number){max_output_number = outputnumber;} 
	MakeGnuplotScript(max_output_number);
	#endif



	
	for(int i = 0; i < axe_size[0]; i++)
		{
		for(int j = 0; j < axe_size[1]; j++)
			{
			delete[] TheCubes[i][j];
			}
		delete[] TheCubes[i];
		}
	delete[] TheCubes;

	//cerr << "I've been killed... signed the substrate" << endl;

	/*Oxygen.~Molecule();
	Glucose.~Molecule();
	Growthfactors.~Molecule();
	Lactate.~Molecule();*/


	#ifdef __MultiMade__
	//Here is the code you want execute only is multi made version.
	
	/* Get the current time. I stole this from Nick. GnakGnakGnak*/
	/*time_t temp_t = time(NULL);
	tm* loctime = localtime(&temp_t);
	char local_time[256];
	strftime (local_time, 256, "%A, %B %d. %k:%M:%S ", loctime);
	FILE *stream;
	stream = fopen("../../output/SimulationOver.txt", "at");
	fprintf(stream, "G:%f O:%f is over at %s.\n",Initial_Glucose,Initial_Oxygen,local_time);
	fclose(stream);*/
	#endif
	
	}

void Substrate::Initialization(int mode)
	{
	

	#ifdef __With_Outputs__
	simulation_number++;
	if(simulation_number > 1 && outputnumber < max_output_number){max_output_number = outputnumber;}
	outputnumber = 0;

	//To have the big output only for the first simulation, comment the line below
	//BigOutput_number = 0;


	#endif

	#ifdef __WithGUI__
	SwitchOffVisualization();
	gui->glWidget->viewVessels = false;
	#endif

	Initialize_Oxygen();
	Initialize_Glucose();
	Initialize_Growthfactors();
	Initialize_Lactate();

	if(mode == 4 || mode == 5)
		{vasculature.Initialize(myAgentList);}

	TimeSum = 0;
	TotalSimulationTime = 0.0;
	
	#ifdef __With_Outputs__
	SimpleOutput();
	#endif
	
	#ifdef __WithGUI__
	gui_is_defined = false;
	gui->glWidget->DefineSimulation( this , gui);

	if(mode == 4 || mode == 5)
		gui->glWidget->viewVessels = true;

	gui_is_defined = true;
	pause = false;

	SwitchOnVisualization();
	#endif
	}
void Substrate::Initialize_Oxygen()
	{
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//IMPORTANT
	//In any case we are looking. Because we assume that the steady state of the diffusion is reached,
	//there will be the initial concentration in all agent. Even in the first tumor cell.

	int i;
	for(i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		{
		voronoiCells[i]->oxygen = Initial_Oxygen;
		}
	}

void Substrate::Initialize_Glucose()
	{
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//IMPORTANT
	//In any case we are looking. Because we assume that the steady state of the diffusion is reached,
	//there will be the initial concentration in all agent. Even in the first tumor cell.

	int i;
	for(i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		{
		voronoiCells[i]-> glucose = Initial_Glucose;
		}
	}

void Substrate::Initialize_Growthfactors()
	{
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//IMPORTANT
	//In any case we are looking. Because we assume that the steady state of the diffusion is reached,
	//there will be the initial concentration in all agent. Even in the first tumor cell.

	int i;
	for(i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		{
		//voronoiCells[i]->growthfactors = Initial_Growthfactors;
		voronoiCells[i]->growthfactors = 0.;
		}
	}

void Substrate::Initialize_Lactate()
	{
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//IMPORTANT
	//In any case we are looking. Because we assume that the steady state of the diffusion is reached,
	//there will be the initial concentration in all agent. Even in the first tumor cell.

	int i;
	for(i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		{
		voronoiCells[i]->lactate = Initial_Lactate;
		}
	}



int Substrate::Update_All(int mode, double duration, ActionTree *p_list)
	{
	
	#ifdef QUALITY_CONTROL
	QualityControl(mode);
	exit(1);
	#endif

	//TIME DIVISION
	TotalSimulationTime += duration;
	TimeSum += duration;
	int iterations = (int)(TimeSum/(timestep));
	TimeSum -= timestep*iterations;
	#if  VERBOSE>= 3
	printf("William said: number of iterations = %i\n",iterations);
	#endif

#ifdef __TestSpeed__
for(int test = 0; test < 1000; test++) ///*Speed test*/
	{
	timer.Start();
#endif

	for(int i = 0; i < iterations; i++)
		{
		
		if( mode > 1)
			{
#ifdef __myOMP__
#pragma omp parallel
	{
	 #pragma omp sections nowait
		{
		#pragma omp section
			{

#endif


			
			#ifndef __NoLimitation__
			Oxygen.GiveMe(&Oxygen);
			#endif


			#ifndef __NoLimitation__
			Glucose.GiveMe(&Glucose);
			#endif

#ifdef __myOMP__
}/* end of this section */
#pragma omp section
	{


#endif
			//Growth factors
			if( mode == 5 )
				{

				Growthfactors.GiveMe(&Growthfactors);

				}


#ifdef __myOMP__
}  /* end of this section */
}  /* end of sections */
}  /* end of parallel section */
#endif			
			}
		
		/*
		//VEGF
		if( mode == 5 )
			{
			}
		*/


		#ifdef __With_Outputs__
		//if(TotalSimulationTime > time_output_frequency*outputnumber){outputnumber = Output(outputnumber);}
		#endif

		} 

	if(mode == 5 && iterations > 0)
		{
		Produce_Growthfactors();
		if(vasculature.Update_Network(timestep*iterations, p_list, myVoronoiDiagram))	
			{
			vasculature.GiveMeTheBlood();
			Refill_Oxygen(mode);
			Refill_Glucose(mode);
			}
		}

	#ifdef __With_Outputs__
	if(iterations > 0)
		{
		int living_cells = getdata_angiogenesis(5);
		int vessel_cells = getdata_angiogenesis(4);
		double radius_of_gyration = getdata(3);
		myRecord.addvalue(TotalSimulationTime, vessel_cells , living_cells, radius_of_gyration );
		//SimpleOutput();
		if( (int)(TotalSimulationTime / 50.) > BigOutput_number)
			{BigOutput(TotalSimulationTime);BigOutput_number++;}
		}
	#endif

#ifdef __TestSpeed__
	timer.End();
	
	#if VERBOSE >= 1
	cerr << timer.Mean() << " seconds/computation " << timer.Total() << " seconds in "<< timer.calls << " calls." << endl;
	if( timer.calls > 20 )
			exit(1);
	#endif

}/*end of the speed test*/
#endif
	#ifdef __WithGUI__
	int i = 0;
	while(pause)
		{
		#if  VERBOSE>= 1
		cerr << "William is on a strike he said: I'dont want to compute anymore:" << i << endl;
		#endif
		i++;
		gui->glWidget->computation_is_paused = true;
		}
	gui->glWidget->computation_is_paused = false;
	#endif
	

	
	return iterations;
	}




void Substrate::Refill_Oxygen(int mode)
	{
	//if(mode == 1){}
	//if(mode == 2){}
	
	if(mode == 3)
		{
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if( GetAgent( voronoiCells[i]) == NULL)
				{voronoiCells[i]->oxygen = Initial_Oxygen;}
			}
		}

	if(mode == 4)
		{
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if(voronoiCells[i]->getState() == VESSEL)
				{
				voronoiCells[i]->oxygen = Initial_Oxygen;
				}
			}
		}

	if(mode == 5)
		{
		for(int i = 0; i < vasculature.nbrSlices; i++)
			{
			if(vasculature.Slice[i]->circulation)
				{
				GetVoronoiCell(  vasculature.Slice[i]->node1->p_agent )->oxygen = Initial_Oxygen;
				GetVoronoiCell(  vasculature.Slice[i]->node2->p_agent )->oxygen = Initial_Oxygen;
				}
			}
		}
	}

void Substrate::Refill_Glucose(int mode)
	{
	//Nothing to do with mode 1
	//if(mode == 1){}
	//if(mode == 2){}
	if(mode == 3)
		{
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if( GetAgent( voronoiCells[i]) == NULL)
				{voronoiCells[i]->glucose = Initial_Glucose;}
			}
		}

	if(mode == 4)
		{
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if(voronoiCells[i]->getState()  == VESSEL)
				{
				voronoiCells[i]->glucose = Initial_Glucose;
				}
			}
		}

	if(mode == 5)
		{
		for(int i = 0; i < vasculature.nbrSlices; i++)
			{
			if(vasculature.Slice[i]->circulation)
				{
				GetVoronoiCell( vasculature.Slice[i]->node1->p_agent )->glucose = Initial_Glucose;
				GetVoronoiCell( vasculature.Slice[i]->node2->p_agent )->glucose = Initial_Glucose;
				}
			}
		}
	}

void Substrate::Refill_Lactate()
	{
	//the boundary are not changed by the diffusion computation
	for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		if( GetAgent( voronoiCells[i]) == NULL)
			voronoiCells[i]->lactate = Initial_Lactate;	
	}


void Substrate::Produce_Lactate()
	{
	for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
		{
		double qg = voronoiCells[i]->glucose*GiveMeTheGlucoseRate(voronoiCells[i]);
		double qo = voronoiCells[i]->oxygen*GiveMeTheOxygenRate(voronoiCells[i]) / 6.0;
		voronoiCells[i]->lactate += timestep*2*(qg - min(qg, qo));
		}
	}

void Substrate::Produce_Growthfactors()
	{
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if(voronoiCells[i]->getState()  == NECROTIC )
				{
				voronoiCells[i]->growthfactors = Initial_Growthfactors;
				}
			}
	}
	
//********************************
//END of the computation functions
//********************************

#ifdef __WithGUI__

void Substrate::SwitchOnVisualization()
	{
	#if  VERBOSE>= 1
	//cerr << "William said:I've been switched on... signed the visualization" << endl;
	#endif
	//Because of the threads, we have sometimes to wait
	while(!gui_is_defined)
		{

		for(int i = 0;i < 1000000;i++){}
		}
	gui->glWidget->OpenVisualization(gui);
	}
	
void Substrate::SwitchOffVisualization()
	{
	#if  VERBOSE>= 1
	//cerr << "I've been killed... signed the visualization" << endl;
	#endif
	gui->glWidget->KillVisualization(gui);
	
	}

void Substrate::PlotGraphics()
	{
	/*
	int a = 0, b = 0, c = 0;
	
	for(int i = 0; i < agentnumber; i++)
		{
		if(voronoiCells[i]->getState() == ACTIVE){a++;}
		if(voronoiCells[i]->getState() == NONACTIVE){a++;}
		if(voronoiCells[i]->getState() == NECROTIC){a++;}
		}
	
	//Average concentration of oxygen in the medium
	b = (int)getdata(2);
	c = (int)getdata(3);
	
	gui->graphiques->AddValues(a,b,c);
 	#if  VERBOSE >= 7
	cerr << "William said: J'ajoute les valeurs" << a << "," << b << "," << c;	
	#endif
	*/
	}
#endif

#ifdef __With_Outputs__

double Substrate::getdata(int WhatData)
	{
	double value = 0.0;

	if(WhatData == 1)
		{
		
		//Average concentration of glucose in the medium
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			value += voronoiCells[i]-> glucose;
			}
		value /= (double)myVoronoiDiagram->countVoronoiCells;
		}

	if(WhatData == 2)
		{
		//Average concentration of oxygen in the medium
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			value += voronoiCells[i]->oxygen;
			}
		value /= (double)myVoronoiDiagram->countVoronoiCells;
		}

	if(WhatData == 3)
		{
		//the radius of gyration

		//First find the center of mass
		int N = 0;
		double x = 0., y = 0., z = 0.;
		double Xcm = 0., Ycm = 0., Zcm = 0.;
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			//It should be the same condition in the next loop.
			if( voronoiCells[i]->getState() == ACTIVE || voronoiCells[i]->getState() == NONACTIVE || voronoiCells[i]->getState() == NECROTIC)
				{
				N++;
				x = voronoiCells[i]->position[0];
				y = voronoiCells[i]->position[1];
				z = voronoiCells[i]->position[2];
			
				Xcm += x;
				Ycm += y;
				Zcm += z;
				}
			}

		Xcm /= (double)N;
		Ycm /= (double)N;
		Zcm /= (double)N;

		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			//It should be the condition than above
			if(voronoiCells[i]->getState() == ACTIVE || voronoiCells[i]->getState() == NONACTIVE || voronoiCells[i]->getState() == NECROTIC)
				{
				x = voronoiCells[i]->position[0];
				y = voronoiCells[i]->position[1];
				z = voronoiCells[i]->position[2];
			
				value += ((x-Xcm)*(x-Xcm) + (y-Ycm)*(y-Ycm) + (z-Zcm)*(z-Zcm));
				}
			}
		value /= (double)N;
		value = sqrt(value);
		}

	if(WhatData == 4)
		{
		//the number of Vessels
		value = (double)vasculature.nbrNodes;
		}

	if(WhatData == 5)
		{
		//the number of living cells
		int counter = 0;
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if( voronoiCells[i]->getState() == ACTIVE || voronoiCells[i]->getState() == NONACTIVE)
				counter++;
			}
		value = (double)counter;
		}


	return value;
	}


int Substrate::getdata_angiogenesis(int WhatData)
	{
	int data = 0;
	if(WhatData == 4)
		{
		//the number of Vessels
		data = vasculature.nbrNodes;
		}

	if(WhatData == 5)
		{
		//the number of living cells
		int counter = 0;
		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if( voronoiCells[i]->getState() == ACTIVE || voronoiCells[i]->getState() == NONACTIVE)
				{counter++;}
			}
		data = counter;
		}


	return data;
	}


int Substrate::SimpleOutput()
	{
	//FILE *stream;
	char filename[100];
	sprintf(filename,"%s/densities.txt",outputpath);

	std::ofstream myfile(filename);		
	if (myfile.is_open())
		{		
		for(int i = 0; i < DataRecord::MaxRecord; i++)
			{
			myfile << myRecord.time[i] << "\t";
			myfile << myRecord.living_cells[i] << "\t";
			myfile << myRecord.vessels[i] << "\t";
			myfile << myRecord.radius_of_gyration[i] << "\n";
			}
		myfile.close();
		}
	else{cerr << "Unable to open file in Substrate::Simple Output "<< endl; exit(1);}
	
	return 0;
	}

int Substrate::BigOutput(double current_time)
	{
	//FILE *stream;
	char filename[100];
	sprintf(filename,"%s/thebigone%i.txt", outputpath, BigOutput_number);

	#if VERBOSE >= 1
	cerr << endl << "Make the big output... at time: " << current_time << endl;
	#endif
	
	int total_number_of_lines = 0;

	for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if(voronoiCells[i]->getState() != FREE && voronoiCells[i]->getState() != VESSEL)
				{
				total_number_of_lines++;
				}
			}

	std::ofstream myfile(filename);		
	if (myfile.is_open())
		{
		myfile <<"#time:"<<current_time<<endl;
		myfile <<"#X:" << xmaxDOMAIN <<" Y:"<< ymaxDOMAIN << " Z:"<< zmaxDOMAIN << endl;
		myfile <<"#x:" << xminDOMAIN <<" y:"<< yminDOMAIN << " z:"<< zminDOMAIN << endl;
		myfile <<"#agentnumber:" << total_number_of_lines + vasculature.nbrSlices << endl;

	

		for(int i = 0; i < myVoronoiDiagram->countVoronoiCells; i++)
			{
			if(voronoiCells[i]->getState() != FREE && voronoiCells[i]->getState() != VESSEL)
				{
				myfile << voronoiCells[i]->position[0] << "\t";
				myfile << voronoiCells[i]->position[1] << "\t";
				myfile << voronoiCells[i]->position[2] << "\t";
				myfile << voronoiCells[i]->getState()  << "\n";
				}
			}

		for(int i = 0; i < vasculature.nbrSlices; i++)
			{
			myfile << vasculature.Slice[i]->node1->x << "\t";
			myfile << vasculature.Slice[i]->node1->y << "\t";
			myfile << vasculature.Slice[i]->node1->z << "\t";
			myfile << VESSEL << "\t";
			myfile << vasculature.Slice[i]->node1->pressure + vasculature.Slice[i]->node2->pressure << "\t";

			myfile << vasculature.Slice[i]->node2->x << "\t";
			myfile << vasculature.Slice[i]->node2->y << "\t";
			myfile << vasculature.Slice[i]->node2->z << "\t";
			myfile << vasculature.Slice[i]->theta<< "\t";
			myfile << vasculature.Slice[i]->phi << "\t";
			myfile << vasculature.Slice[i]->length << "\t";
			myfile << vasculature.Slice[i]->node1->radius << "\t";
			myfile << vasculature.Slice[i]->node2->radius << "\n";
			}

		myfile.close();
		}
	else{cerr << "Unable to open file in Substrate::Simple Output "<< endl; exit(1);}
	
	return 0;
	}

int Substrate::Output(int Number)
	{
	//FILE *stream;
	char filename[100];
	//cerr << Number << endl;
	if(Number == -1)
		{
		sprintf(filename,"%s/steadystate.txt",outputpath);
		}
	else
		{
		sprintf(filename,"%s/%i.txt",outputpath,Number);
		}

	
	//When it's not the first simulation, read, mean then write a file.
	if(simulation_number > 1 && Number < max_output_number)
		{
		double *A1,*B1;
		double zix,ziy, **A2, **B2;
		std::ifstream myfiletoread(filename, ios::in);
		if (myfiletoread.is_open())
			{
			if(dimensionview == 2)
				{
				A2 = new double*[axe_size[0]];
				B2 = new double*[axe_size[0]];
				for(int i = 0; i < axe_size[0]; i++)
					{
					A2[i] = new double[axe_size[1]];
					B2[i] = new double[axe_size[1]];
					}
	
				int i = 0, j = 0;
			
				while ( i <  axe_size[0])
					{
					myfiletoread >> zix >> ws;
					myfiletoread >> ziy >> ws;
					myfiletoread >> A2[i][j] >> ws;
					myfiletoread >> B2[i][j] >> ws;
					
					A2[i][j] *=  simulation_number;
					A2[i][j] +=  TheCubes[i][j][z_viewfixpoint]->oxygen;
					A2[i][j] /=  simulation_number + 1;

					B2[i][j] *=  simulation_number;
					B2[i][j] +=  TheCubes[i][j][z_viewfixpoint]->glucose;
					B2[i][j] /=  simulation_number + 1;
					
					//cerr << zix << "\t";
					//cerr << ziy << "\t";
					//cerr << A2[i][j] << "\t";
					//cerr << B2[i][j] << "\n"; 
					j+=space_output_frequency;
					if(j >= axe_size[1])
						{
						j = 0;
						i += space_output_frequency;
						}
					}
				}
			if(dimensionview == 1)
				{
				A1 = new double[axe_size[0]];
				B1 = new double[axe_size[0]];	
				int i = 0;

				while ( i <  axe_size[0])
					{
					myfiletoread >> zix >> ws;
					myfiletoread >> A1[i] >> ws;
					myfiletoread >> B1[i] >> ws;

					A1[i] *=  simulation_number;
					A1[i] +=  TheCubes[i][y_viewfixpoint][z_viewfixpoint]->oxygen;
					A1[i] /=  simulation_number + 1;

					B1[i] *=  simulation_number;
					B1[i] +=  TheCubes[i][y_viewfixpoint][z_viewfixpoint]->glucose;
					B1[i] /=  simulation_number + 1;

					//cerr << i*spacestep << "\t";
					//cerr << A1[i] << "\t";
					i+=space_output_frequency;
					}
				}
			myfiletoread.close();
			}
			else{cerr <<endl<< "Unable to open file in Substrate::Output number = " << Number << ", simulation number = "<< simulation_number << endl; exit(1);}
		
		std::ofstream myfile(filename);		
		if (myfile.is_open())
			{
			if(dimensionview == 2)
				{
				for(int i=0; i < axe_size[0]; i += space_output_frequency)
					{
					for(int j=0; j < axe_size[1];j += space_output_frequency )
						{
						myfile << i*spacestep << "\t";
						myfile << j*spacestep << "\t";
						myfile << A2[i][j] << "\t";
						myfile << B2[i][j] << "\n";
						}
					}
				for(int i = 0; i < axe_size[0]; i++)
					{
					delete[] A2[i];
					delete[] B2[i];
					}
				delete[] A2;
				delete[] B2;
				}
			if(dimensionview == 1)
				{
				for(int i=0; i < axe_size[0]; i += space_output_frequency)
					{
					myfile << i*spacestep << "\t";
					myfile << A1[i] << "\t";
					myfile << B1[i] << "\n";
					}
				delete[] A1;
				delete[] B1;	
				}
			myfile.close();
			}
			else{cerr << "Unable to open file in Substrate::Output number " << Number << "simulation number "<< simulation_number << endl; exit(1);}
		}
	//Create a file when it does not exist
	else
		{
		std::ofstream myfile(filename);		
		if (myfile.is_open())
			{
			if(dimensionview == 2)
				{
				for(int i=0; i < axe_size[0]; i += space_output_frequency)
					{
					for(int j=0; j < axe_size[1];j += space_output_frequency )
						{
						myfile << i*spacestep << "\t";
						myfile << j*spacestep << "\t";
						myfile << TheCubes[i][j][z_viewfixpoint]->oxygen << "\t";
						myfile << TheCubes[i][j][z_viewfixpoint]->glucose << "\n";
						}
					}
				}
			if(dimensionview == 1)
				{
				for(int i=0; i < axe_size[0]; i += space_output_frequency)
					{
					myfile << i*spacestep << "\t";
					myfile << TheCubes[i][y_viewfixpoint][z_viewfixpoint]->oxygen << "\t";
					myfile << TheCubes[i][y_viewfixpoint][z_viewfixpoint]->glucose << "\n";		
					}
				}
			myfile.close();
			}
			else{cerr << "Unable to open file in Substrate::Output number " << Number << "simulation number "<< simulation_number << endl; exit(1);}
		}
	return ++Number;
	}
	
void Substrate::MakeGnuplotScript(int NumberOfFiles)
	{	
	char filename[100];
	sprintf(filename,"%s/script-gnuplot.txt",outputpath);
	
	std::ofstream myfile(filename);

	if (myfile.is_open())
  		{
		#if  VERBOSE >= 1
		cerr << "make the gnuplot script for "<< NumberOfFiles << " text flies"<< endl;	
		#endif
		myfile << "set autoscale\n"; 
		myfile << "set xtic auto\n";
		myfile << "set ytic auto\n";
		myfile << "set title \" ADI Scheme 3D z = " << z_viewfixpoint<< "\"\n";
		myfile << "set xlabel 'x'\n";
		myfile << "set ylabel 'y'\n";
		myfile << "set zlabel 'Concentration'\n";
		//myfile << "set logscale y\n";
		myfile << "set size 1,1\n";
		myfile << "set xrange["<< 0 <<":"<< spacestep*(axe_size[0]-1)<< "]\n";

		
		if(dimensionview == 2)
			{
			myfile << "set zrange["<< 0.0<<":"<< Initial_Glucose*1.1<< "]\n";
			for(int i=0; i < NumberOfFiles ;i++)
			{myfile << "splot '"<< i << ".txt' using 1:2:3 title 'Oxygen ("<<i<<")', '"<< i << ".txt' using 1:2:4 title 'Glucose'\n";}
			}
		if(dimensionview == 1)
			{
			myfile << "set yrange["<< 0.0<<":"<< Initial_Glucose*1.1<< "]\n";
			for(int i=0; i < NumberOfFiles ;i++)
			{myfile << "plot '"<< i << ".txt' using 1:2 title 'Oxygen ("<<i<<")', '"<< i << ".txt'using 1:3 title 'Glucose'\n";}
			}
		myfile.close();
		}
	else{cerr << "Unable to open file";exit(1);}

	sprintf(filename,"%s/script-radial.txt",outputpath);
	std::ofstream radialfile(filename);
	if (radialfile.is_open())
  		{
		#if  VERBOSE >= 3
		cerr << "make the gnuplot script "<< endl;
		#endif
		radialfile << "set autoscale\n"; 
		radialfile << "set xtic auto\n";
		radialfile << "set ytic auto\n";
		radialfile << "set title \" Mean over Radius and Time\" \n";
		radialfile << "set xlabel 'x'\n";
		radialfile << "set ylabel 'y'\n";
		radialfile << "set zlabel 'Concentration'\n";
		radialfile << "set size 1,1\n";
		radialfile << "set xrange["<< 0 <<":"<< spacestep*(axe_size[0]-1)<< "]\n";

		
		radialfile << "set zrange["<< 0.0<<":"<< Initial_Oxygen*1.1<< "]\n";
		radialfile << "splot 'radialData.dat'\n";
		radialfile.close();
		}
	else{cerr << "Unable to open file";exit(1);}

	}


#endif

#ifdef QUALITY_CONTROL

double Substrate::fixrate = 0.;

double Substrate::FixRate(VoronoiCell *thecell)
	{
	return fixrate;
	}

void Substrate::QualityControl(int mode)
	{
	//Iterations
	int iteration_todo = 70000;
	mymolecule_diffusion = 100;
	fixrate = 0.1;
	mymolecule_dirichletvalue = 16.5; // No zero

	#if VERBOSE >= 1
	cerr << "Quality Control Starts in dimension = " << DIMENSIONS;
	cerr << " with time step = " << timestep << endl;
	#endif


	//Fisrt Initialization
	
	mymolecule.SetMemory();


	alpha_mymolecule = mymolecule_diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);

	mymolecule.SetMethod(CONSUMPTION_DIFFUSION,BORDERAREBORDER,"Neumann");
	
	int i,j,k;
	for(i = 0; i < axe_size[0]; i++){for(j = 0; j < axe_size[1]; j++){for(k = 0; k < axe_size[2]; k++)
	{mymolecule.SetValuePointer(i, j, k, &(TheCubes[i][j][k]->glucose));}}}

	mymolecule.SetMatrix(alpha_mymolecule);
	mymolecule.Rate = &FixRate;

	//TEST PURE DIFFUSION
		//TEST NEUMANN
			//DIFFUSION  = 0
				//STEADY
				//UNSTEADY
			//DIFFUSION != 0
				//STEADY
				//UNSTEADY
		//TEST DIRICHLET
	//TEST ut = uxx - qu
		//Same
	//TEST ut = uxx - q
		//Same
	/*
	string bc[2];
		bc[0] = "Neumann";
		bc[1] = "Dirichlet";
		
	double mydirichletvalues[2];
		mydirichletvalues[0] = 0.;
		mydirichletvalues[1] = mymolecule_dirichletvalue;
	double mydiff[2];
		mydiff[0] = 0.;
		mydiff[1] = mymolecule_diffusion;
	double myrate[2];
		myrate[0] = 0.;
		myrate[1] = fixrate;
	
	int n = 1;
	for(i = 0; i < 3; i++) //DIFFERENT EQUATIONS
		{
		
		for(j = 0; j < 2; j++) //DIFFERENT BORDER CONDITIONS
			{
			for(int t = 0; t < 2 ; t++)
			{
			mymolecule_dirichletvalue = mydirichletvalues[t];
			if(bc[j] == "Neumann"){t = 123;}
			for(k = 0; k < 2; k++)	//DIFFUSION NULL OR NOT
				{
				
				for(int m = 0; m < 2; m++) //CONSUMPTION NULL OR NOT
					{
					
					if(i == 0){ m = 127;}

					alpha_mymolecule = mydiff[k]*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
					mymolecule.SetMethod(i,BORDERAREBORDER,bc[j]);
					mymolecule.SetMatrix(alpha_mymolecule);
					fixrate = myrate[m];

					cerr << "START TEST ("<<n<<"/60): " << i << ", " << bc[j] << ", ";
					if(bc[j] == "Dirichlet")
						cerr << " value = "<< mydirichletvalues[t] << ",";

					cerr << "Diff: " << mydiff[k];
					cerr << ", Consumption: " << myrate[m];

					cerr << ", Steady " << endl;
					RunSteadyTest(n,iteration_todo, bc[j], mydirichletvalues[t],mydiff[k],myrate[m]);
					n++;

					cerr << "START TEST ("<<n<<"/60): " << i << ", " << bc[j] << ", ";
					if(bc[j] == "Dirichlet")
						cerr << " value = "<< mydirichletvalues[t] << ",";

					cerr << "Diff: " << mydiff[k];
					cerr << ", Consumption: " << myrate[m];
					cerr << ", Unsteady " << endl;
					RunUnsteadyTest();
					n++;
					}	
				}
			}
			}
		}
	*/
	
//	TimeStepControl(mode);
// 	DirichletZero(mode);
//	Parabole(mode);
	Instationnaire_qu(mode);
// 	Instationnaire_q(mode);


	#if VERBOSE >= 1
	cerr << "End of the quality control" << endl;
	#endif	
	}

void Substrate::RunSteadyTest(int test_number,int time_iterations,string bc,double dirichlet_value,double mydiffusion, double myrate)
	{


	//INITIAL CONDITION	
	double u[ axe_size[0] ];
	double pi = 3.141592653589793238;
	double dx = SPATIAL_UNIT;
	double lgx = (axe_size[0] - 1) * SPATIAL_UNIT;
	double dt = timestep;

	if(bc == "Dirichlet")
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose    = sin(0*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += 8*sin(1*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose   += sin(2*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose   += sin(3*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose   += sin(4*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose   += sin(5*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose  += sin(17*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose  += sin(19*pi*dx*i/lgx);

			(TheCubes[i][0][0])->glucose  += dirichlet_value;
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		}
	else
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose  = 7*cos(0*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(1*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(2*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(3*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(4*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(5*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(17*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(19*pi*dx*i/lgx);
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		}
			


	for(int n = 0; n < time_iterations; n++)
			{
			mymolecule.GiveMe(&mymolecule);
			#if VERBOSE >= 1
			if(n % 47 == 0)
				cerr << "\r" << 100 * (double)n / (double)(time_iterations) <<"% 1602";
			#endif
			}
	
	#if VERBOSE >= 1
	cerr << "\n nb iterations: " << time_iterations << endl;
	#endif

	char filename[100];
	sprintf(filename,"%s/%i-quality_test.txt",outputpath,test_number);

	std::ofstream myfile(filename);
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;


	if (myfile.is_open())
		{

		myfile << "#border condition: " << bc << endl;
		myfile << "#dirichlet value:  " << dirichlet_value << endl;
		myfile << "#diffusion: " << mydiffusion << endl;
		myfile << "#Consumption: " << myrate << endl;
		myfile << "#Steady" << endl;		
		
		for(int i=0; i < axe_size[0]; i += quality_output_frequency)
			{
			myfile << i*dx << "\t";
			myfile << TheCubes[i][cy][cz]->glucose << "\t";
			myfile << u[i] << "\n";
			}
		myfile.close();
		}
	else{cerr << "Unable to open file qualitycontrol-"<< timestep << endl; exit(1);}
	
	}

void Substrate::RunUnsteadyTest()
	{}

void Substrate::Instationnaire_q(int mode)
	{
	//Gnuplot: plot "moche.txt"u 1:2 w l title 'simulation', "moche.txt"u 1:3 w l title 'exacte'
	//USE JAGIELLA AND USE THE FIXRATE 
	//Iterations


	int iteration_indice = 0;
	double dirichlet_value = 0;
	//string bc = "Neumann";
	string bc = "Dirichlet";

	fixrate = 0.01;

	//Glucose.SetMethod(PURE_DIFFUSION,BORDERAREBORDER,bc);
	Glucose.SetMethod(CONSUMPTION_DIFFUSION,BORDERAREBORDER,bc);
	double alpha = Glucose_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT);
	Glucose.SetMatrix(alpha);

	//Glucose.Rate = &GiveMeTheGlucoseRate;	
	Glucose.Rate = &FixRate;

	int a = 0;
	if(bc == "Dirichlet"){a = 1;}

	Glucose.BuildMatrix(0,-1,0,0,&Glucose,a);

	
	if(bc == "Dirichlet")
	{		
	for(int i = 0; i < axe_size[1];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[0][i][j])->glucose = dirichlet_value;
			(TheCubes[axe_size[0] - 1][i][j])->glucose = dirichlet_value;
			}
		}
	if(DIMENSIONS > 1)
		{
		for(int i = 0; i < axe_size[0];i++)
			{
			for(int j = 0; j < axe_size[2];j++)
				{
				(TheCubes[i][0][j])->glucose = dirichlet_value;
				(TheCubes[i][axe_size[1] - 1][j])->glucose = dirichlet_value;
				}
			}
		
		
		if(DIMENSIONS > 2)
			{
			for(int i = 0; i < axe_size[0];i++)
				{
				for(int j = 0; j < axe_size[1];j++)
					{
					(TheCubes[i][j][0])->glucose = dirichlet_value;
					(TheCubes[i][j][axe_size[2] - 1])->glucose = dirichlet_value;
					}
				}
			}
		}
	}

	//INITIAL CONDITION	
	double u[ axe_size[0] ];
	double pi = 3.141592653589793238;
	double dx = SPATIAL_UNIT;
	double lgx = (axe_size[0] - 1) * SPATIAL_UNIT;
	double mydiffusion = Glucose_Diffusion;
	double dt = timestep;

	if(bc == "Dirichlet")
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose    = sin(0*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose += 8*sin(1*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose   += sin(2*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose   += sin(3*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose   += sin(4*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose   += sin(5*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose  += sin(17*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			(TheCubes[i][0][0])->glucose  += sin(19*pi*dx*i/lgx) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		cerr << "last: " << u[ axe_size[ 0] - 1] <<endl;
		}
	else
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose  = 7*cos(0*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(1*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(2*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(3*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(4*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(5*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(17*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(19*pi*dx*i/lgx);
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		}


	for(int n = 0; n < iteration_indice; n++)
			{
			Update_Glucose(mode);
			#if VERBOSE >= 1
			if(n % 47 == 0)
				cerr << "\r" << 100 * (double)n / (double)(iteration_indice) <<"% 1745";
			#endif
			}
	
	#if VERBOSE >= 1
	cerr << "\n nb iterations: " << iteration_indice << endl;
	#endif
	char filename[100];
	sprintf(filename,"%s/moche.txt",outputpath);

	std::ofstream myfile(filename);
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;
	
	//Make Exact Solution
	double lambda;
	if(bc == "Dirichlet")
		{
		for(int i = 0 ; i < axe_size[0]; i++)
			{
			
			lambda = 0*0*mydiffusion*pi*pi/(lgx*lgx);
			u[i]   = sin(0*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 1*1*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += 8*sin(1*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice) + (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 2*2*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(2*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 3*3*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(3*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 4*4*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(4*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 5*5*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(5*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 17*17*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(17*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			lambda = 19*19*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += sin(19*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice)+ (i*dx)*(i*dx - lgx)*fixrate*0.5;
			
			}
		}
	else
		{
		for(int i = 0 ; i < axe_size[0]; i++)
			{

			lambda = 0*0*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]   = 7*cos(0*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 1*1*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(1*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 2*2*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(2*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 3*3*mydiffusion*pi*pi/(lgx*lgx);
			u[i]  += cos(3*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 4*4*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(4*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 5*5*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(5*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 17*17*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(17*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 19*19*mydiffusion*pi*pi/(lgx*lgx) ;
			u[i]  += cos(19*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			}
		}
	

	if (myfile.is_open())
		{
		for(int i=0; i < axe_size[0]; i += quality_output_frequency)
			{
			myfile << i*dx << "\t";
			myfile << TheCubes[i][cy][cz]->glucose << "\t";
			myfile << u[i] << "\n";	
			}
		myfile.close();
		}
	else{cerr << "Unable to open file qualitycontrol-"<< timestep << endl; exit(1);}	
	}

void Substrate::Instationnaire_qu(int mode)
	{
	//Gnuplot: plot "sturm.txt"u 1:2 w l title 'simulation', "sturm.txt"u 1:3 w l title 'exacte'
	//USE JAGIELLA AND USE THE FIXRATE 
	//Iterations


	int iteration_indice = 25000;

	string bc = "Neumann";

	//string bc = "Dirichlet";
	double dirichlet_value = 0;


	Glucose.SetMethod(PURE_DIFFUSION,BORDERAREBORDER,bc);
// 	Glucose.SetMethod(CONSUMPTION_DIFFUSION,BORDERAREBORDER,bc);

	double alpha = Glucose_Diffusion*timestep/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);
// 	alpha = 0.;	
	Glucose.SetMatrix(alpha);

	//Glucose.Rate = &GiveMeTheGlucoseRate;
	fixrate = 0.00;
	Glucose.Rate = &FixRate;

	int a = 0;
	if(bc == "Dirichlet"){a = 1;}

	Glucose.BuildMatrix(0,-1,0,0,&Glucose,a);

	
	if(bc == "Dirichlet")
	{		
	for(int i = 0; i < axe_size[1];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[0][i][j])->glucose = dirichlet_value;
			(TheCubes[axe_size[0] - 1][i][j])->glucose = dirichlet_value;
			}
		}
	if(DIMENSIONS > 1)
		{
		for(int i = 0; i < axe_size[0];i++)
			{
			for(int j = 0; j < axe_size[2];j++)
				{
				(TheCubes[i][0][j])->glucose = dirichlet_value;
				(TheCubes[i][axe_size[1] - 1][j])->glucose = dirichlet_value;
				}
			}
		
		
		if(DIMENSIONS > 2)
			{
			for(int i = 0; i < axe_size[0];i++)
				{
				for(int j = 0; j < axe_size[1];j++)
					{
					(TheCubes[i][j][0])->glucose = dirichlet_value;
					(TheCubes[i][j][axe_size[2] - 1])->glucose = dirichlet_value;
					}
				}
			}
		}
	}

	//INITIAL CONDITION	
	double u[ axe_size[0] ];
	double pi = 3.141592653589793238;
	double dx = SPATIAL_UNIT;
	double lgx = (axe_size[0] - 1) * SPATIAL_UNIT;
	double mydiffusion = Glucose_Diffusion;
// 	double mydiffusion = 0.;
	double dt = timestep;

	if(bc == "Dirichlet")
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose  = sin(0*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += 8*sin(1*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(2*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(3*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(4*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(5*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(17*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += sin(19*pi*dx*i/lgx);
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		cerr << "last: " << u[ axe_size[ 0] - 1] <<endl;
		}
	else
		{
		for(int i = 0; i < axe_size[0]; i++)
			{
			(TheCubes[i][0][0])->glucose  = 7*cos(0*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(1*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(2*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(3*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(4*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(5*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(17*pi*dx*i/lgx);
			(TheCubes[i][0][0])->glucose += cos(19*pi*dx*i/lgx);
		
			u[ i ] = (TheCubes[i][0][0])->glucose;
			}
		}


	for(int n = 0; n < iteration_indice; n++)
			{
			Update_Glucose(mode);
			#if VERBOSE >= 1
			if(n % 47 == 0)
				cerr << "\r" << 100 * (double)n / (double)(iteration_indice) <<"% 1964";
			#endif
			}
	
	#if VERBOSE >= 1
	cerr << "\n nb iterations: " << iteration_indice << endl;
	#endif
	char filename[100];
	sprintf(filename,"%s/sturm.txt",outputpath);

	std::ofstream myfile(filename);
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;

	//Make Exact Solution
	double lambda = 0.;
	if(bc == "Dirichlet")
		{
		for(int i = 0 ; i < axe_size[0]; i++)
			{
			
			lambda = 0*0*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]   = sin(0*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 1*1*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += 8*sin(1*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 2*2*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(2*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 3*3*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(3*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 4*4*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(4*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 5*5*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(5*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 17*17*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(17*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			lambda = 19*19*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += sin(19*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			
			}
		}
	else
		{
		for(int i = 0 ; i < axe_size[0]; i++)
			{

			lambda = 0*0*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]   = 7*cos(0*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 1*1*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(1*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
	
			lambda = 2*2*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(2*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 3*3*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(3*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 4*4*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(4*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 5*5*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(5*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 17*17*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(17*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			lambda = 19*19*mydiffusion*pi*pi/(lgx*lgx) + fixrate;
			u[i]  += cos(19*pi*dx*i/lgx)*exp(-lambda*dt*iteration_indice);
			cerr << "u[i] = " << u[i] << endl;	
			
			}
		}
	

	if (myfile.is_open())
		{
		for(int i=0; i < axe_size[0]; i += quality_output_frequency)
			{
			myfile << i*dx << "\t";
			myfile << TheCubes[i][cy][cz]->glucose << "\t";
			myfile << u[i] << "\n";	
			}
		myfile.close();
		}
	else{cerr << "Unable to open file qualitycontrol-"<< timestep << endl; exit(1);}	
	}

void Substrate::Parabole(int mode)
	{
	//Gnuplot: plot "parabole.txt"u 1:2 w l, "parabole.txt"u 1:3 w l
	//Iterations
	int themax = 70000;
	//USE SCHALLER AND REMOVE THE TEST OF ZERO CONCENTRATION IN SCHALLER 
	double dirichlet_value = 10;
	string bc = "Dirichlet";
	Glucose.SetMethod(CONSUMPTION_DIFFUSION,BORDERAREBORDER,bc);
	//Glucose.SetMethod(PURE_DIFFUSION,BORDERAREBORDER,"Dirichlet");

	//Glucose.Rate = &GiveMeTheGlucoseRate;
	fixrate = 0.1;
	Glucose.Rate = &FixRate;
	int a = 0;
	if(bc == "Dirichlet"){a = 1;}
	Glucose.BuildMatrix(0,-1,0,0,&Glucose,a);

			
	for(int i = 0; i < axe_size[1];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[0][i][j])->glucose = dirichlet_value;
			(TheCubes[axe_size[0] - 1][i][j])->glucose = dirichlet_value;
			}
		}
	if(DIMENSIONS > 1)
	{
	for(int i = 0; i < axe_size[0];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[i][0][j])->glucose = dirichlet_value;
			(TheCubes[i][axe_size[1] - 1][j])->glucose = dirichlet_value;
			}
		}
	
	
	if(DIMENSIONS > 2)
		{
		for(int i = 0; i < axe_size[0];i++)
			{
			for(int j = 0; j < axe_size[1];j++)
				{
				(TheCubes[i][j][0])->glucose = dirichlet_value;
				(TheCubes[i][j][axe_size[2] - 1])->glucose = dirichlet_value;
				}
			}
		}
	}
	
	for(int n = 0; n < themax; n++)
			{
			Update_Glucose(mode);
			#if VERBOSE >= 1
			if(n % 47 == 0)
				cerr << "\r" << 100 * (double)n / (double)(themax) <<"% 2122";
			#endif
			}
	
	#if VERBOSE >= 1
	cerr << "\n nb iterations: " << themax << endl;
	#endif
	char filename[100];
	sprintf(filename,"%s/parabole.txt",outputpath);

	std::ofstream myfile(filename);
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;

	double D = Glucose_Diffusion/(SPATIAL_UNIT*SPATIAL_UNIT*spacestep*spacestep);

	if (myfile.is_open())
		{
		for(int i=0; i < axe_size[0]; i += quality_output_frequency)
			{
			myfile << i*SPATIAL_UNIT << "\t";
			myfile << TheCubes[i][cy][cz]->glucose << "\t";
			myfile << i*( i - axe_size[0])*fixrate/(2*D) + dirichlet_value << "\n";	
			}
		myfile.close();
		}
	else{cerr << "Unable to open file qualitycontrol-"<< timestep << endl; exit(1);}
		
	}
void Substrate::DirichletZero(int mode)
	{
	//Gnuplot: plot "dirichlet.txt"
	double dirichlet_value = 0;

	Glucose.SetMethod(PURE_DIFFUSION,BORDERAREBORDER,"Dirichlet");

	fixrate = 0;
	Glucose.Rate = &FixRate;
	Glucose.BuildMatrix(0,-1,0,0,&Glucose,1);

			
	for(int i = 0; i < axe_size[1];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[0][i][j])->glucose = dirichlet_value;
			(TheCubes[axe_size[0] - 1][i][j])->glucose = dirichlet_value;
			}
		}
	if(DIMENSIONS > 1)
	{
	for(int i = 0; i < axe_size[0];i++)
		{
		for(int j = 0; j < axe_size[2];j++)
			{
			(TheCubes[i][0][j])->glucose = dirichlet_value;
			(TheCubes[i][axe_size[1] - 1][j])->glucose = dirichlet_value;
			}
		}
	
	
	if(DIMENSIONS > 2)
		{
		for(int i = 0; i < axe_size[0];i++)
			{
			for(int j = 0; j < axe_size[1];j++)
				{
				(TheCubes[i][j][0])->glucose = dirichlet_value;
				(TheCubes[i][j][axe_size[2] - 1])->glucose = dirichlet_value;
				}
			}
		}
	}
	int themax = 35000;
	for(int n = 0; n < themax; n++)
			{
			Update_Glucose(mode);
			#if VERBOSE >= 1
			if(n % 47 == 0)
				cerr << "\r" << 100 * (double)n / (double)(themax) <<"% 2201";
			#endif
			}
	
	#if VERBOSE >= 1
	cerr << "\n nb iterations: " << themax << endl;
	#endif
	char filename[100];
	sprintf(filename,"%s/dirichlet.txt",outputpath);

	std::ofstream myfile(filename);
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;

	if (myfile.is_open())
		{
		for(int i=0; i < axe_size[0]; i += quality_output_frequency)
			{
			myfile << i << "\t";
			myfile << TheCubes[i][cy][cz]->glucose << "\n";		
			}
		myfile.close();
		}
	else{cerr << "Unable to open file qualitycontrol-"<< timestep << endl; exit(1);}
		
	}

void Substrate::TimeStepControl(int mode)
		{
		char filename[100];
		double timestep_memory = timestep;

	int cx = axe_size[0]/2;
	int cy = axe_size[1]/2;
	int cz = axe_size[2]/2;

	if((TheCubes[cx][cy][cz])->getState() == ACTIVE)
		{
		#if VERBOSE >= 5
		cerr << "William: Youpi we have a cell in the center to make the test" << endl;
		#endif
		}
	else	{
		#if VERBOSE >= 1
		cerr << "No cell in the center in x:"<< cx << " y:"<< cy << " z:"<< cy << endl;
		#endif
		exit(1);
		}
	
	
	for(int factor = 1; factor < 100000; factor *= 10)
		{

		timestep = timestep_memory / factor ;
		int QualityNumberOfIterations = (int)(ControlDuration/(timestep));
		centralCellValue = new double[QualityNumberOfIterations*factor];
		#if VERBOSE >= 1
		cerr << "Time Step divided by factor "<< factor << endl;
		#endif
		RebuildMatrix();
		//Initialize_Oxygen();
		Initialize_Glucose();

		

		for(int n = 0; n < QualityNumberOfIterations; n++)
			{
			//Update_Oxygen(mode);
			Update_Glucose(mode);
			centralCellValue[n] = TheCubes[cx][cy][cz]->glucose;
			#if VERBOSE >= 1
			cerr << "\r" << 100 * (double)n / (double)(QualityNumberOfIterations) <<"% 2272";
			#endif
			}

		sprintf(filename,"%s/%i-qualitycontrol-%.1e.txt",outputpath,factor,timestep);
		Output_QualityControl(centralCellValue, factor,filename);
		
		#if VERBOSE >= 1
		cerr << endl;
		#endif
		delete[] centralCellValue;
		}
	timestep = timestep_memory;


		}
		
void Substrate::Output_QualityControl(double *tab, int factor,char *filename)
	{
	std::ofstream myfile(filename);

	if (myfile.is_open())
		{
		int QualityNumberOfIterations = (int)(ControlDuration/(timestep));
		for(int i=0; i < QualityNumberOfIterations; i += quality_output_frequency)
			{
			myfile << i*timestep << "\t";
			myfile << centralCellValue[i] << "\n";		
			}
		myfile.close();
		}
	else{ exit(1);}
	
	}
#endif





