/*
 * AN_TranFunction.hpp
 *
 *  Created on: Oct 30, 2013
 *      Author: benk
 */

#ifndef AN_TRANFUNCTION_HPP_
#define AN_TRANFUNCTION_HPP_

#include "sim_utils.hpp"
#include "NS_Function.hpp"
#include "AN_TranAnalysis.hpp"
#include "AN_AnalysisFunctionBase.hpp"

/** the function class of the TRAN analysis nonlinear function */
class AN_TranFunction : public AN_AnalysisFunctionBase, public NS_Function
{
public:

  /** Ctor for the Tran analysis cost function*/
  AN_TranFunction(AN_TranAnalysis *tranAnalysis);

  /** empty Dtor*/
  virtual ~AN_TranFunction();

  /** see base class */
  virtual void prepareAnalysisRun();

  /** see NS_Function base class */
  virtual void eval(
      MYINT evalIndex,
      std::vector<MYREAL> &u,
      std::vector<MYREAL> &res,
      std::vector<MYREAL> &absContribs);

  /** see NS_Function base class */
  virtual LA_MatrixBasis<MYREAL>* evalWithDeriv(
      MYINT evalIndex,
      std::vector<MYREAL> &u,
      std::vector<MYREAL> &res,
      std::vector<MYREAL> &absContribs);

  /** returns the linear solver of the function.
   * This function is analysis specific and usually the solver is
   * analysis specific
   * @param index it is "0" when it requires the global solver and > 0 when it uses partions ...*/
  virtual LS_SolverBase<MYREAL>* getLinearSolver(MYINT index) { return tranAnalysis_->getRealSolver(index); }

  /** we can require always at least one iteration */
  virtual MYINT minimumNrIteration() const;

private:

  /** pointer to the transisent analysis class */
  AN_TranAnalysis *tranAnalysis_;
};

#endif /* AN_TRANFUNCTION_HPP_ */
