/*
 * LA_MatrixRepresentation.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#ifndef LA_MATRIXREPRESENTATION_HPP_
#define LA_MATRIXREPRESENTATION_HPP_

#include "sim_utils.hpp"

// forward declaration of the class
class LA_MatrixNetlistMap;

/** abstract class for sparse matrix representation,
 * not just for squared matrixes but for general structure */
template <typename DATAT>
class LA_MatrixBasis : public UT_SIMVerboseClass {
public:

  /** Ctor for square matrix representation
   * @param nrRows number of rows
   * @param nrCols number of columns
   * @param nnz number of nonzero entries in the matrix
   * @param map the map that maps the network equations to the vector and matrix indices */
  LA_MatrixBasis(MYINT nrRows, MYINT nrCols, MYINT nnz,
      const LA_MatrixNetlistMap* map) :
    UT_SIMVerboseClass("MatrixRepresentation"),
    map_(map), nrRows_(nrRows), nrCols_(nrCols), nrNz_(nnz) {;}

  /** Dtor */
  virtual ~LA_MatrixBasis() { ; }

  /** returns the number of rows */
  MYINT getNrRows() const { return nrRows_; }

  /** returns the number of columns */
  MYINT getNrColumns() const { return nrCols_; }

  /** returns the number of nonzero elements in the matrix.
   * It is -1 if is not set */
  MYINT getNz() const { return nrNz_; }

  /** returns true if the matrix is a square matrix, false otherwise */
  bool isMatrixSquare() const { return ((nrRows_>0) && (nrRows_ == nrCols_)); }

  /** resets all matrix entries to zero */
  virtual void clearMatrix() = 0;

  /** returns the linear index of the matrix such that this index can be directly
   * used to acces the matrix elements. Returns -1 of this entry is not found */
  virtual MYINT getLinearIndex(MYINT row, MYINT col) const = 0;

  /** returns a dummy index where all the "ground" stuff can be stamped */
  virtual MYINT getDummyLinearIndex() const = 0;

  /** returns the pointer to the linearized index
   * IMPORTANT the linear indexing MUST be continuous */
  virtual DATAT* getPointer(MYINT linearIndex) = 0;

  /** returns the value of the matrix at the linearized index */
  virtual DATAT getValue(MYINT linearIndex) const = 0;

  /** sets the value of the matrix at the linearized index */
  virtual void setValue(MYINT linearIndex, DATAT& value) = 0;

  /** adds one contribution to the matrix at the specified column and
   * row index */
  virtual void addContribution(MYINT row, MYINT col, DATAT& value) = 0;

  /** add to one matrix item specified by the linear index */
  virtual void addContribution(MYINT linearIndex, DATAT& value) = 0;

  /** prints out the matrix into a file
   * @param fileName name of the file where the matrix  should be printed */
  virtual void printMatrix(std::string& fileName) const = 0;

  /** resets all the matrix entries to zero */
  virtual void resetvalues() = 0;

  /** returns the map to the vector and matrix entries to the network variables */
  const LA_MatrixNetlistMap* getMatrixMap() const { return map_; }

protected:

  /** map to map the network variables to vector and matrix indices */
  const LA_MatrixNetlistMap* map_;

  /** number of rows */
  const MYINT nrRows_;

  /** number of columns */
  const MYINT nrCols_;

  /** number of nonzero elements */
  const MYINT nrNz_;
};


#endif /* LA_MATRIXREPRESENTATION_HPP_ */
