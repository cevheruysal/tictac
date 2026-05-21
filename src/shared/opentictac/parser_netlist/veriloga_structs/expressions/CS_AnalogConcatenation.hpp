/*
 * CS_AnalogConcatenation.hpp
 *
 *  Created on: Dec 11, 2012
 *      Author: benk
 */

#ifndef ANALOGCONCATENATION_HPP_
#define ANALOGCONCATENATION_HPP_

#include "CS_ExpressionBasis.hpp"
#include "verilog_comp.hpp"

/** This class represents the expressions grouped into a set, e.g., {1,2,3}
 *
 * see analog_concatenation in LRM */
class CS_AnalogConcatenation: public CS_ExpressionBasis
{
public:

  /** Ctor */
  CS_AnalogConcatenation(const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Ctor*/
  virtual ~CS_AnalogConcatenation() {;}

  /** returns the size of the set */
  int getLength() const { return setLength_; }

  /** adds one */
  void addExpr( CS_ExpressionBasis *ex ) { exprs_.push_back(ex); setLength_++; }

  /** returns the expression at the given index */
  const CS_ExpressionBasis* getExpr(int i) const { return exprs_[i]; }

  /** returns the expression at the given index */
  CS_ExpressionBasis* getExpr(int i) { return exprs_[i]; }

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const {
    int tmpV = 0;
    int cellType = this->getTypes();
    for (int i = 0; i < setLength_ ; i++) { tmpV = tmpV | exprs_[i]->getClasses(); }
    // for string concats we need extra internal variables
    if ( (cellType == STRING) || (cellType == STRING_ARRAY) )
      tmpV = tmpV | INTERNALVAR;
    return tmpV;
  }

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const ;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogConcatenation &)(*this)); }

  //this should not occur in the statment's expression therefore no implementation here
  //virtual CS_AKDependency propagateDependencies();

  /** function that creates a deep copy of the expression itself */
   virtual CS_ExpressionBasis* copy() {
     // create new object
     CS_AnalogConcatenation *ret = new CS_AnalogConcatenation( this->getModule(),
         this->getStartLine(), this->getStartColumn());
     // copy the concatenation
     for (int i = 0; i < setLength_; i++)
       ret->addExpr(exprs_[i]->copy());
     // return the object
     return ret;
   }

   /** */
   virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
     VERILOG_PRINT_L2( verb(), " doDifferentiate function not implemented for CS_AnalogConcatenation");
     VERILOG_ERROR_STOP("Internal Error", this);
     return 0;
   }

   /** */
   virtual CS_AKDependency propagateDependencies( );

   /** the size of the expression */
   virtual int getExprSize() const { return setLength_;}

private:

  /** the vector containing the expressions */
  std::vector<CS_ExpressionBasis*> exprs_;

  /** size of the Set*/
  int setLength_;

};

#endif /* ANALOGCONCATENATION_HPP_ */
