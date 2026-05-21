/*
 * CS_Infinity.hpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#ifndef CS_INFINITY_HPP_
#define CS_INFINITY_HPP_

#include "CS_ExpressionBasis.hpp"

/** class contains the infinity constant expression
 *
 * see inf from the LRM */
class CS_Infinity : public CS_ExpressionBasis
{
public:

  /** Ctor */
  CS_Infinity(int sign, const CS_ModuleDeclaration* module, int startLine,
      int startColumn): CS_ExpressionBasis(module, startLine, startColumn), is_neg_(sign) {;}

  virtual ~CS_Infinity() {;}

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const { return CONSTANT; }

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const { return REAL; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_Infinity &)(*this)); }

  /** Infinity should not appear in expressions */
  virtual CS_AKDependency propagateDependencies()
    {
      VERILOG_PRINT_L3(verb()," evalConst function for CS_Infinity not defined ");
      VERILOG_ERROR_STOP("Internal Error", this);
      return CS_AKDependency();
    }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // create new object
    return (new CS_Infinity( is_neg_, this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

  /** @returns the integer signaling if it is negative infinity or not */
  int getIsNeg() const { return is_neg_; }

private:

  /** sign of the infinity */
  int is_neg_;

};


#endif /* CS_INFINITY_HPP_ */
