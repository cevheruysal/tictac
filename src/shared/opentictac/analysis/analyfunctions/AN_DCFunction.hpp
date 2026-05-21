/*
 * AN_DCFunction.hpp
 *
 *  Created on: Oct 23, 2013
 *      Author: benk
 */

#ifndef AN_DCFUNCTION_HPP_
#define AN_DCFUNCTION_HPP_

#include "sim_utils.hpp"
#include "NS_Function.hpp"
#include "AN_DCAnalysis.hpp"
#include "AN_AnalysisFunctionBase.hpp"

/** the function class of the DC analysis nonlinear function */
class AN_DCFunction : public AN_AnalysisFunctionBase, public NS_Function
{
public:

  /** Ctor */
  AN_DCFunction(AN_DCAnalysis *dcAnalysis);

  /** Dtor */
  virtual ~AN_DCFunction();

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
  virtual LS_SolverBase<MYREAL>* getLinearSolver(MYINT index) { return dcAnalysis_->getRealSolver(index); }

  /** see base class */
  virtual void prepareAnalysisRun();

private :

  /** pointer to the DC analysis */
  AN_DCAnalysis *dcAnalysis_;

};

#endif /* AN_DCFUNCTION_HPP_ */
