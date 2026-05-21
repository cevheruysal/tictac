/*
 * AN_DCAnalysis.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#include "AN_DCAnalysis.hpp"
#include "LS_SolverFactory.hpp"
#include "Netlist.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "IN_SimOptions.hpp"
#include "NS_NonlinearSolver.hpp"
#include "AN_DCFunction.hpp"
#include "OUT_MessageHandle.hpp"
#include "AN_Analysis.hpp"

AN_DCAnalysis::AN_DCAnalysis(Netlist *netlist, IN_SimOptions* simulationOptions,
    TICTAC_SPTR<OUT_WaveFormPlotter> plotter) :
AN_Analysis(netlist, DC_TYPE, simulationOptions, plotter)
{
  // TODO: ... add later code
  // this->setVerb(6);

  dcLinearSolver_ = NULL;
}

AN_DCAnalysis::~AN_DCAnalysis()
{
  // IMPORTANT it is the task of each analysis to clean up the memory
  //clean up: solver, matrix, matrix-map
  if (dcLinearSolver_ == NULL) return;

  if (dcLinearSolver_->getSolverMatrix()->getMatrixMap() != NULL)
    delete dcLinearSolver_->getSolverMatrix()->getMatrixMap();

  // delete the matrix (which is stored in the solver)
  if (dcLinearSolver_->getSolverMatrix() != NULL)
    delete dcLinearSolver_->getSolverMatrix();

  // delete solver
  if (dcLinearSolver_ != NULL)
    delete dcLinearSolver_;
}

void AN_DCAnalysis::executeAnalysis([[maybe_unused]] std::vector<MYREAL>& stateVector)
{
  SIM_PRINT_L2(verb(), "DCAnalysis::executeAnalysis");
  LS_SolverFactory    solverFactory;
  NS_NonlinearSolver  nonlinearSolver;
  // global parameters for the nonlinear solver TODO: setup these parameters properly
  MYINT maxIter = 100;
  MYREAL absTol = 1e-8, realTol = 1e-5;
  // MYREAL absTol = 1e-5, realTol = 1e-3;
  MYREAL dampingFact = 1.0; // for student version do no damping even for DC analysis
  NS_NonlinearSolverStatus nonlinSolverStatus;
  AN_DCFunction       dcfunction(this);

  // IMPORTANT building the netlist for this analysis
  this->globalNetlist_->buildNetlist(this);

  // create a linear solver (including)
  dcLinearSolver_ = solverFactory.factorSolverReal(this, this->globalNetlist_, this->simulationOptions_);
  setMaxMatrixStamp( solverFactory.getActualMaximalMatrixStampSize() );

  // once the matrix is ready update the controllers
  this->getSimulationController()->updateControllers(
      globalNetlist_, this, this->getSimulationOption(),
      dcLinearSolver_->getSolverMatrix()->getMatrixMap());

  // set the parameters for the non-linear solver
  nonlinearSolver.setParameters(maxIter, absTol, realTol, dampingFact);

  // initialize the setup run, finalize
  dcfunction.prepareAnalysisRun();

  // set initial guess to zero
  actualState_.resize( dcfunction.getInputDimension(), 0.0);
  // apply NODESETs for the initial condition of DC
  this->applyInitialCondition(actualState_);

  SIM_PROTOCOL_L0( DC_TYPE , "Start computing DC point ... ");
  // --- SOLVING THE NONLINEAR PROBLEM ---
  nonlinearSolver.solveNonLinearProblem( &dcfunction , actualState_, nonlinSolverStatus);

  // see if this was successful
  if (nonlinSolverStatus.converged) {
      // DC converged
      SIM_PROTOCOL_L0( DC_TYPE , "DC Converged within " << nonlinSolverStatus.nrIteration );
  } else {
      // DC not converged
      SIM_PROTOCOL_WARNING( DC_TYPE , " DC has NOT Converged within " << nonlinSolverStatus.nrIteration );
  }

  // print DC state in the protocol file
  OUT_MessageHandle::printDCState(this, dcLinearSolver_, this->globalNetlist_, this->actualState_);
}


// ==============================
void AN_DCAnalysis::applyInitialCondition(std::vector<MYREAL> &unknownVector)
{
  for (MYINT i = 0; i < this->nrInitialConditions_; i++) {
      MYINT unkID = this->getRealSolver(0)->getSolverMatrix()->getMatrixMap()->getColIndexForUnkID( this->initialConditionNode_[i]);
      SIM_PRINT_L2(verb(), ".IC node ID=" <<  this->initialConditionNode_[i] << " unknownID="
          << unkID << " prevVal=" << unknownVector[unkID] << " newVal=" << this->initialConditionValue_[i]);
      unknownVector[unkID] = this->initialConditionValue_[i];
  }
  // document if there are any NODESETs
  if ( nrInitialConditions_ > 0)
    SIM_PROTOCOL_L0( DC_TYPE , "Setting " << nrInitialConditions_ << " .NODESETs ...");
}
