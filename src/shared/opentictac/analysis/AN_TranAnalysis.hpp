/*
 * AN_TranAnalysis.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#ifndef AN_TRANANALYSIS_HPP_
#define AN_TRANANALYSIS_HPP_

#include "AN_Analysis.hpp"
#include "AN_DTController.hpp"

/** class for transient analysis:
 *  all the functionalities regarding the execution of the transient analysis
 *  should be contained in this class
 * */
class AN_TranAnalysis: public AN_Analysis
{
public:

  /** Ctor for the analysis
   *@param netlist the global netlist
   *@param startT  starting time point
   *@param endT end time point of the analysis
   *@param simulationOptions options for the simulation
   *@param plotter if not given then it is NULL */
  AN_TranAnalysis(Netlist *netlist, MYREAL startT, MYREAL endT,
      IN_SimOptions* simulationOptions,
      TICTAC_SPTR<OUT_WaveFormPlotter> plotter);

  /** empty Dtor */
  virtual ~AN_TranAnalysis();

  /** virtual method to execute the analysis*/
  virtual void executeAnalysis(std::vector<MYREAL>& stateVector);

  /** see super class */
  virtual LS_SolverBase<MYREAL>* getRealSolver([[maybe_unused]] MYINT indx) { return tranLinearSolver_;}
  /** see super class */
  virtual const LS_SolverBase<MYREAL>* getRealSolver([[maybe_unused]] MYINT indx) const { return tranLinearSolver_;}
  /** see super class */
  virtual std::string getAnalysisName() const { return std::string("TRAN"); }

  /** imposes the .IC constructs */
  virtual void applyInitialCondition(std::vector<MYREAL> &unknownVector);

  /** returns the start time of the transient analysis */
  MYREAL getStartTime() const { return startTime_; }

  /** returns the end time of this transient simulation */
  MYREAL getEndTime() const { return endTime_; }

private:

  /** internal method which handles the next step size.
   * This method is called after */
  bool nextTimeStepSizeHandling();

  /** this internal functions treates all waveform plotting */
  void treatPlotting();

  /** starting time of the analysis, this is*/
  MYREAL startTime_;

  /** end time of the simulation */
  MYREAL endTime_;

  /** actual simulation time */
  MYREAL actualTime_;

  /** actual time step index */
  MYINT  timeStepIndex_;

  /** count the number of time steps after the break point */
  MYINT  timeStepAfterBreakPoint_;

  /** actual time step */
  MYREAL actualDeltaTime_;

  /** after a BP the device which caused it should also tell the starting DT, which is stored here */
  MYREAL actualStartDeltaTime_;

  /** previous time steps */
  MYREAL oldDeltaTime_;

  /** the linear solver for this Tran analysis  */
  LS_SolverBase<MYREAL>* tranLinearSolver_;

  /** the index of devices which need postprocessing after each TRAN step*/
  std::vector<MYINT>  postTranStepDevices_;
  /** number of devices which require postprocessing */
  MYINT               nrPostTranDevices_;

  /** type of the actual time step */
  TimeStepType          stepType_;
  /** type of the comming time step */
  TimeStepType          nextstepType_;

  /** struct containing all the time step size control parameters */
  TimeStepSizeControlRecord timeStepCtr_;
};

#endif /* AN_TRANANALYSIS_HPP_ */
