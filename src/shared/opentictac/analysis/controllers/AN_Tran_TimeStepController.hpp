/*
 * AN_Tran_TimeStepController.hpp
 *
 *  Created on: Oct 30, 2013
 *      Author: benk
 */

#ifndef AN_TRAN_TIMESTEPCONTROLLER_HPP_
#define AN_TRAN_TIMESTEPCONTROLLER_HPP_

#include "sim_utils.hpp"

class IN_SimOptions;
class TimeStepSizeControlRecord;
class Netlist;
class LA_MatrixNetlistMap;
class AN_DTController;

/** class to control the time step size in the transient analysis.
 *
 * In the first version it is meant to use a simple method, linear predictor and then
 * consider the predictor solution.
 *
 * Actual this class should use a simple method to make a prediction, linearly extrapolate
 * two values to get a prediction.
 * Then the error is defined between this extrapolated value and the value given by the
 * implicit iteration.
 *
 * IMPORTANT: the unknown vectors are all in the LA format as they are provided by the solver !!!*/
class AN_Tran_TimeStepController : public UT_SIMVerboseClass
{
public:

  /** Ctor */
  AN_Tran_TimeStepController(
      MYINT nrUnknowns, const IN_SimOptions* options,
      const Netlist* netlist, const LA_MatrixNetlistMap *map,
      const AN_DTController *dtContrl);

  /** Dtor */
  virtual ~AN_Tran_TimeStepController();

  /** after the next time step has been estimated one needs to update the history
   * with this method. This should only be called once a time step is accepted
   * @param  unkVector unknown vector at time actualTime
   * @param  actualTime time in sec */
  void setActualVector( std::vector<MYREAL> &unkVector, MYREAL actualTime);

  /** in case of REFUSE time steps one needs to reset the actual (the most
   * actual) time step
   * @param unkVector unknown vector at reset time
   * @param resetTime the resetet time */
  void resetActualVector(std::vector<MYREAL> &unkVector, MYREAL resetTime);

  /** predicts (based on the previous time steps and no circuit specific
   * information only linear) the unknowns in the next time step
   * @param unkVector [OUT] this is the output unknown vector with the predicted values
   * @param actualTime [IN] time stamp at the prediction */
  void predictNextTimeStep( std::vector<MYREAL> &unkVector, MYREAL actualTime );

  /** function to propose (based on LTE or similar) the size of the next time.
   * Input is the converged solution vector (computed with the corrector/implicit scheme).
   * It is important that this function uses the internally stored result of the
   * "predictNextTimeStep" function
   * @param unkVector solution of the implicit/corrector scheme
   * @param oldDT      previous time step
   * @param deltaT     output of the next time step size
   * @param maxLTEFact maximal LTE factor
   * @param rejectStep true if the actual time step is too high*/
  void computeLTEandnewTimeStep( std::vector<MYREAL> &unkVector, MYREAL oldDT, MYREAL &deltaT,
      MYREAL &maxLTEFact, bool &rejectStep);

  /** returns the last accepted state */
  const std::vector<MYREAL>& getPrevState() const { return unknownHistory_[swapVector_[1]];}


private:

  /** reference to the DDT controllers */
  const AN_DTController *dtContrl_;

  /** the vector of vectors containing the history of the unknowns (voltages/currents)*/
  std::vector<  std::vector<MYREAL>  >  unknownHistory_;

  /** times in second for each history vector*/
  std::vector<MYREAL> timeStamps_;

  /** help vector to make swap among history */
  std::vector<MYINT> swapVector_;

  /** valid time stamps */
  MYINT nrValidTimeStep_;

  /** actual extrapolated values */
  std::vector<MYREAL> actualExtrapolation_;

  /** the time value at the actual extrapolation */
  MYREAL actualExtrapolationTime_;

  /** number of unknowns */
  MYINT nrUnknowns_;

  /** the sizeof the history */
  const MYINT nrMaxHistory_;

  /** pointer to the netlist */
  // const Netlist* netlist_;

  // --- parameters for the time step estimation ----

  /** the vector that defines the epsilon for each unknown*/
  std::vector<MYREAL> epsilonFactorPerUnknown_;

  /** the epsilon which is the target error per time step */
  MYREAL targetEpsilon_;

  /** apply a damping factor to the time step control */
  MYREAL dampingFactor_;

  /** how much the time step can be increased (usual value is 2). */
  MYREAL maxDTIncrease_;

  /** class containing the settings for the timestep control */
  const TimeStepSizeControlRecord *timeStepControlRec_;

};

#endif /* AN_TRAN_TIMESTEPCONTROLLER_HPP_ */
