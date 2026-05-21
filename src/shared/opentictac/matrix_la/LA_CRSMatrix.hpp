/*
 * LA_CRSMatrix.hpp
 *
 *  Created on: Oct 1, 2013
 *      Author: benk
 */

#ifndef LA_CRSMATRIX_HPP_
#define LA_CRSMATRIX_HPP_

#include "sim_utils.hpp"

#include "OUT_MessageHandle.hpp"
#include "LA_MatrixBasis.hpp"

#include <cstring>

template <typename DATAT>
/** class representing the CRS matrices (stored in the CRS format). */
class LA_CRSMatrix : public LA_MatrixBasis<DATAT>
{

public:

  /** The Ctor of the CRS matrix class.
   * Inputs are nut just the number of columns and rows but also the row and
   * column pointer vectors */
  LA_CRSMatrix(MYINT nrRows, MYINT nrCols, MYINT nnz,
      std::vector<MYINT>& rowPtr, std::vector<MYINT>& colPtr,
      const LA_MatrixNetlistMap* map) :
      LA_MatrixBasis<DATAT>(nrRows, nrCols, nnz, map)
    {
      MYINT tmp = -1;
      // just copy the two vectors
      rowPtr_ = rowPtr;
      colPtr_ = colPtr;
      // and check only the lengths
      SIM_ERROR_COND_STOP((MYINT)rowPtr_.size() != this->nrRows_+1,
        "LA_CRSMatrix rowPtr_.size()=" << rowPtr_.size() << " nrRows_+1=" << this->nrRows_+1);
      SIM_ERROR_COND_STOP((MYINT)colPtr_.size() != this->nrNz_,
        "LA_CRSMatrix colPtr_.size()=" << colPtr_.size() << " nrNz=" << this->nrNz_);

      // compute the diagonal indices
      diagPtr_.resize(this->nrRows_,-1);
      for (MYINT i = 0; i < this->nrRows_; i++)
        {
          tmp = getLinearIndex(i,SIMMIN(i, this->nrRows_));
          // tmp could also be -1
          diagPtr_[i] = ((tmp >= 0) ? tmp+1: tmp);
          //std::cout << " diagPtr[" << i << "]=" << diagPtr_[i] << "\n";
        }

      // initialize the matrix's values, the +1 is for dummy entries
      values_.resize(nnz+1);
      clearMatrix();
    }

  /** empty Dtor. Memory will be deleted automatically */
  virtual ~LA_CRSMatrix() { ; }

  /** resets all matrix entries to zero */
  virtual void clearMatrix() {
    // set the matrix to zero
    std::memset( (void*)&(values_[0]), 0, this->nrNz_*sizeof(DATAT));
  }

  /** returns the linear index of the matrix such that this index can be directly
   * used to acces the matrix elements.
   * If this element does not exists returns -1. */
  virtual MYINT getLinearIndex(MYINT row, MYINT col) const {
    MYINT startI = rowPtr_[row], endI = rowPtr_[row+1]-1, res;
    // make binary search
    res = sim_binarysearch(startI, endI, col, colPtr_);
    // what if this returns -1, then issue at least a warning,  TODO: later we could switch to ERROR
    if (res < 0)
      SIM_WARNING("CRSMatrix: wrong index access row=" << row << " col=" << col);
    return res;
  }

  /** returns the pointer to the linearized index
   * IMPORTANT the linear indexing MUST be continuous */
  virtual DATAT* getPointer(MYINT linearIndex) { return &(values_[linearIndex]); }

  /** returns the value of the matrix at the linearized index */
  virtual DATAT getValue(MYINT linearIndex) const { return values_[linearIndex]; }

  /** returns the value of the matrix at the linearized index */
  virtual void setValue(MYINT linearIndex, DATAT& value) { values_[linearIndex] = value; }

  /** adds one contribution to the matrix at the specified column and
   * row index */
  virtual void addContribution(MYINT row, MYINT col, DATAT& value)
  {
    MYINT linearIndex = getLinearIndex(row, col);
    values_[linearIndex] += value;
  }

  /** add to one matrix item specified by the linear index */
  virtual void addContribution(MYINT linearIndex, DATAT& value) { values_[linearIndex] += value; }

  /** returns the dummy index for dummy stamping */
  virtual MYINT getDummyLinearIndex() const { return this->nrNz_; }

  /** resets the matrix values to zero */
  virtual void resetvalues() {
    for (MYINT i = 0; i < this->nrNz_+1; i++)
      values_[i] = 0.0;
  }

  /** prints out the matrix */
  virtual void printMatrix(std::string& fileName) const {
    MYINT i, j, nnz = 0;
    std::ofstream myfile;
    // print this into a file and overwrite the file
    myfile.open (fileName.c_str(), std::ios::out);
    myfile << "% print CRSMatrix r=" << this->nrRows_ << " c=" << this->nrCols_ << "\n";
    myfile << "A = sparse(" << this->nrRows_ << "," << this->nrRows_ << ");\n";
    for (i = 0; i < this->nrRows_; i++) {
        for (j = rowPtr_[i]; j < rowPtr_[i+1]; j++)
          myfile << "A(" << i+1 << "," << this->colPtr_[j]+1 << ")=" << this->values_[j] << "+1E-45; %[" << nnz++ << "]\n";
      }
    myfile << "% END print CRSMatrix \n";
    myfile.close();
  }

// ---------- CRS specific functions -----------------

  /** returns the vector with the diagonal indices */
  inline const std::vector<MYINT>& getDiagonalVector() const { return diagPtr_; }

  /** returns the column index vector */
  inline const std::vector<MYINT>& getColPtr() const { return colPtr_; }

  /** returns the row index vector */
  inline const std::vector<MYINT>& getRowPtr() const { return rowPtr_; }

  /** returns the vector with the matrix values */
  inline const std::vector<DATAT>& getValueVect() const { return values_; }
  inline std::vector<DATAT>& getValueVect() { return values_; }

private:

  /** row pointer vector is of size nrRows+1 */
  std::vector<MYINT> rowPtr_;

  /** column pointer vector is of size of nonzero elements */
  std::vector<MYINT> colPtr_;

  /** values of the matrix */
  std::vector<DATAT> values_;

  /** optionally for the LU solvers we also store the pointers to the diagonal
   * elements (needed for static LU).
   * If there is no diagonal entry in one row than this index is -1*/
  std::vector<MYINT> diagPtr_;
};

#endif /* LA_CRSMATRIX_HPP_ */
