/*
 * LS_LUSolver_STUDENT.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: benk
 */

#ifndef LS_LUSOLVER_STUDENT_HPP_
#define LS_LUSOLVER_STUDENT_HPP_

#include "LS_SolverBase.hpp"
#include "LA_CRSMatrix.hpp"
#include "sim_utils.hpp"

/** class for direct LU solving */
template <typename DATAT>
class LS_LUSolver_STUDENT: public LS_SolverBase<DATAT> {

public:

  /** Ctor where the input is the already */
  LS_LUSolver_STUDENT(LA_CRSMatrix<DATAT> *matrix) :
    LS_SolverBase<DATAT>(matrix), CRSmatrix_(matrix) {
    table_.resize(matrix->getNrRows(),-1);

    // Markowitz test are made in the solver factory so no need to make it here
    // there is much easier to make this test, since they are stored in sets

    // get the verbosity for the solver
    std::string className = "LUSolver_STUDENT";
    verb_ = UT_SIMVerboseLevelHandle::getVerbLevel(className);
  }

  /** empty Dtor */
  virtual ~LS_LUSolver_STUDENT() {;}

  /** virtual method to solve the unknown for the i
   * @param x the initial solution and as output the solution of the system
   * @param rhs the right hand side */
  virtual void solve(std::vector<DATAT>& x, const std::vector<DATAT>& rhs)
  {
    // -------DEBUG ----------
    if (verb_ > 4) {
        DATAT * ptr = CRSmatrix_->getPointer(0);
        std::string tmp = "";
        for (MYINT i=0; i < CRSmatrix_->getNz(); i++)
          tmp += sim_doubleToString((MYREAL)(ptr[i])) + ",";
        SIM_PRINT_L5(verb_, "A={" << tmp << "}");
    }

    // make LU factorization
    doLUFactor();

    // -------DEBUG ----------
    if (verb_ > 4) {
        DATAT * ptr = CRSmatrix_->getPointer(0);
        std::string tmp = "";
        for (MYINT i=0; i < CRSmatrix_->getNz(); i++)
          tmp += sim_doubleToString((MYREAL)(ptr[i])) + ",";
        SIM_PRINT_L5(verb_, "A={" << tmp << "}");
    }

    // make FB substitution
    doFBSubstitution(x, rhs);

    // -------DEBUG ----------
    if (verb_ > 4) {
        std::string tmp = "";
        for (MYINT i=0; i < CRSmatrix_->getNrRows() ; i++)
          tmp += sim_doubleToString((MYREAL)(rhs[i])) + ",";
        SIM_PRINT_L5(verb_, "rhs={" << tmp << "}");
        tmp = "";
        for (MYINT i=0; i < CRSmatrix_->getNrRows() ; i++)
          tmp += sim_doubleToString((MYREAL)(x[i])) + ",";
        SIM_PRINT_L5(verb_, "x={" << tmp << "}");
    }
  }

  /** resets the matrix */
  virtual void setMatrix(LA_MatrixBasis<DATAT> *matrix) {
    this->matrix_ = matrix;
    table_.resize(matrix->getNrRows(),-1);
    LA_CRSMatrix<DATAT> *CRSmatrix = dynamic_cast<LA_CRSMatrix<DATAT>*>(matrix);
    SIM_ERROR_COND_STOP( CRSmatrix==NULL ,"LS_LUSolver works only with CRS structure");
    CRSmatrix_ = CRSmatrix;
  }

private:

  /** inverts a real number */
  inline MYREAL my_invert(MYREAL inVal) const { return 1.0/inVal;}
  /** inverts a complex number */
  inline COMPLEX my_invert(COMPLEX inVal) const {
    COMPLEX t;
    MYREAL tt = inVal.re*inVal.re + inVal.im*inVal.im;
    t.re = inVal.re/tt; t.im = -inVal.im/tt; return t;
  }

  /** used for solving the zero pivot in the LU decomposition */
  inline MYREAL my_convert(MYREAL inVal, MYREAL dummy) const { (dummy == 0.0); return inVal;  }
  /** used for solving the zero pivot in the LU decomposition */
  inline COMPLEX my_convert(MYREAL inVal, COMPLEX dummy) const { COMPLEX t; t.im = 0; t.re = inVal; return t; }
  /** fabs function for MYREAL values */
  inline MYREAL my_fabs(MYREAL inval) const { return fabs(inval); }
  /** fabs function for COMPLEX16 values */
  inline MYREAL my_fabs(COMPLEX inval) const { return sqrt(inval.re*inval.re+inval.im*inval.im); }
  /** conversion function from real to complex to thread pmin situations (zero pivoting)*/
  MYREAL my_getReal(MYREAL inVal) { return inVal; }
  /** conversion function from real to complex to thread pmin situations (zero pivoting)*/
  MYREAL my_getReal(COMPLEX inVal) { return inVal.re; }

// ====================== LU factorization ==============================
  /** makes the LU factorization on the internal matrix, which must be of
   * Markowitz structure */
  void doLUFactor()
  {
    const std::vector<MYINT>& rowPtr = CRSmatrix_->getRowPtr();
    const std::vector<MYINT>& iu_index = CRSmatrix_->getDiagonalVector();
    const std::vector<MYINT>& colInd = CRSmatrix_->getColPtr();
    std::vector<DATAT>& A = CRSmatrix_->getValueVect();
    DATAT h = my_convert(1.0,A[0]);
    std::vector<MYINT>& tabl = table_;

    MYINT verb = 5, iuflow = 0;
    MYREAL pmin = 1E-15;

    /*=========== START STUDENT PROJECT A1 ===================== */
    const MYINT *colTabl;
    MYINT colIndTmp, actualRow, nrLColumns, colIndI, i, j;
    for (actualRow = 0; actualRow < CRSmatrix_->getNrRows(); actualRow++)
      {
        bool needsElimin = ((iu_index[actualRow] - rowPtr[actualRow]) > 1 );
        if (needsElimin)
          {
            /* ======================== FACTORIZATION ========================= */
            /* set up the lookup table */
            for (i = rowPtr[actualRow]; i < rowPtr[actualRow+1]; i++)
              tabl[ colInd[i] ] = i;

            /* number of elements in L part that we can eliminate actually */
            nrLColumns = iu_index[actualRow] - rowPtr[actualRow] - 1;
            colIndI = rowPtr[actualRow];
            colTabl = &(colInd[0]);
            /* for each row impacting this row */
            for (i = 0 ; i < nrLColumns ; i++)
              {
                colIndTmp = colInd[colIndI+i];

                /* make elimination only if the element is nonzero */
                if (my_fabs(A[ tabl[colIndTmp] ]))
                  {
                    MYINT startI = iu_index[colIndTmp];
                    MYINT endI = rowPtr[colIndTmp+1];

                    A[ tabl[colIndTmp] ] =
                        A[ tabl[colIndTmp] ] * A [ iu_index[colIndTmp]-1 ];
                    h = A[ tabl[colIndTmp] ];
                    for ( j = startI ; j < endI ; j++)
                      A[ tabl[ colTabl[j] ] ] = A[ tabl[ colTabl[j] ] ] - h*A[j];
                  }
               }
           } // end if elimination is needed

        /* compute the 1/a_jj only when we are done with this row */
        j = iu_index[actualRow]-1;
        if (my_fabs( A[ j ] ) < pmin)
          {
            A[j] = my_convert( (my_getReal(A[j]) < 0.0) ? ((-1.0 / pmin)) : ((+1.0 / pmin)) , A[0] );
            iuflow++;
          }
        else
          A[j] = my_invert(A[j]);
      }

    /*============== END STUDENT PROJECT A1 ===============*/

    // print underflow
    if (iuflow > 0)
      SIM_PRINT_L0(verb,"underflow in LU factorization " << iuflow << " times, with PMIN=" << pmin);
  }

  // ==================== FB substituation =============================
  /** makes the forward and backward substituation with the factorized matrix
   * @param X the unknown vector
   * @param rhs the right hand side */
  void doFBSubstitution(std::vector<DATAT>& X, const std::vector<DATAT>& rhs) const
  {
    //Local variables:
    MYINT i,j,k,l;
    MYINT jstart, jstop, verb = this->verb();
    DATAT tmpr;
    MYINT n = this->getSolverMatrix()->getNrRows();
    const std::vector<MYINT>& ilstrt = CRSmatrix_->getRowPtr();
    const std::vector<MYINT>& iustrt = CRSmatrix_->getDiagonalVector();
    const std::vector<MYINT>& colinf = CRSmatrix_->getColPtr();
    const std::vector<DATAT>& A = CRSmatrix_->getValueVect();

    SIM_PRINT_L0(verb,"doFBSubstitution ...");

    /*=========== START STUDENT PROJECT A1 ===================== */
    // forward substitution
    X[0] = rhs[0];
    for (i = 1; i < n; i++)
      {
        jstart = ilstrt[i];
        jstop = iustrt[i]-1;
        tmpr = my_convert(0.0,A[0]);
        for (j = jstart; j < jstop; j++)
          {
            tmpr = tmpr + A[j] * X[colinf[j]];
          }
        X[i] = rhs[i] - tmpr;
        //SIM_PRINT_L5(verb,"FS i=" << i << " jstart="<< jstart << " jstop=" << jstop << " x[i]" << X[i]);
      }
    // backward substitution
    for (i = n-1; i >= 0 ; i--)
      {
        jstart = iustrt[i];
        jstop = ilstrt[i+1];
        tmpr = my_convert(0.0,A[0]);
        for (j = jstart; j < jstop; j++)
          {
            tmpr = tmpr + A[j]* X[colinf[j]];
          }
        X[i] = X[i] - tmpr;
        //Diagonal entry was already inverted
        X[i] = X[i] * A[jstart-1];
        //SIM_PRINT_L5(verb,"BS i=" << i << " jstart="<< jstart << " jstop=" << jstop << " x[i]" << X[i]);
      }
    /*=========== END STUDENT PROJECT A1 ===================== */

  }

  /** pointer to the CRS matrix */
  LA_CRSMatrix<DATAT> *CRSmatrix_;

  /** temporary lookup table for the LU factorization */
  std::vector<MYINT> table_;

  /** verbosity of the solver*/
  MYINT verb_;

};



#endif /* LS_LUSOLVER_STUDENT_HPP_ */
