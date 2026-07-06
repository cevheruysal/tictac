/*
 * AN_Tran_TimeStepController.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: benk
 */

#include "AN_Tran_TimeStepController.hpp"
#include "IN_SimOptions.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "Netlist.hpp"
#include "AN_DTController.hpp"


AN_Tran_TimeStepController::AN_Tran_TimeStepController(
    MYINT nrUnknowns, const IN_SimOptions* options ,
    const Netlist* netlist, const LA_MatrixNetlistMap *map,
    const AN_DTController *dtContrl) :
    UT_SIMVerboseClass("TimeStepController"),
    dtContrl_(dtContrl), nrMaxHistory_(2)//, netlist_(netlist)
{
  // Initialize the vector
  //this->setVerb(0);
  dampingFactor_ = 0.5; // TODO: this could be set from the configuration file
  maxDTIncrease_ = 2.0;
  nrUnknowns_ = nrUnknowns;
  unknownHistory_.resize(nrMaxHistory_);
  timeStamps_.resize(nrMaxHistory_);
  swapVector_.resize(nrMaxHistory_);
  actualExtrapolation_.resize(nrUnknowns_, 0.0);

  for (MYINT i = 0; i < nrMaxHistory_; i++) {
      unknownHistory_[i].resize(nrUnknowns_);
      timeStamps_[i] = 0.0;
      swapVector_[i] = i;
  }

  // get and set the tolerances for currents and voltages
  timeStepControlRec_ = &(options->getTimeStepSizeControlRecord());
  targetEpsilon_ = timeStepControlRec_->targetEpsilon_;
  epsilonFactorPerUnknown_.resize(nrUnknowns_, 1.0);
  for (MYINT i = 0; i < nrUnknowns_; i++) {
      MYINT indx = map->getUnkIDForColIndex(i);
      if ( indx >= netlist->getNrNodesNoGnd() )
        epsilonFactorPerUnknown_[i] = timeStepControlRec_->currentEpsFact_;
      else
        epsilonFactorPerUnknown_[i] = timeStepControlRec_->voltageEpsFact_;
  }

  nrValidTimeStep_ = 0;
  actualExtrapolationTime_ = 0.0;
}

AN_Tran_TimeStepController::~AN_Tran_TimeStepController()
{
  // no explicit delete needed
}


void AN_Tran_TimeStepController::setActualVector( std::vector<MYREAL> &unkVector, MYREAL actualTime)
{
  SIM_PRINT_L3(verb(), "TimeStepController::setActualVector");
  // the 0-th index should be always the actual
  if (nrValidTimeStep_ > 0)
    { // only when there is at least one history
      MYINT tmp = swapVector_[nrMaxHistory_-1];
      for (MYINT i = nrMaxHistory_ - 1; i > 0; i--) {
          swapVector_[i] = swapVector_[i-1];
      }
      swapVector_[0] = tmp;
    }
  SIM_PRINT_L2(verb(), "swapVector_[0] = " << swapVector_[0] << " swapVector_[1]=" << swapVector_[1]);
  // set the information
  unknownHistory_[swapVector_[0]] = unkVector;
  timeStamps_[swapVector_[0]] = actualTime;
  SIM_PRINT_L5(verb(), "swapVector_[0]=" << swapVector_[0] << " actualTime=" << actualTime);
  // increase the number of valid time steps
  nrValidTimeStep_ = ((nrValidTimeStep_ < nrMaxHistory_) ? (nrValidTimeStep_+1): nrMaxHistory_ );
}


void AN_Tran_TimeStepController::resetActualVector(std::vector<MYREAL> &unkVector, MYREAL resetTime)
{ // just set the actual time step information
  SIM_PRINT_L3(verb(), "TimeStepController::resetActualVector");
  timeStamps_[swapVector_[0]] = resetTime;
  unknownHistory_[swapVector_[0]] = unkVector;
}


void AN_Tran_TimeStepController::predictNextTimeStep(
    std::vector<MYREAL> &unkVector, MYREAL actualTime )
{
  SIM_PRINT_L3(verb(), "TimeStepController::predictNextTimeStep");
  // --- actual make only a linear extrapolation ---
  // test if nrValidTimeStep_ is high enough, and if the "actualTime" has higher than
  SIM_ERROR_COND_STOP( actualTime < timeStamps_[swapVector_[0]], " wrong time step actualTime=" << actualTime
      << " lastTS=" << timeStamps_[swapVector_[0]] << " swapVector_[0]=" << swapVector_[0]);
  SIM_ERROR_COND_STOP( nrValidTimeStep_ <= 0, " not enough time steps for the predictor");
  // previous time steps
  MYINT i = 0;
  MYREAL one_over_deltaT = 1.0/(SIMABS(timeStamps_[swapVector_[0]] -timeStamps_[swapVector_[1]]));
  MYREAL predictDeltaT = actualTime - timeStamps_[swapVector_[0]];
  for (i = 0; i < nrUnknowns_; i++){
      // here we compute the gradient
      actualExtrapolation_[i] = one_over_deltaT * ( unknownHistory_[swapVector_[0]][i] - unknownHistory_[swapVector_[1]][i]);
      // predictor[i] = u_{i} + dt * du_{i}/dt -> linear extrapolation
      actualExtrapolation_[i] = unknownHistory_[swapVector_[0]][i] + predictDeltaT * actualExtrapolation_[i];
      SIM_PRINT_L5(verb(), "actualExtrapolation_[" << i << "] = " << actualExtrapolation_[i] << " , unkVector["<<i<<"]=" << unkVector[i]);
      unkVector[i] = actualExtrapolation_[i];
  }
}

void AN_Tran_TimeStepController::computeLTEandnewTimeStep(
    [[maybe_unused]] std::vector<MYREAL> &unkVector, MYREAL oldDT,
    MYREAL &deltaT, MYREAL &maxLTEFact, bool &rejectStep )
{

  // this is just the reamaining of previous assignment where we all computed with min deltaT
  deltaT = timeStepControlRec_->minimalDeltaT_;

  rejectStep = false;
  MYINT i, nrDDT = dtContrl_->getNrDDTs();
  maxLTEFact = 0.0; // This is maximum LTE_{i}/TOL_{i}

  /*=========== START STUDENT PROJECT A2 ===================== */

  for (i = 0; i < nrDDT; ++i)
    {
      MYREAL lte_i = dtContrl_->getLTE_DDTX(i);
      MYREAL tol_i = timeStepControlRec_->chargeTRTOL_ * (timeStepControlRec_->chargeAbsTol_
          + timeStepControlRec_->chargeRelTol_ * SIMABS( dtContrl_->getDDT(i) ) );
      MYREAL tol_i_heur = timeStepControlRec_->chargeHeuristicRelTol_ *
                     SIMMAX( SIMABS(dtContrl_->getDDTX(i)) , timeStepControlRec_->chargeHeuristicAbsTol_ )
                     / dtContrl_->getDeltaT();
      lte_i = SIMABS(lte_i);
      tol_i = SIMMAX( tol_i, tol_i_heur);
      maxLTEFact = SIMMAX(maxLTEFact, ( lte_i / tol_i) );
      SIM_PRINT_L3(verb(), "i=" << i << " lte_i=" << lte_i << " tol_i=" << tol_i << " DDT=" << dtContrl_->getDDT(i) );
    }
 
  // if this is too much then set the flag correctly
  if (maxLTEFact > timeStepControlRec_->maxChargeLTEFactor_) {
      // in case of refuse already reset deltaT
      if (oldDT > 1.1*timeStepControlRec_->minimalDeltaT_ )
        rejectStep = true;
  }

  // compute the length of the new time step
  if (maxLTEFact < 1E-20)
    maxLTEFact = 1E-20;
  deltaT = dampingFactor_ * oldDT * pow( 1.0/maxLTEFact , 1.0/(dtContrl_->getIntegrationOrder() + 1.0));
  MYREAL pdt = deltaT;
 
  // do not allow a too much increase, this should be limited by a factor
  if (deltaT > timeStepControlRec_->maximalTimeStepSizeIncrease_*oldDT)
    deltaT = timeStepControlRec_->maximalTimeStepSizeIncrease_*oldDT;
  // we limit the time step to min and max dt
  if (deltaT < timeStepControlRec_->minimalDeltaT_)
    deltaT = timeStepControlRec_->minimalDeltaT_;
  if (deltaT > timeStepControlRec_->maximalDeltaT_)
    deltaT = timeStepControlRec_->maximalDeltaT_;

  SIM_PRINT_L3(verb(), "TimeStepController::computeLTEandnewTimeStep oldDT=" << oldDT
      << " deltaT=" << deltaT << " maxLTEFact=" << maxLTEFact << " rejectStep=" << rejectStep << " pdt=" << pdt);

  /*=========== END STUDENT PROJECT A2 ===================== */


  /*=========== START STUDENT PROJECT AA ===================== */
/*
  // here how build the maximum norm between the predicted value and the corrected value
  SIM_PRINT_L3(verb(), "TimeStepController::computeLTEandnewTimeStep");
  MYREAL maxNorm_mul_factor = 1E-29;
  MYREAL previousDT = SIMABS(timeStamps_[swapVector_[0]] -timeStamps_[swapVector_[1]]);
  MYINT i = 0;
  for (i = 0; i < nrUnknowns_; i++)
    {
      MYREAL tmpDiff      = SIMABS((unkVector[i] - actualExtrapolation_[i]));
      MYREAL tmpMagnitude = SIMMAX( 1E-29, (SIMABS(unkVector[i]) + SIMABS(actualExtrapolation_[i])) );
      MYREAL tmpMax       = SIMMAX(tmpDiff, tmpDiff/tmpMagnitude);
      // TODO: THIS NEEDS TO BE REWORKED !!! to consider relative and absolute changes separately
      // TODO: in addition charge (DDT) based time step control could also help
      maxNorm_mul_factor =  SIMMAX( maxNorm_mul_factor , epsilonFactorPerUnknown_[i] * tmpMax );
      SIM_PRINT_L5(verb(), "i=" << i << " maxNorm_mul_factor= " << maxNorm_mul_factor << " diff=" << tmpMax);
      SIM_PRINT_L5(verb(), "actualExtrapolation_[i]=" << actualExtrapolation_[i]
                        << " unkVector[i]= " << unkVector[i] << " e[i]=" << epsilonFactorPerUnknown_[i]);
    }
  // here is the simple LTE control
  deltaT = previousDT * dampingFactor_* (targetEpsilon_/maxNorm_mul_factor);
  // limit the time step increase by a factor (usually is a factor 2.0)
  deltaT = SIMMIN( maxDTIncrease_*previousDT, deltaT);
  // print the proposed time step
  SIM_PRINT_L2(verb(), "proposeNextTimeStep = " << deltaT << " previousDT=" << previousDT);
*/
  /*=========== END STUDENT PROJECT AA ===================== */

}
