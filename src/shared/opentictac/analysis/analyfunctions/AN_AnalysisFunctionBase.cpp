/*
 * AN_AnalysisFunctionBase.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: benk
 */

#include "AN_AnalysisFunctionBase.hpp"
#include "LA_AssemblyMatrixMap.hpp"
#include "LA_MatrixBasis.hpp"
#include "Netlist.hpp"

AN_AnalysisFunctionBase::AN_AnalysisFunctionBase(AN_Analysis *analysis):
UT_SIMVerboseClass("AnalysisFunction"),
analysis_(analysis)
{
  // no work in the Ctor
  // this->setVerb(9);

  // we use direct pointers
  assemblyMap_ = TICTAC_SPTR<LA_AssemblyMatrixMap>(
      new LA_AssemblyMatrixMap(analysis, analysis_->getGlobalNetlist()));
}

AN_AnalysisFunctionBase::~AN_AnalysisFunctionBase()
{
  // no work in the Dtor (smart pointers should do the job)
}


void AN_AnalysisFunctionBase::prepareAnalysis()
{
  // ---- initialize auxiliary data structures ---
  // the code from here has been moved to the AssemblyMatrixMap class

  // we just only initiate the map, IMPORTANT that matrix should be ready !!!
  assemblyMap_->generateMap();
}

void AN_AnalysisFunctionBase::assemblyMatrixAndRHS(
    LA_MatrixBasis<MYREAL>* matrix,
    Netlist* netlist,
    std::vector<MYREAL> &u,
    std::vector<MYREAL> &res,
    std::vector<MYREAL> &absContribs,
    MYINT &flags)
{
  MYINT deviceIndex, i;

  // local vectors needed for the device evaluation and stamping
  std::vector<MYREAL> localVars(analysis_->getActualMaxMatrixStamp(),0.0);
  std::vector<MYREAL> matrixValues(analysis_->getActualMaxMatrixStamp(),0.0);
  std::vector<MYREAL> rhsValues(analysis_->getActualMaxMatrixStamp(),0.0);
  [[maybe_unused]] MYINT partiID = netlist->getPariID();
  // ----------- OUTPUTS ----------
  MYINT nrMatrixStamp;
  MYINT nrRHSStamp;

  // get the references for efficient matrix assembly
  const std::vector<MYINT>& mapLocalVarsToGlobalVector = assemblyMap_->getMapLocalVarsToGlobalVector();
  const std::vector<MYINT>& mapLocalVarsStartIndex = assemblyMap_->getMapLocalVarsStartIndex();
  const std::vector<MYINT>& mapLocalVarsToGlobalMatrix = assemblyMap_->getMapLocalVarsToGlobalMatrix();
  const std::vector<MYINT>& mapLocalMatrixStartIndex = assemblyMap_->getMapLocalMatrixStartIndex();
  const std::vector<MYINT>& mapLocalVarsToGlobalRHS = assemblyMap_->getMapLocalVarsToGlobalRHS();
  const std::vector<MYINT>& mapLocalRHSStartIndex = assemblyMap_->getMapLocalRHSStartIndex();

  // ---- 1) reset Matrix and RHS to zero --------
  matrix->resetvalues();
  for (i = 0; i < matrix->getNrRows(); i++)
    {
      res[i]         = 0.0;
      absContribs[i] = 0.0;
    }

  SIM_PRINT_L4(verb(), this->analysis_->getAnalysisName() << ": START evaluation loop");
  // ======= 2) Assemble the Matrix and the RHS =============
  for (deviceIndex = 0; deviceIndex < netlist->getNrDevices(); deviceIndex++)
    { // get the device
      Device* device = netlist->getDevice(deviceIndex);
      MYINT nrLocalVars = assemblyMap_->getNrLocalVars(deviceIndex);

      // --- get the local variables from the global ---
      for (MYINT i = 0; i < nrLocalVars; i++) {
          MYINT globInd = mapLocalVarsToGlobalVector[mapLocalVarsStartIndex[deviceIndex] + i];
          // the ground voltage is always zero
          localVars[i] = ((globInd >= 0) ? (u[globInd]) : (0.0));
          SIM_PRINT_L4(verb(), " Local i=" << i << " start=" << mapLocalVarsStartIndex[deviceIndex]
              << " mapI=" << globInd << " localVars[i]=" << localVars[i]);
      }

      // --- call the evaluate method ---
      device->evalDevice( analysis_ , analysis_->getSimulationController(),
          localVars , netlist->getPariID(),
          nrMatrixStamp, nrRHSStamp,
          matrixValues, rhsValues, flags);

      // --- copy the local stamp into the global matrix ----
      for (MYINT i = 0; i < nrMatrixStamp; i++) {
          SIM_PRINT_L5(verb(), "A[" << mapLocalVarsToGlobalMatrix[mapLocalMatrixStartIndex[deviceIndex]+i] << "]+=" << matrixValues[i]);
          SIM_PRINT_L5(verb(),"i = "<<i<<" "<<matrixValues.size());
          SIM_PRINT_L5(verb(),"i = "<<i<<" "<<matrixValues[i]);
          matrix->addContribution(
              mapLocalVarsToGlobalMatrix[mapLocalMatrixStartIndex[deviceIndex] + i],
              matrixValues[i]);
      }
          SIM_PRINT_L5(verb(),"test");
      // --- copy the contributions to RHS, and build absContribs ---
      for (MYINT i = 0; i < nrRHSStamp; i++) {
          MYINT tmp = mapLocalVarsToGlobalRHS[mapLocalRHSStartIndex[deviceIndex] + i];
          SIM_PRINT_L5(verb(), "rhs[" << tmp << "]+=" << rhsValues[i]);
          if (tmp >= 0) {
            res[tmp] += rhsValues[i];
            // here collect the maximal absolute values of the equations, such that
            // the relative tolerance for the Newton method could be measured
            //absContribs[tmp] +=  SIMABS(rhsValues[i]);
            absContribs[tmp] = SIMMAX( SIMABS(rhsValues[i]), absContribs[tmp]);
          }
      }
    } // end evaluation loop
  SIM_PRINT_L4(verb(), this->analysis_->getAnalysisName() << ": END evaluation loop");
}
