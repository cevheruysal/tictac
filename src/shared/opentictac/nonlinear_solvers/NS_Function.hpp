/*
 * NS_Function.hpp
 *
 *  Created on: Oct 21, 2013
 *      Author: benk
 */

#ifndef NS_FUNCTION_HPP_
#define NS_FUNCTION_HPP_

#include "sim_utils.hpp"
#include "LA_MatrixBasis.hpp"
#include "LS_SolverBase.hpp"

/** abstract class to model the nonlinear functions that need to be
 * zero, F(u)=res.*/
class NS_Function
{
public:

  /** empty Ctor */
  NS_Function();

  /** empty Dtor */
  virtual  ~NS_Function() {;}

  /** virtual method to evaluate the function without the Jacobi matrix.
   * Hence, this function is probably not so often used since the Jacobi
   * is critical OR eventual use the old direction
   * @param evalIndex the nonlinear iteration index
   * @param u the input vector, u from F(u)=res
   * @param res residuum (without the -1 sign!!!)
   * @param absContribs the magnitudes of the F component (needed for relative error check)*/
  virtual void eval(
      [[maybe_unused]] MYINT evalIndex,
      [[maybe_unused]] std::vector<MYREAL> &u,
      [[maybe_unused]] std::vector<MYREAL> &res,
      [[maybe_unused]] std::vector<MYREAL> &absContribs)
  {
    SIM_ERROR_STOP("NS_Function::eval not implemented");
  }

  /** virtual method to evaluate the function with the Jacobi matrix,
   * which is returned by this function
   *
   * @param evalIndex the nonlinear iteration index
   * @param u the input vector, u from F(u)=res
   * @param res the output of "F(u)=res" (without the -1 sign!!!)
   * @param absContribs the magnitudes of the F component (needed for relative error check)
   * @return the matrix of sensitivities \Grad F|_{u} */
  virtual LA_MatrixBasis<MYREAL>* evalWithDeriv(
      [[maybe_unused]] MYINT evalIndex,
      [[maybe_unused]] std::vector<MYREAL> &u,
      [[maybe_unused]] std::vector<MYREAL> &res,
      [[maybe_unused]] std::vector<MYREAL> &absContribs)
  {
    SIM_ERROR_STOP("NS_Function::evalWithSens not implemented");
    return NULL;
  }

  /** returns the linear solver of the function.
   * This function is analysis specific and usually the solver is
   * analysis specific
   * @param index "0" means the default (global) solver. In case of partitioned approch
   * this might become different than zero */
  virtual LS_SolverBase<MYREAL>* getLinearSolver(MYINT index) = 0;

  /** each function can require for the solver to have at least a
   * required number of iterations */
  virtual MYINT minimumNrIteration() const { return 0; }

  /** returns the dimension of the input "u"*/
  MYINT getInputDimension() const { return inDim_; }

  /** returns the output dimension of "F"*/
  MYINT getOutputDimension() const { return outDim_; }

  /** returns the absolute tolerances */
  const std::vector<MYREAL>& getAbsTolVect() const { return absTol_; }

  /** returns the relative tolerances */
  const std::vector<MYREAL>& getRelTolVect() const { return relTol_; }

protected:

  /** sets the dimension of the input "u"*/
  void setInputDimension(MYINT inD) { inDim_ = inD; }

  /** sets the output dimension of "F"*/
  void setOutputDimension(MYINT outD) { outDim_ = outD; }

  /** the dimension of the input argument "u" */
  MYINT inDim_;

  /** dimension of the output vector of F */
  MYINT outDim_;

  /** vector of size outDim_ and contains the absolute tolerances for the Newton convergence */
  std::vector<MYREAL> absTol_;
  /** vector of size outDim_ and contains the absolute tolerances for the Newton convergence */
  std::vector<MYREAL> relTol_;
};

#endif /* NS_FUNCTION_HPP_ */
