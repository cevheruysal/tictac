/*
 * AN_DCAnalysis.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#ifndef AN_DCANALYSIS_HPP_
#define AN_DCANALYSIS_HPP_

#include "AN_Analysis.hpp"
#include "LS_SolverBase.hpp"

/** the DC analysis class*/
class AN_DCAnalysis : public AN_Analysis
{
public:

  /** Ctor for the analysis
   *@param netlist the global netlist
   *@param simulationOptions options for the simulation */
  AN_DCAnalysis(Netlist *netlist, IN_SimOptions* simulationOptions,
      TICTAC_SPTR<OUT_WaveFormPlotter> plotter);

  /** cleanup in Dtor */
  virtual ~AN_DCAnalysis();

  /** virtual method to execute the analysis*/
  virtual void executeAnalysis(std::vector<MYREAL>& stateVector);

  /** return the solver for this analysis */
  const LS_SolverBase<MYREAL>* getDCLinearSolver() const { return dcLinearSolver_; }
  LS_SolverBase<MYREAL>* getDCLinearSolver() { return dcLinearSolver_; }

  /** imposes the .NODSET conditions for voltages */
  virtual void applyInitialCondition(std::vector<MYREAL> &unknownVector);

  /** see super class */
  virtual LS_SolverBase<MYREAL>* getRealSolver([[maybe_unused]] MYINT indx) { return dcLinearSolver_;}
  /** see super class */
  virtual const LS_SolverBase<MYREAL>* getRealSolver([[maybe_unused]] MYINT indx) const { return dcLinearSolver_;}

  /** see super class*/
  virtual std::string getAnalysisName() const { return std::string("DC"); }

private:

  /** the linear solver for the DC */
  LS_SolverBase<MYREAL>* dcLinearSolver_;
};

#endif /* AN_DCANALYSIS_HPP_ */
