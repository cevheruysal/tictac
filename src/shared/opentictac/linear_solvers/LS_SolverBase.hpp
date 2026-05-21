/*
 * LS_SolverBase.hpp
 *
 *  Created on: Oct 1, 2013
 *      Author: benk
 */

#ifndef LS_SOLVERBASE_HPP_
#define LS_SOLVERBASE_HPP_

#include "sim_utils.hpp"
#include "LA_MatrixBasis.hpp"
#include "Netlist.hpp"

/** abstract class of the linear solver */
template <typename DATAT>
class LS_SolverBase : public UT_SIMVerboseClass{
public:

  /** Ctor where the input is the*/
  LS_SolverBase(LA_MatrixBasis<DATAT> *matrix) :
    UT_SIMVerboseClass("Solver"), matrix_(matrix)
  {;}

  /** empty Dtor */
  virtual ~LS_SolverBase() {;}

  /** virtual method to solve the unknown for the i
   * @param x the initial solution and as output the solution of the system
   * @param rhs the right hand side */
  virtual void solve(std::vector<DATAT>& x, const std::vector<DATAT>& rhs) = 0;

  /** for each solver should be possible to replace the matrix during an analysis
   * @param matrix new matrix that replaces the old one.
   * The number of rows and columns might be different */
  virtual void setMatrix(LA_MatrixBasis<DATAT> *matrix) = 0;

  /** returns the matrix of the solver, which might be different from the
   * input, e.g. for LU one needs Markovitz */
  const LA_MatrixBasis<DATAT>* getSolverMatrix() const { return matrix_; }
  LA_MatrixBasis<DATAT>* getSolverMatrix() { return matrix_; }

protected:

  /** the solver's matrix */
  LA_MatrixBasis<DATAT> *matrix_;

};


#endif /* LS_SOLVERBASE_HPP_ */
