/*
 * NS_NonlinearSolver.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: benk
 */

#include "NS_NonlinearSolver.hpp"
#include "NS_Function.hpp"
#include <cmath>

NS_NonlinearSolver::NS_NonlinearSolver() : UT_SIMVerboseClass("NonlinearSolver")
{
  // At the moment empty Ctor, nothing to do here
  maxIter_ = 0;
  actIteration_ = 0;
  dampingFactor_ = 1.0;
  absTolFact_ = 1.0;
  relTolFact_ = 1.0;

  dimRes_ = 0;
  absTolerance_ = 1E-8;
  relTolerance_ = 1E-5;
  hasNewtonConverged_ = false;
  useGlobalTols_ = false;
  this->setVerb(6);
}


void NS_NonlinearSolver::solveNonLinearProblem(
    NS_Function* F,
    std::vector<MYREAL> &u_solve,
    NS_NonlinearSolverStatus &status)
{
  SIM_PRINT_L2(verb(), "NonlinearSolver::solveNonLinearProblem start");

  // get the solver
  LS_SolverBase<MYREAL>* solver = F->getLinearSolver(0);
  std::vector<MYREAL> res(F->getOutputDimension() ,0.0);
  std::vector<MYREAL> absContribs(F->getOutputDimension() ,1.0);
  std::vector<MYREAL> u_delta;
  MYINT i = 0;
  dimRes_ = F->getOutputDimension();
  actIteration_ = 0;
  hasNewtonConverged_ = false;

  // get the Jacobi matrix
  SIM_PRINT_L5(verb()," Eval F " << actIteration_);
  [[maybe_unused]] LA_MatrixBasis<MYREAL>* jacobi = F->evalWithDeriv(actIteration_, u_solve, res, absContribs);

  // debug output
  if (verb() > 4) {
      std::string tmp = "";
      for (i = 0; i < F->getInputDimension(); i++)
        tmp += sim_doubleToString(res[i]) + ",";
      SIM_PRINT_L5(verb(),"Initial residual = {" << tmp << "}");
      tmp = "";
      for (i = 0; i < F->getInputDimension(); i++)
        tmp += sim_doubleToString(absContribs[i]) + ",";
      SIM_PRINT_L5(verb(),"absContribs = {" << tmp << "}");
  }

  // get the flag if we are already converged (for student version do it differently)
  hasNewtonConverged_ = false;

  // iterate as long we converge and we reach the required minimum number of iterations
  while ( (!hasNewtonConverged_) || ((F->minimumNrIteration() > actIteration_) && (actIteration_ < maxIter_)) )
    {
      // early break if no iteration is intended (only function evaluation)
      if (actIteration_ >= maxIter_) {
          SIM_PRINT_L5(verb(),"BREAK Newton due to iteration limit");
          break;
      }

      // we set the correction to zero
      if (actIteration_ == 0)
        u_delta.resize(F->getInputDimension(),0.0);
      else
        for (i = 0; i < F->getInputDimension(); u_delta[i] = 0.0, i++);

      // solve the system
      SIM_PRINT_L5(verb()," Linear solving " << actIteration_ << " , maxIter=" << maxIter_);
      solver->solve(u_delta, res);

      if (verb() > 4) {
          std::string tmp = "";
          for (i = 0; i < F->getInputDimension(); i++)
            tmp += sim_doubleToString(u_delta[i]) + ",";
          SIM_PRINT_L5(verb()," Solution u = {" << tmp << "}");
      }

      // here increase the iteration number
      actIteration_++;

      // update the actual solution
      // IMPORTANT: here is the -1 to update the
      // u_i = u_i + dampingFact * \Delta u_{i} .. but there is the -1 due to Newton !!!
      for (i = 0; i < F->getOutputDimension(); i++)
        u_solve[i] = u_solve[i] - dampingFactor_ * u_delta[i];

      if (verb() > 4) {
           std::string tmp = "";
           for (i = 0; i < F->getInputDimension(); i++)
             tmp += sim_doubleToString(u_solve[i]) + ",";
           SIM_PRINT_L5(verb()," ACTUAL Solution u = {" << tmp << "}");
       }

      // break iteration when maximum iteration is reached
      if (actIteration_ > maxIter_)
        break;

      SIM_PRINT_L5(verb()," Eval F " << actIteration_);
      jacobi = F->evalWithDeriv(actIteration_, u_solve, res, absContribs);
      // get the flag if we are already converged
      hasNewtonConverged_ = this->checkConvergance(F, res, absContribs);
    }

  // set the fields in the status of the nonlinear solver
  status.nrIteration = actIteration_;
  status.converged = hasNewtonConverged_;
  SIM_PRINT_L2(verb(), "NonlinearSolver::solveNonLinearProblem end with="<< hasNewtonConverged_
      << " iterNr.=" << actIteration_);

  if (verb() > 4) {
      std::string tmp = "";
      for (i = 0; i < F->getInputDimension(); i++)
        tmp += sim_doubleToString(u_solve[i]) + ",";
      SIM_PRINT_L5(verb()," FINAL Solution u = {" << tmp << "}");
  }
}


bool
    NS_NonlinearSolver::checkConvergance(NS_Function*         F,
                                         std::vector<MYREAL>& res,
                                         std::vector<MYREAL>& absContribs)
{
  bool ret = true;


 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
  for (size_t i = 0; i < res.size(); ++i) {
    MYREAL absRes = std::abs(res[i]);
    if (absRes >= F->getAbsTolVect()[i] && absRes >= F->getRelTolVect()[i] * absContribs[i]) {
      ret = false;
      break;
    }
  }
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 

  // return the computed flag
  return ret;
}
