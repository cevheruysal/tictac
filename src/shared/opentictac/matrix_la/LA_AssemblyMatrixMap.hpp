/*
 * LA_AssemblyMatrixMap.hpp
 *
 *  Created on: Jan 31, 2014
 *      Author: benk
 */

#ifndef LA_ASSEMBLYMATRIXMAP_HPP_
#define LA_ASSEMBLYMATRIXMAP_HPP_

#include "sim_utils.hpp"

// forward declarations
class Netlist;
class AN_Analysis;

/** for a fast matrix assembly (stamping the devices into the matrix) a quick access
 * to the linear matrix entries is crucial.
 * This class models exactly this fast access of each device into the matrix, using nothing
 * but the linear index of the matrix.
 * This linear index is computed only at the beginning and is used later.
 * We also use this map for mapping the unknowns and the rhs vecors.
 * In some sense this duplicates the functionality of LA_MatrixNetlistMap.
 * In addition to this it stores the unknown and RHS vector maps in a much
 * efficient way using more storage, and also stores the linear matrix indices
 * for fast matrix assembly */
class LA_AssemblyMatrixMap : public UT_SIMVerboseClass
{
public:

  /** Ctor for the
   * @param analysis needed as input for the device methods
   * @param netlist the netlist (must not be the global netlist)*/
  LA_AssemblyMatrixMap(AN_Analysis *analysis, Netlist *netlist);

  /** empty Dtor */
  virtual ~LA_AssemblyMatrixMap() {;}

  /** important method to call which generates all the data required for the
   * analysis */
  void generateMap();

  /** returns the total number of variables
   * @param devIndex the device index*/
  inline MYINT getNrLocalVars(MYINT devIndex) const { return nrLocalVars_[devIndex]; }

  /** returns the constant vector with the map from all variables to the unkown vector */
  inline const std::vector<MYINT>& getMapLocalVarsToGlobalVector() const { return mapLocalVarsToGlobalVector_;}
  /** return the entry points for the devices index is the device index from the Ctor netlist and returns
   * the index in the "getMapLocalVarsToGlobalVector" where the local vars start for the device */
  inline const std::vector<MYINT>& getMapLocalVarsStartIndex() const { return mapLocalVarsStartIndex_; }

  /** returns the constant vector with the map from all variables to the right hand side vector */
  inline const std::vector<MYINT>& getMapLocalVarsToGlobalRHS() const { return mapLocalVarsToGlobalRHS_;}
  /** return the entry points for the devices index is the device index from the Ctor netlist and returns
   * the index in the "getMapLocalVarsToGlobalRHS" where the local vars start for the device */
  inline const std::vector<MYINT>& getMapLocalRHSStartIndex() const { return mapLocalRHSStartIndex_; }

  /** returns the constant vector with the map from all variables to the matrix */
  inline const std::vector<MYINT>& getMapLocalVarsToGlobalMatrix() const { return mapLocalVarsToGlobalMatrix_;}
  /** return the entry points for the devices index is the device index from the Ctor netlist and returns
   * the index in the "getMapLocalVarsToGlobalRHS" where the local vars start for the device */
  inline const std::vector<MYINT>& getMapLocalMatrixStartIndex() const { return mapLocalMatrixStartIndex_; }


private:

  /** internal variable to show if this object was already initialized */
  bool isInitialized_;

  /** the analysis for which this assembly was build for*/
  AN_Analysis *analysis_;

  /** the netlist used for the creation of this netlist */
  Netlist *netlist_;

  /** for each device the number of local variables */
  std::vector<MYINT> nrLocalVars_;

  /**  maps the local variables of each device into the global index of the unkown vector
   * this is important for the device evaluation input */
  std::vector<MYINT> mapLocalVarsToGlobalVector_;
  /** similar to CRS structures this is the entry point of one device into the
   * mapLocalVarsToGlobalVector_ vector */
  std::vector<MYINT> mapLocalVarsStartIndex_;

  /** for the RHS maps each local variable of each device to an entry into the RHS
   * IMPORTANT: this map is not similar to the unknown map (due to the column swaps)*/
  std::vector<MYINT> mapLocalVarsToGlobalRHS_;
  /** entry point of a device mapLocalVarsToGlobalRHS_ */
  std::vector<MYINT> mapLocalRHSStartIndex_;

  /** maps each local matrix entry of each device to a linear matrix entry */
  std::vector<MYINT> mapLocalVarsToGlobalMatrix_;
  /** entry point of each device in the matrix mapLocalVarsToGlobalMatrix_ */
  std::vector<MYINT> mapLocalMatrixStartIndex_;
};

#endif /* LA_ASSEMBLYMATRIXMAP_HPP_ */
