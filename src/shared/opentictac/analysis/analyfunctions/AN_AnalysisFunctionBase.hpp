/*
 * AN_AnalysisFunctionBase.hpp
 *
 *  Created on: Oct 24, 2013
 *      Author: benk
 */

#ifndef AN_ANALYSISFUNCTIONBASE_HPP_
#define AN_ANALYSISFUNCTIONBASE_HPP_

#include "sim_utils.hpp"
#include "AN_Analysis.hpp"
#include "LA_MatrixNetlistMap.hpp"

// forward declaration
class LA_AssemblyMatrixMap;
class Netlist;
class NS_Function;

/** base class for all analysis function which contains general functionalities,
 * that might be needed for various analysis */
class AN_AnalysisFunctionBase : public UT_SIMVerboseClass
{
public:

  /** Ctor with the analysis pointer*/
  AN_AnalysisFunctionBase(AN_Analysis *analysis);

  /** no work in the Dtor*/
  virtual ~AN_AnalysisFunctionBase();

  /** method to be called before the analysis in order to map each variable of the
   * devices to matrix and vector rows and columns */
  void prepareAnalysis();

  /** method which should be implemented and should be called before the starting of each
   * analysis ... this routine should make the final preparations for the run.
   * This should call also "prepareAnalysis"*/
  virtual void prepareAnalysisRun() = 0;

  /** function as a generic tool for all analysis to assemble the matrix and
   * right hand sides. This function is usually called within the eval function
   * of the Analysis function
   * @param matrix matrix to be assembled
   * @param netlist netlist belonging to this matrix
   * @param u unkown vector
   * @param res right hand side (residuum vector)
   * @param absContribs maximal contributions to the right hand size
   * @param flags status returned by the devices*/
  void assemblyMatrixAndRHS(
      LA_MatrixBasis<MYREAL>* matrix,
      Netlist* netlist,
      std::vector<MYREAL> &u,
      std::vector<MYREAL> &res,
      std::vector<MYREAL> &absContribs,
      MYINT &flags);

protected:

  /** pointer to the DC analysis */
  AN_Analysis *analysis_;

  /** the map which maps the netlist directly to the matrix entries.
   * In some sense this duplicates the functionality of LA_MatrixNetlistMap.
   * In addition to this it stores the unknown and RHS vector maps in a much
   * efficient way using more storage, and also stores the linear matrix indicies
   * for fast matrix assembly */
  TICTAC_SPTR<LA_AssemblyMatrixMap> assemblyMap_;

};

#endif /* AN_ANALYSISFUNCTIONBASE_HPP_ */
