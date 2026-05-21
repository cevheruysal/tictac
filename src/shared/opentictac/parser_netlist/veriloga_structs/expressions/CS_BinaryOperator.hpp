/*
 * CS_BinaryOperator.hpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#ifndef BINARYOPRATOR_HPP_
#define BINARYOPRATOR_HPP_

#include "CS_ExpressionBasis.hpp"

typedef enum{
  VERILOG_BINAR_MODULUS = 0,
  VERILOG_BINAR_PLUS = 1,
  VERILOG_BINAR_MIN = 2,
  VERILOG_BINAR_MUL = 3,
  VERILOG_BINAR_DIV = 4,
  VERILOG_BINAR_DIV_SAFE = 5,
  VERILOG_BINAR_GT = 6,
  VERILOG_BINAR_LT = 7,
  VERILOG_BINAR_GEQ = 8,
  VERILOG_BINAR_LEQ = 9,
  VERILOG_BINAR_LOGEQ = 10,
  VERILOG_BINAR_LOGNEQ = 11,
  VERILOG_BINAR_LOGAND = 12,
  VERILOG_BINAR_LOGOR = 13,
  VERILOG_BINAR_REDAND = 14,
  VERILOG_BINAR_REDNAND = 15,
  VERILOG_BINAR_REDOR = 16,
  VERILOG_BINAR_REDXOR = 17,
  VERILOG_BINAR_REDNOR = 18,
  VERILOG_BINAR_REDXNOR = 19,
  VERILOG_BINAR_LSHIFT = 20,
  VERILOG_BINAR_RSHIFT = 21
  // add "**" todo: add the power operator ..
} BinarOpType;

/** class to model basic operations (no functions) with two operands
 *
 * see binary_operator in the LRM */
class CS_BinaryOperator : public CS_ExpressionBasis
{
public:

  CS_BinaryOperator(BinarOpType ty, CS_ExpressionBasis* exprL, CS_ExpressionBasis* exprR,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_BinaryOperator() {;}

  virtual double evalConst() const;

  virtual int getClasses() const;

  virtual int getTypes() const;

  /** returns the right expression */
  CS_ExpressionBasis* getLeftExpr() { return targetExprL_; }

  /** returns the left expression */
  CS_ExpressionBasis* getRightExpr() { return targetExprR_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_BinaryOperator &)(*this)); }

  /** return the type of this binary operation */
  BinarOpType getOperationType() const { return opType_;}

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() ;

  /** returns true if the operands are strings */
  bool hasStringOperands() const { return hasStringOperands_; }

  /** the size of the expression */
  virtual int getExprSize() const { return (targetExprL_->getExprSize() + targetExprR_->getExprSize())/2;}

private:

   /** type of the binary operator */
   BinarOpType opType_;

   /** if both operands are string this is true*/
   bool hasStringOperands_;

   /** left expression */
   CS_ExpressionBasis* targetExprL_;
   /** right expression */
   CS_ExpressionBasis* targetExprR_;

};

#endif /* BINARYOPRATOR_HPP_ */
