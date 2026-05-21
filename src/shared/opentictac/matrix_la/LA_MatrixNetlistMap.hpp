/*
 * LA_MatrixNetlistMap.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: benk
 */

#ifndef LA_MATRIXNETLISTMAP_HPP_
#define LA_MATRIXNETLISTMAP_HPP_

#include "sim_utils.hpp"
#include "LA_MatrixBasis.hpp"

// forward declaration
class Netlist;

/** class to model the connection between one Netlist, which can be only a part
 * of the toplevel circuit, and a matrix.
 * It is important to note that here we do not use the linear index into the matrix
 * but only the row an column indices including the permutations.
 * For a map between the device and the linear index into the matrix we use a
 * different object. */
class LA_MatrixNetlistMap : public UT_SIMVerboseClass {
public:

  /** Ctor for the map
   * @param netlist the netlist to map.
   * @param rowNr the row and column of the matrix
   *              (only square matrixes are considered here)
   * @param vartypes (size of unknowns) types of variables of the matrix
   * @param nodeToUnknown (size of number of nodes)
   * map the nodes (intern or extern) to unknowns, GND is not counted
   * since it is not a variable in the matrix
   * @param currentsToUnknown maps each current from the netlist to an unknown
   * index
   * @param colMap the permutations of the columns in the matrix (column to unknown ID)
   * @param rhsMap the permutations of the rows in the matrix and in the RHS (row to unknown ID)
   * */
  LA_MatrixNetlistMap(
      const Netlist *netlist,
      MYINT rowNr,
      std::vector<MNAVarType>& vartypes,
      std::vector<MYINT>& nodeToUnknown,
      std::vector<MYINT>& currentsToUnknown,
      std::vector<MYINT>& colMap,
      std::vector<MYINT>& rowsMap );

  /** Ctor for the map with complex matrix */
  //LA_MatrixNetlistMap(const Netlist *netlist, const LA_MatrixBasis<COMPLEX> *matrix);

  /** empty Dtor */
  virtual ~LA_MatrixNetlistMap();

  /** returns the variable type of the unknown vector */
  inline MNAVarType getVarType(MYINT index) const { return vartypes_[index]; }

  /** the voltage unknown index for one pin ID (the voltage on this pin)*/
  inline MYINT getUknIDForPinID(MYINT pinID) const { return pinID_To_UnknID_[pinID]; }
  /** returns the pin index of a unknown index(that should be a voltage unknown),
   * returns -1 if not applicable */
  inline MYINT getPinIDForUknID(MYINT unknID) const { return unknID_To_PinID_[unknID]; }

  /** the voltage unknown index for one pin ID (the voltage on this pin)*/
  inline MYINT getUknIDForCurrentID(MYINT pinID) const { return currentID_To_UnknID_[pinID]; }
  /** returns the current index for one unknown index, it returns -1 if this is not applicable */
  inline MYINT getCurrentIDForUknIndex(MYINT unknID) const { return unknID_To_currentID_[unknID]; }

  /** returns the number of rows and columns for which this map was constructed for */
  inline MYINT getNrColAndRow() const { return rowAndColNr_;}

  /** return the netlist of the map, for which this was created */
  inline const Netlist* getMapsNetlist() const {return  netlist_;}

  /** get the unknown index for one column index
   * @param colIndex column index
   * @returns unkown index */
  inline MYINT getUnkIDForColIndex(MYINT colIndex) const {
    //std::cout << "col_To_unknID_[" << colIndex << "]=" <<  col_To_unknID_[colIndex] << "\n";
    return col_To_unknID_[colIndex];
  }

  /** get the column index for one unknown index
   * @param unknIndex unknown index
   * @returns column index */
  inline MYINT getColIndexForUnkID(MYINT unknIndex) const {
    //std::cout << "unknID_To_col_[" << unknIndex << "]=" <<  unknID_To_col_[unknIndex] << "\n";
    return unknID_To_col_[unknIndex];
  }

  /** get the unknown index for one row index
   * @param rowIndex row index
   * @returns unkown index */
  inline MYINT getUnkIDForRowIndex(MYINT rowIndex) const {
    //std::cout << "row_To_unknID_[" << rowIndex << "]=" <<  row_To_unknID_[rowIndex] << "\n";
    return row_To_unknID_[rowIndex];
  }

  /** get the row index for one unknown index
   * @param unknIndex unknown index
   * @returns row index */
  inline MYINT getRowIndexForUnkID(MYINT unknIndex) const {
    //std::cout << "unknID_To_row_[" << unknIndex << "]=" <<  unknID_To_row_[unknIndex] << "\n";
    return unknID_To_row_[unknIndex];
  }

private:

  /** ptr to the given netlist */
  const Netlist *netlist_;

  /** the number of columns and rows in the matrix */
  MYINT rowAndColNr_;

  /** total number of variables */
  MYINT nrTotalVars_;

  /** total number of internal and external pins and no GND */
  MYINT nrNodesNoGnd_;

  /** types of the variables */
  std::vector<MNAVarType> vartypes_;

  /** maps one pin index (internal or external) to one unknown index. -1 means this is ground node
   * (size of number of pins )*/
  std::vector<MYINT> pinID_To_UnknID_;
  /** maps the unknowns to node ID's () (the reverse of pinIDToVarIndex_) -1 if not present
   * (size of unknowns)*/
  std::vector<MYINT> unknID_To_PinID_;

  /** maps the current index to the unknown index (size of nr current)*/
  std::vector<MYINT> currentID_To_UnknID_;
  /** maps the unknown index to current index (size of nr. unknowns)*/
  std::vector<MYINT> unknID_To_currentID_;

  /** maps the unknown index to the rows (in the matrix and in the RHS)*/
  std::vector<MYINT> unknID_To_row_;
  /** maps the matrix and RHS rows to global variable (convention) IDs*/
  std::vector<MYINT> row_To_unknID_;

  /** maps the unknown index to the columns (in the matrix)*/
  std::vector<MYINT> unknID_To_col_;
  /** maps the matrix and columns to global variable (convention) IDs*/
  std::vector<MYINT> col_To_unknID_;

};

#endif /* LA_MATRIXNETLISTMAP_HPP_ */
