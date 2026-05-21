/*
 * LS_SolverFactory.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: benk
 */

#ifndef LS_SOLVERFACTORY_HPP_
#define LS_SOLVERFACTORY_HPP_

#include "LS_SolverBase.hpp"
#include "Netlist.hpp"
#include "IN_SimOptions.hpp"
#include "AN_Analysis.hpp"
#include "sim_utils.hpp"

/** factory class to produce a solver object */
class LS_SolverFactory : public UT_SIMVerboseClass
{
public:

  /** empty Ctor */
  LS_SolverFactory();

  /** empty Dtor */
  virtual ~LS_SolverFactory();

  /** method to produce a solver object for a given analysis,
   * netlist and simulation options */
  LS_SolverBase<MYREAL>* factorSolverReal(const AN_Analysis* analysis,
           const Netlist* netlist, const IN_SimOptions* simoptions);

  /** returns the maximal size of matrix stamp found during the last factor call*/
  MYINT getActualMaximalMatrixStampSize() const { return nrMaxMatrixStampSize_; }

protected:

  /** private method to make the Markowitz fill-in method.
   * Inputs are the two vetors of the CRS matrix,
   * and the outputs are the two vectors of the CRS matrix with the fill-ins
   * plus the row and column swaps which were made
   * @param inRowPtr input row pointer of the CRS structure
   * @param inColPtr input column vector of the CRS
   * @param  nrRows number of rows and columns
   * @param nnz number of nonzeros
   * @param unknownMap result of the column swaps
   * @param rhsMap result of the row swaping
   * @param outRowPtr output row pointer (CRS) after Markowitz
   * @param outColPtr output column vector of the CRS structur after Markowitz
   * @param nnzOut number of nonzeros after Markowitz (including the fill-ins) */
  void makeMarkowitz(const std::vector<MYINT>& inRowPtr,
                     const std::vector<MYINT>& inColPtr,
                     MYINT nrRows,
                     MYINT nnz,
                     // ------ inout -----
                     std::vector<MYINT>& unknownMap,
                     std::vector<MYINT>& rhsMap,
                     // ------ output ----------
                     std::vector<MYINT>& outRowPtr,
                     std::vector<MYINT>& outColPtr,
                     MYINT &nnzOut                    );

private:

  /** for the latest factory call it shows which is the maximal device stamp size */
  MYINT nrMaxMatrixStampSize_;

};

#endif /* LS_SOLVERFACTORY_HPP_ */
