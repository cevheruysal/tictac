/*
 * SimOptions.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: benk
 */

#include "IN_SimOptions.hpp"

IN_SimOptions::IN_SimOptions()
{
  // IMPORTANT: these default values should be configurable when the netlist is loaded !!!
  //            Those default should matter, not these here ...

  timeStepControlRec_.currentEpsFact_ = 3E+0;
  timeStepControlRec_.voltageEpsFact_ = 1E-3;
  timeStepControlRec_.targetEpsilon_ = 1E-2; //1E-6
  timeStepControlRec_.maximalTimeStepSizeIncrease_ = 2.0;
  timeStepControlRec_.minimalDeltaT_ = 1E-12;
  timeStepControlRec_.maximalDeltaT_ = 1E-3;
  timeStepControlRec_.startDeltaT_= 1E-12;
  timeStepControlRec_.factorToReduceTimeStep_ = 10.0;
  timeStepControlRec_.factorToRefuseTimeStep_ = 6.0;

  // LTE charge tolerances
  timeStepControlRec_.chargeRelTol_ = 1E-3;
  timeStepControlRec_.chargeAbsTol_ = 1E-12;
  timeStepControlRec_.maxChargeLTEFactor_ = 2.0;
  timeStepControlRec_.chargeTRTOL_ = 40;
  // LTE charge tolerances for the heuristic part
  timeStepControlRec_.chargeHeuristicAbsTol_= 1E-14;
  timeStepControlRec_.chargeHeuristicRelTol_ = 1E-3;

  // Newton tolerances for various analysis
  dcTols_.absTolCurrents_ = 1E-9;
  dcTols_.absTolVoltages_ = 1E-9;
  dcTols_.relTolCurrents_ = 1E-8;
  dcTols_.relTolVoltages_ = 1E-8;

  // Newton tolerances for Transient analysis
  tranTols_.absTolCurrents_ = 1E-7;
  tranTols_.absTolVoltages_ = 1E-6;
  tranTols_.relTolCurrents_ = 1E-4;
  tranTols_.relTolVoltages_ = 1E-4;
}
