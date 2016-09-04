#ifndef FAST_cInterface_h
#define FAST_cInterface_h

#include "FAST_Library.h"
#include "sys/stat.h"
#include "math.h"
#include <iostream>
#include <string>
#include <cstring>
#include <malloc.h>
#include <stdexcept>
#include "yaml-cpp/yaml.h"
#include "dlfcn.h"
#ifdef HAVE_MPI
  #include "mpi.h"
#endif
class FAST_cInterface {

 private:

  bool dryRun;        // If this is true, class will simply go through allocation and deallocation of turbine data
  int nTurbinesProc;
  int nTurbinesGlob;
  bool restart;
  double dtFAST;
  double tMax;
  float ** TurbinePos;
  int * TurbID;
  char ** FASTInputFileName;
  char ** CheckpointFileRoot;
  double tStart, tEnd;
  int nt_global;           
  int ntStart, ntEnd;      // The time step to start and end the FAST simulation
  int nEveryCheckPoint;    // Check point files will be written every 'nEveryCheckPoint' time steps
  int * numBlades;           // Number of blades
  int * numElementsPerBlade;
  int * numTwrElements;
  int * numScOutputs;  // # outputs from the supercontroller == # inputs to the controller == NumSC2Ctrl
  int * numScInputs;   // # inputs to the supercontroller == # outputs from the controller == NumCtrl2SC
  
  OpFM_InputType_t ** cDriver_Input_from_FAST;
  OpFM_OutputType_t ** cDriver_Output_to_FAST;

  // Turbine Number is DIFFERENT from TurbID. Turbine Number simply runs from 0:n-1 locally and globally.
  std::map<int, int> turbineMapGlobToProc; // Mapping global turbine number to processor
  std::map<int, int> turbineMapProcToGlob; // Mapping local to global turbine number
  std::set<int> turbineSetProcs; // Set of processors containing atleast one turbine 
  int * turbineProcs; // Same as the turbineSetProcs, but as an integer array

  //Supercontroller stuff
  bool scStatus;
  std::string scLibFile;
  // Dynamic load stuff copied from 'C++ dlopen mini HOWTO' on tldp.org
  void *scLibHandle ;
  typedef void (*DISCON_sc_t)(OpFM_InputType_t* , OpFM_OutputType_t*);
  DISCON_sc_t DISCON_SuperController;

#ifdef HAVE_MPI
  int fastMPIGroupSize;
  MPI_Group fastMPIGroup;
  MPI_Comm  fastMPIComm;
  int fastMPIRank;

  MPI_Group worldMPIGroup;
  int worldMPIRank;
#endif

  int ErrStat;
  char ErrMsg[INTERFACE_STRING_LENGTH];  // make sure this is the same size as IntfStrLen in FAST_Library.f90

 public: 

  // Constructor 
  FAST_cInterface() ;
  
  // Destructor
  ~FAST_cInterface() {} ;
  
  int readInputFile(std::string cInterfaceInputFile);  
  int init();
  int step();
  int get_ntStart() { return ntStart; }
  int get_ntEnd() { return ntEnd; }
  bool isDryRun() { return dryRun; }
  void end();

 private:

  void checkError(const int ErrStat, const char * ErrMsg);
  void setOutputsToFAST(OpFM_InputType_t* cDriver_Input_from_FAST, OpFM_OutputType_t* cDriver_Output_to_FAST) ;

  int cDriverRestart();
  inline bool checkFileExists(const std::string& name);

  void readTurbineData(int iTurb, YAML::Node turbNode);
  void allocateInputData();
  void allocateTurbinesToProcs(YAML::Node cDriverNode);
  void loadSuperController(YAML::Node c);
};

#endif
