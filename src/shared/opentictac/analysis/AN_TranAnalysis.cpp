/*
 * AN_TranAnalysis.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#include "AN_TranAnalysis.hpp"
#include "AN_TranFunction.hpp"
#include "Netlist.hpp"
#include "NS_NonlinearSolver.hpp"
#include "LS_SolverFactory.hpp"
#include "OUT_MessageHandle.hpp"
#include "AN_Tran_TimeStepController.hpp"

AN_TranAnalysis::AN_TranAnalysis(
    Netlist *netlist, MYREAL startT, MYREAL endT,
    IN_SimOptions* simulationOptions,
    TICTAC_SPTR<OUT_WaveFormPlotter> plotter):
    AN_Analysis(netlist, TRAN_TYPE, simulationOptions, plotter),
    startTime_(startT), endTime_(endT)
{
  //this->setVerb(0);
  actualTime_ = 0.0;
  tranLinearSolver_ = NULL;

  // this is initialized as expected
  actualDeltaTime_ = 0.0;
  oldDeltaTime_ = 0.0;
  stepType_ = NORMAL_STEP;
  nextstepType_ = NORMAL_STEP;
  postTranStepDevices_.resize(0);
  nrPostTranDevices_ = 0;
  timeStepIndex_ = 0;
  timeStepAfterBreakPoint_ = 0;
  actualStartDeltaTime_ = 0.0;

}

AN_TranAnalysis::~AN_TranAnalysis()
{
  // IMPORTANT it is the task of each analysis to clean up the memory
  //clean up: solver, matrix, matrix-map
  // TODO: this could be done in a better structured way, with smart pointers !!!!

  if (tranLinearSolver_ == NULL) return;

  if (tranLinearSolver_->getSolverMatrix()->getMatrixMap() != NULL)
    delete tranLinearSolver_->getSolverMatrix()->getMatrixMap();

  // delete the matrix (which is stored in the solver)
  if (tranLinearSolver_->getSolverMatrix() != NULL)
    delete tranLinearSolver_->getSolverMatrix();

  // delete solver
  if (tranLinearSolver_ != NULL)
    delete tranLinearSolver_;
}

void AN_TranAnalysis::executeAnalysis([[maybe_unused]] std::vector<MYREAL>& stateVector)
{
  SIM_PROTOCOL_L0( TRAN_TYPE , "TranAnalysis::executeAnalysis from " << startTime_ << " to " << endTime_);
  LS_SolverFactory    solverFactory;
  NS_NonlinearSolver  nonlinearSolver;
  // global parameters for the nonlinear solver TODO: setup these parameters properly
  MYINT maxIter = 30; // maximal number of iterations
  MYREAL absTol = 1e-7, realTol = 1e-5, dampingFact = 1.0; // no damping in transient analysis

  NS_NonlinearSolverStatus nonlinSolverStatus;
  AN_TranFunction       tranFunction(this);
  AN_DTController*      ddtControler = NULL;
  AN_Tran_TimeStepController  *timeStepSizeController = NULL;
  MYINT               i, innerItID = 0, nrNoConvergance = 0;
  bool                  repeatStep = false;
  std::vector<MYREAL>     dummyState(this->actualState_.size(),0.0);

  actualDeltaTime_ = 0.0;
  oldDeltaTime_ = 0.0;
  stepType_ = NORMAL_STEP;
  nextstepType_ = NORMAL_STEP;
  nrPostTranDevices_ = 0;
  timeStepIndex_ = 0;
  timeStepAfterBreakPoint_ = 0;
  postTranStepDevices_.clear();

  // init the step size control parameter struct
  timeStepCtr_ = this->getSimulationOption()->getTimeStepSizeControlRecord();
  // set the deltaMax
  timeStepCtr_.maximalDeltaT_ = SIMMIN(timeStepCtr_.maximalDeltaT_, SIMABS(this->endTime_-this->startTime_)/50.0);

  // IMPORTANT building the netlist for this analysis
  SIM_PROTOCOL_L0( TRAN_TYPE , "Building Netlist for transient analysis ...");
  this->globalNetlist_->buildNetlist(this);
  // start plotting
  if (this->getAnalysisPlotter()) this->getAnalysisPlotter()->startRecording();

  // =========== SOLVER FACTORY =======================
  SIM_PROTOCOL_L0( TRAN_TYPE , "Getting the solver for the transient analysis ...");
  this->tranLinearSolver_ = solverFactory.factorSolverReal(this, this->globalNetlist_, this->simulationOptions_);
  // store the maximum number of matrix stamp device
  setMaxMatrixStamp( solverFactory.getActualMaximalMatrixStampSize() );

  // IMPORTANT: once the solver is ready
  // and the matrix is ready update the controllers
  this->getSimulationController()->updateControllers(
      globalNetlist_, this, this->getSimulationOption(),
      tranLinearSolver_->getSolverMatrix()->getMatrixMap());
  // get the DDT controller
  ddtControler = this->getSimulationController()->getDDTController();
  timeStepSizeController = this->getSimulationController()->getTimeStepController();

  // here we collect all devices which might cause breakpoints or need post eval after
  // each transient step
  for (i = 0; i < globalNetlist_->getNrDevices(); i++) {
      if (globalNetlist_->getDevice(i)->needsTranPostProcesseing()) {
          postTranStepDevices_.push_back(i);
          nrPostTranDevices_++;
      }
  }

  // resize the solution vector
  actualState_.resize(tranLinearSolver_->getSolverMatrix()->getNrColumns(), 0.0);
  dummyState.resize(tranLinearSolver_->getSolverMatrix()->getNrColumns(), 0.0);
  // get the solution from the [revious analysis
  if (this->getPreviousAnalysis() != NULL)
    AN_Analysis::copyOneStateToOther( this->getPreviousAnalysis(), this, this->globalNetlist_);

  // apply .ICs for the initial condition of the TRAN analysis
  this->applyInitialCondition(actualState_);

  // initialize the setup run, finalize
  tranFunction.prepareAnalysisRun();

  // prepare the first fake time step (the fake time step is needed only for initialization purposes)
  ddtControler->prepateFirstTranStep();
  nonlinearSolver.setParameters(0, absTol, realTol, dampingFact);

  SIM_PROTOCOL_L0( TRAN_TYPE , " TIME 0 initialization ");
  // --- SOLVING THE NONLINEAR PROBLEM ---
  nonlinearSolver.solveNonLinearProblem( &tranFunction , actualState_, nonlinSolverStatus);

  actualDeltaTime_ = timeStepCtr_.startDeltaT_;
  actualStartDeltaTime_ = timeStepCtr_.startDeltaT_;
  // prepare the second real time step
  ddtControler->prepateSecondTranStep(actualDeltaTime_);

  // set the parameters for the non-linear solver
  nonlinearSolver.setParameters(maxIter, absTol, realTol, dampingFact);

  // print DC state in the protocol file
  actualTime_ = 0.0;
  timeStepIndex_ = 0;
  timeStepSizeController->setActualVector(actualState_, actualTime_);
  // plot initial state
  treatPlotting();

  // the actual time is the delta(T)
  actualTime_ = actualDeltaTime_;
  this->getSimulationController()->setActTime(actualTime_);

// ============================== START TIME LOOP ====================================
  while ( actualTime_ <= (endTime_+SIM_ABS_TIME_STEP_LIMIT) )
    {
      // initialization
      innerItID = 0;
      nrNoConvergance = 0;
      //SIM_PROTOCOL_L0( TRAN_TYPE , "Time step index = " << timeStepIndex_);
      // -------------- LOOP TO ENFORCE CONVERGANCE ----------------
      do
        {
          repeatStep = false;
          // use the predictor, only NOT in the first time step, and not when we have no convs
          if (timeStepIndex_ > 0 && (innerItID < 2)) {
              SIM_PRINT_L2(verb(), "TRAN: Call predictor ... timeStepIndex=" << timeStepIndex_);
              timeStepSizeController->predictNextTimeStep(actualState_, actualTime_ );
          }
          // else { this->applyInitialCondition(actualState_); }

          // ---------- SOLVING THE NONLINEAR PROBLEM --------------
          nonlinearSolver.solveNonLinearProblem( &tranFunction , actualState_, nonlinSolverStatus);
          // store old time step before setting the new one
          oldDeltaTime_ = actualDeltaTime_;

          // check if this has converged
          repeatStep = !nonlinSolverStatus.converged;
          if (repeatStep) {
              //OUT_MessageHandle::printDCState(this, tranLinearSolver_, netlist_, actualState_);
              SIM_PROTOCOL_L0( TRAN_TYPE , "NO CONV  nr. it = " << nonlinSolverStatus.nrIteration
                  << " nrNoConvergance=" << nrNoConvergance);
              nrNoConvergance++;
            }
          else { // --- Accepted time step ---------
              // set the accepted time step for the timestep size controller
              SIM_PRINT_L2(verb(), "TRAN: Set the implicit solution for the predictor ... time=" << (actualTime_)
                  << " timeStepAfterBreakPoint=" <<  timeStepAfterBreakPoint_ << " type=" << stepType_);

              // make a difference if we are repeating this loop,
              // because then we need to reset the actual vector
              if (innerItID <= 0)
                timeStepSizeController->setActualVector(actualState_, actualTime_);
              else
                timeStepSizeController->resetActualVector(actualState_, actualTime_);

              // call the function which handles the next time step size and also the type (BP or not)
              // furthermore this checks wheather LTE tolerances
              repeatStep = nextTimeStepSizeHandling();
              nrNoConvergance = 0;
            }

          // check if this step should be repeated, if yes then reduce time step
          // (and/or also make additional changes)
          // ======= REFUSE OR NO_CONV TIME STEP ==============
          if (repeatStep)
            {
              // take care for NO CONVS ...
              if (actualDeltaTime_ > timeStepCtr_.minimalDeltaT_)
                {
                  actualTime_ = actualTime_ - oldDeltaTime_;
                  actualDeltaTime_ = (oldDeltaTime_/timeStepCtr_.factorToReduceTimeStep_);
                  actualTime_ = actualTime_ + actualDeltaTime_;
                }
              // set the actual simulation time
              this->getSimulationController()->setActTime(actualTime_);
              // reset predictor and unknown history
              actualState_ = timeStepSizeController->getPrevState();
              timeStepSizeController->resetActualVector(actualState_, actualTime_);
              // also reset DDT controller
              ddtControler->resetNextTimeStamp(timeStepIndex_, actualTime_, actualDeltaTime_, stepType_);
              // limit everything to delmin
              if (actualDeltaTime_ < timeStepCtr_.minimalDeltaT_)
                actualDeltaTime_ = timeStepCtr_.minimalDeltaT_;

              // TODO: we could also have other alternative strategies to handle this
              // - increase number of iterations + add damping factor
              // - relax tolerances for the nonlinear solver
            }
          // increment the counter to see if this step will be repeated
          innerItID++;

          // ------------- RESCUE if there are just too many no convs -----
          // in case of too many no convergance just accept this step
          if (nrNoConvergance > 10)
            break;
        } // -------- LOOP TO ENSURE that we converge --------------
      while (repeatStep);

      // ------- This is now an accepted time step ---------------
      SIM_PRINT_L2(verb(), "TRAN: accepted time step" );

      // ----- from this point this is an accepted time step  -----
      // early break when we are finished
      if (actualDeltaTime_ < SIM_ABS_TIME_STEP_LIMIT)  break;

      // write into the protocol
      SIM_PROTOCOL_L0( TRAN_TYPE , " TIME " << std::setprecision(5) << std::scientific << (actualTime_)
          << "   dt = " << actualDeltaTime_ << "   nr.it=" << nonlinSolverStatus.nrIteration);

      // ===== post processing the actual state at the actual time ======
      // plotting the actual state at the actual time
      treatPlotting();

      // ====== preparing next time step ================
      // after an accepted time step increase the time
      actualTime_ = actualTime_ + actualDeltaTime_;
      timeStepAfterBreakPoint_ = ( (stepType_==BREAKPOINT_STEP) ? (0): (timeStepAfterBreakPoint_+1));
      timeStepIndex_++;
      // tell the DDT controller that next time step is coming
      ddtControler->nextTimeStamp( timeStepIndex_, actualTime_, stepType_);
      // set the actual simulation time
      this->getSimulationController()->setActTime(actualTime_);
    }
// =================== END TIME LOOP ===================================

  // plotting final solution at the final time
  treatPlotting();
  // write into the protocol
  SIM_PROTOCOL_L0( TRAN_TYPE , " TIME " << std::setprecision(5) << std::scientific << actualTime_
      << "   dt = " << actualDeltaTime_ ); //<< "   nr.it=" << nonlinSolverStatus.nrIteration);
  // close the plotting file
  if (this->getAnalysisPlotter()) this->getAnalysisPlotter()->endPlotting();
  // print to protocol the finish
  SIM_PROTOCOL_L0( TRAN_TYPE , "END TRAN  nr.time steps=" << timeStepIndex_);
  // print DC state in the protocol file
  OUT_MessageHandle::printDCState(this, tranLinearSolver_, globalNetlist_, actualState_);
}



// ==============================
void AN_TranAnalysis::treatPlotting()
{
  if (this->getAnalysisPlotter()) {
      // actualize plotting values
      this->getAnalysisPlotter()->actualizeValues(this->globalNetlist_, actualState_,
        tranLinearSolver_->getSolverMatrix()->getMatrixMap(),
        this->getSimulationController(), actualTime_);
      // flush into the output the actual line
      this->getAnalysisPlotter()->flushActualLine();
  }
}


// ==============================
void AN_TranAnalysis::applyInitialCondition(std::vector<MYREAL> &unknownVector)
{
  for (MYINT i = 0; i < this->nrInitialConditions_; i++) {
      MYINT unkID = this->getRealSolver(0)->getSolverMatrix()->getMatrixMap()->getColIndexForUnkID( this->initialConditionNode_[i]);
      SIM_PRINT_L2(verb(), ".IC node ID=" <<  this->initialConditionNode_[i] << " unknownID="
          << unkID << " prevVal=" << unknownVector[unkID] << " newVal=" << this->initialConditionValue_[i]);
      unknownVector[unkID] = this->initialConditionValue_[i];
  }
  if ( nrInitialConditions_ > 0)
    SIM_PROTOCOL_L0( TRAN_TYPE , "Setting " << nrInitialConditions_ << " .ICs ...");
}


// ==============================
bool AN_TranAnalysis::nextTimeStepSizeHandling()
{

  AN_Tran_TimeStepController  *timeStepSizeController =
      this->getSimulationController()->getTimeStepController();
  bool refuse = false;
  MYREAL maxLTEFact;
  // make an LTE check, if fails than repeat reduce actual time step
  timeStepSizeController->computeLTEandnewTimeStep( actualState_, actualDeltaTime_, actualDeltaTime_, maxLTEFact, refuse);
  // if flag set accordingly than just return true, meaning
  if (refuse)
    {
      SIM_PROTOCOL_L0( TRAN_TYPE , "REFUSE old=" << oldDeltaTime_ << " new=" << actualDeltaTime_);
      return true;
    }

  // around the breakpoint use the old time step
  if (timeStepAfterBreakPoint_ <= 1) {actualDeltaTime_ = oldDeltaTime_;}

  // delmax limitation
  actualDeltaTime_ = SIMMIN( timeStepCtr_.maximalDeltaT_, actualDeltaTime_);

  SIM_PRINT_L2(verb(), "TYPE TP: " <<  nextstepType_ << " actualDeltaTime=" << actualDeltaTime_);
  if (nextstepType_ == BREAKPOINT_STEP)
    { // when we have a breakpoint then have a better strategy for the next time step
      // the device which caused should tell which should be the starting delta T
      actualDeltaTime_ = actualStartDeltaTime_;
    }
  SIM_PRINT_L2(verb(), "TYPE TP: " <<  nextstepType_ << " actualDeltaTime=" << actualDeltaTime_);

  // LIMIT the increase in the time step
  actualDeltaTime_ = SIMMIN(timeStepCtr_.maximalTimeStepSizeIncrease_ * oldDeltaTime_, actualDeltaTime_);

  SIM_PRINT_L2(verb(), "TRAN: next time step ... actualDeltaTime=" << actualDeltaTime_);
  // decide if we have a breakpoint in the next time step
  MYINT flags = 0, i;
  actualStartDeltaTime_ = actualDeltaTime_;
  for (i = 0; i < nrPostTranDevices_; i++)
    {
      SIM_PRINT_L4(verb(), "call postProcessing for device = " << postTranStepDevices_[i]);
      // all devices which are registered are now called
      globalNetlist_->getDevice(postTranStepDevices_[i])->tranPostProcess(
          this->getSimulationController(),actualTime_,
          actualDeltaTime_, flags, actualStartDeltaTime_);
    }

  // limit the time step when we arrive at the end
  if (actualTime_ + actualDeltaTime_ > endTime_)
    actualDeltaTime_ = endTime_ - actualTime_;
  // hit exactly the end time
  if (actualTime_ + actualDeltaTime_ + SIM_ABS_TIME_STEP_LIMIT > endTime_)
    actualDeltaTime_ = endTime_ - actualTime_;



   stepType_ = NORMAL_STEP;
   nextstepType_ = NORMAL_STEP;
  return false;

  // false means this time step is now accepted
  return false;

}
