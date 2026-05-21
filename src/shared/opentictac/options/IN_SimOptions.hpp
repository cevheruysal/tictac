/*
 * IN_SimOptions.hpp
 *
 *  Created on: Jun 17, 2013
 *      Author: benk
 */

#ifndef IN_SIMOPTIONS_HPP_
#define IN_SIMOPTIONS_HPP_

#include "sim_utils.hpp"

/** the record */
class TimeStepSizeControlRecord {
public:
  /// factor for currents
  MYREAL currentEpsFact_;
  /// factor for voltages
  MYREAL voltageEpsFact_;
  /// the target epsilon, the target error for one time step
  MYREAL targetEpsilon_;
  /// factor by which the time step can be increased by the LTE
  MYREAL maximalTimeStepSizeIncrease_;
  /// the minimal time step size which is numerically OK
  MYREAL minimalDeltaT_;
  /// the maximal Delta T
  MYREAL maximalDeltaT_;
  /// the starting delta T
  MYREAL startDeltaT_;
  /// in case of refuse or no convergence to reduce the time step size
  MYREAL factorToReduceTimeStep_;
  /// this factor specifies which is the limit to refuse a time step
  MYREAL factorToRefuseTimeStep_;

  // --- LTE Control ---
  /// charge relative LTE control
  MYREAL chargeRelTol_;
  /// charge absolute LTE control
  MYREAL chargeAbsTol_;
  /// maximal allowed LTE factor error within a time step
  MYREAL maxChargeLTEFactor_;
  /// TRTOL for chage controll
  MYREAL chargeTRTOL_;

  /// the heuristic absolute tolerance for charges
  MYREAL chargeHeuristicAbsTol_;
  /// the heuristic relatice tolerance for charges
  MYREAL chargeHeuristicRelTol_;

} ;

/** the record containing the */
class NewtonTolerances {
public:
  MYREAL absTolVoltages_;
  MYREAL relTolVoltages_;
  MYREAL absTolCurrents_;
  MYREAL relTolCurrents_;
} ;

/** class containing all the options of a simulation */
class IN_SimOptions
{

public:

  /** Ctor */
  IN_SimOptions();

  /** empty Dtor */
  virtual ~IN_SimOptions() {;}

  /** for the time integration maximal needed time stamp history */
  inline MYINT getMaxRequiredTimeHistory() const { return 5;}

  /** returns the control record struct for the time step size control */
  const TimeStepSizeControlRecord& getTimeStepSizeControlRecord() const { return timeStepControlRec_; }
  TimeStepSizeControlRecord& getTimeStepSizeControlRecord() { return timeStepControlRec_; }

  /** newton tolerances for DC */
  const NewtonTolerances& getDCNewtonTols() const { return dcTols_; }
  NewtonTolerances& getDCNewtonTols() { return dcTols_; }

  /** newton tolerances for tran */
  const NewtonTolerances& getTranNewtonTols() const { return tranTols_; }
  NewtonTolerances& getTranNewtonTols() { return tranTols_; }

private:

  /** record for time step size control */
  TimeStepSizeControlRecord timeStepControlRec_;

  /** newton tolerances for the DC analysis */
  NewtonTolerances dcTols_;

  /** Newton tolerances for the transient analysis */
  NewtonTolerances tranTols_;

};

#endif /* IN_SIMOPTIONS_HPP_ */
