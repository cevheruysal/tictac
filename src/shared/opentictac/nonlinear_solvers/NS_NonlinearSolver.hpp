/*
 * NS_NonlinearSolver.hpp
 *
 *  Created on: Oct 21, 2013
 *      Author: benk
 */

#ifndef NS_NONLINEARSOLVER_HPP_
#define NS_NONLINEARSOLVER_HPP_

#include "sim_utils.hpp"
#include "LS_SolverBase.hpp"

// forward declaration of the nonlinear function
class NS_Function;

/** struct to store the status of the nonlinear solver */
struct NS_NonlinearSolverStatus {
  MYINT nrIteration;
  bool converged;
};

/** class to represent the nonlinear problems which need to be solved
 * in various analysis
 * */
class NS_NonlinearSolver : public UT_SIMVerboseClass
{
public:

  /** empty Ctor (since parameters should be changeable dynamically)*/
  NS_NonlinearSolver();

  /** empty Dtor */
  virtual ~NS_NonlinearSolver() {;}

   /** this routine sets only global parameters */
   void setParameters(  MYINT maxIter,
       MYREAL absTol,
       MYREAL realTol,
       MYREAL dampingFact = 1.0,
       MYREAL absFact = 1.0,
       MYREAL relFact = 1.0,
       bool useGlobalTols = false) {
     maxIter_ = maxIter;
     dampingFactor_ = dampingFact;
     absTolerance_ = absTol;
     relTolerance_ = realTol;
     absTolFact_ = absFact;
     relTolFact_ = relFact;
     useGlobalTols_ = useGlobalTols;
   }

   /** method to solve the nonlinear problem F(u) = 0
    * @param F function F that should become zero
    * @param u_solve [in/out] represents the initial guess and the final solution
    * @param linear_solver linear solver for the linear step
    * @param status the returned status of the solver */
   void solveNonLinearProblem(
       NS_Function* F,
       std::vector<MYREAL> &u_solve,
       NS_NonlinearSolverStatus &status);

private :

   /** internal method to check the convergence of the nonlinear probelm */
   bool checkConvergance(NS_Function* F,
                         std::vector<MYREAL>& res,
                         std::vector<MYREAL>& absContribs );

   /** limit for the nonlinear iterations */
   MYINT maxIter_;

   /** actual iteration number */
   MYINT actIteration_;

   /** dimension of the residuum */
   MYINT dimRes_;

   /** actual damping factor */
   MYREAL dampingFactor_;

   /** absolute tolerance */
   MYREAL absTolerance_;
   /** used to relax vector abs tolerances */
   MYREAL absTolFact_;

   /** relative tolerance */
   MYREAL relTolerance_;
   /** used to realx vector relative tolerances */
   MYREAL relTolFact_;

   /** flag to indicate if the last Newton problem converged */
   bool hasNewtonConverged_;

   /** flag to indicate weather to use global tolerance values and not the vector
    * tolerances from the "NS_Function"*/
   bool useGlobalTols_;

};

#endif /* NS_NONLINEARSOLVER_HPP_ */
