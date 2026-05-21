/*
 * AN_Analysis.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#ifndef AN_ANALYSIS_HPP_
#define AN_ANALYSIS_HPP_

#include "sim_utils.hpp"
#include "IN_SimOptions.hpp"
#include "AN_SimulationControllers.hpp"
#include "LS_SolverBase.hpp"
#include "Netlist.hpp"
#include "OUT_WaveFormPlotter.hpp"

/** abstract class to represent one analysis (e.g., DC or TRAN)
 * A run of the simulation could be composed of several consecutive runs */
class AN_Analysis : public UT_SIMVerboseClass{
public:

  /** Ctor for the analysis
   *@param netlist the global netlist */
  AN_Analysis(Netlist *netlist, const Analysis_type atype,
      IN_SimOptions* simulationOptions,
      TICTAC_SPTR<OUT_WaveFormPlotter> plotter):UT_SIMVerboseClass("Analysis"),
      globalNetlist_(netlist), anal_type_(atype), simulationOptions_(simulationOptions)
  {
    actualState_.resize(0);
    plotter_ = plotter;
    nrInitialConditions_ = 0;
    previousAnalysis_ = NULL;
    maxMatrixStamp_ = 130; // some higher dummy value which will be anyhow updatet
    simulationController_ = TICTAC_SPTR<AN_SimulationControllers>(
        new AN_SimulationControllers(this->globalNetlist_, this, this->simulationOptions_));
  }

  /** Dtor*/
  virtual ~AN_Analysis() {;}

  /** virtual method to execute the analysis
   * @param stateVector [in/out] the input and output state of the network */
  virtual void executeAnalysis(std::vector<MYREAL>& stateVector) = 0;

  /** return the analysis type */
  Analysis_type getAnalysisType() const { return anal_type_; }

  /** return the simulation options of this analysis */
  const IN_SimOptions* getSimulationOption() const { return simulationOptions_;}

  /** returns the state of the simulation */
  std::vector<MYREAL>& getState() { return actualState_; }
  const std::vector<MYREAL>& getState() const { return actualState_; }

  /** returns pointer to the global netlist */
  const Netlist* getGlobalNetlist() const { return globalNetlist_; }
  Netlist* getGlobalNetlist() { return globalNetlist_; }

  /** returns the simulation controller */
  const AN_SimulationControllers* getSimulationController() const { return simulationController_.get();}
  AN_SimulationControllers* getSimulationController() { return simulationController_.get();}

  /** returns the solver of the analysis
   * One analysis might have have several solvers (one for each partition)
   * @param indx index of the solver if there are many (for partitions), "0" if there is only one
   *             "0" here also means that we require the global solver.
   *             Hence the local/parti solvers should start from "1" */
  virtual LS_SolverBase<MYREAL>* getRealSolver(MYINT indx) = 0;
  virtual const LS_SolverBase<MYREAL>* getRealSolver(MYINT indx) const = 0;

  /** returns the plotter of each analysis, default return is NULL */
  virtual OUT_WaveFormPlotter* getAnalysisPlotter() { return plotter_.get(); }
  virtual TICTAC_SPTR<OUT_WaveFormPlotter> getAnalysisPlotterPtr() { return plotter_; }

  /** sets the initial conditions for one analysis, at the moment we only support
   * voltages as initial conditions
   * @param initialConditionNode array of node indices where initial conditions are set
   * @param initialConditionValue array with the voltages as initial conditions
   * @param nrInitialConditions number of initial conditions */
  virtual void setInitialCondition(
       std::vector<MYINT> &initialConditionNode,
       std::vector<MYREAL>    &initialConditionValue,
       MYINT nrInitialConditions) {
    // set the fields
    initialConditionValue_ = initialConditionValue;
    initialConditionNode_ = initialConditionNode;
    nrInitialConditions_ = nrInitialConditions;
  }

  /** applyies the initial conditions if there are any
   * @param unknownVector [IN/OUT] the unknown vector representing the actual status */
  virtual void applyInitialCondition([[maybe_unused]] std::vector<MYREAL> &unknownVector) { ; }

  /** sets the previous analysis of this (current) analysis*/
  void setPreviousAnalysis(AN_Analysis*   previousAnalysis) { previousAnalysis_ = previousAnalysis; }

  /** returns the analysis for the previous analysis */
  const AN_Analysis* getPreviousAnalysis() const { return previousAnalysis_; }
  AN_Analysis* getPreviousAnalysis() { return previousAnalysis_; }

  /** returns the name of the name of the analysis */
  virtual std::string getAnalysisName() const = 0;

  /** returns the maximal number of matrix stamp elements that can occur during one analysis */
  MYINT getActualMaxMatrixStamp() const { return maxMatrixStamp_; }

  /** copy from the source analysis the state onto the destination analysis */
  static void copyOneStateToOther(
      const AN_Analysis *source, AN_Analysis* dest,
      Netlist *netlist);

protected:

  // TODO, once we have a partition we might have several netlists for one analysis
  /** the global netlist */
  Netlist *globalNetlist_;

  /** type of the analysis */
  const Analysis_type anal_type_;

  /** initial state of the unknowns */
  std::vector<MYREAL> actualState_;

  /** simulation options */
  const IN_SimOptions* simulationOptions_;

  /** object containing all the necessary simulation controllers */
  TICTAC_SPTR<AN_SimulationControllers> simulationController_;

  /** output plotter for each analysis (for DC this is null)*/
  TICTAC_SPTR<OUT_WaveFormPlotter> plotter_;

  /** previous analysis, NULL if there is no previous */
  AN_Analysis*   previousAnalysis_;

  /** the maximal matrix stamp size that can occur for a device during analysis */
  MYINT maxMatrixStamp_;

  /** function which sets internally the maximal size of the device stamp
   * @param maxStampLength new maximal length */
  void setMaxMatrixStamp(MYINT maxStampLength) {maxMatrixStamp_ = maxStampLength;}

  // -------- initial condition ---------
  /** value of the initial condition */
  std::vector<MYREAL>     initialConditionValue_;
  /** node of the initial conditions */
  std::vector<MYINT>    initialConditionNode_;
  /** number of initial conditions */
  MYINT nrInitialConditions_;
};


#endif /* AN_ANALYSIS_HPP_ */
