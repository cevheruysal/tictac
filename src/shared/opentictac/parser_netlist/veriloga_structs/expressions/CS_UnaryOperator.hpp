/*
 * CS_UnaryOperator.hpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#ifndef CS_UNARYOPERATOR_HPP_
#define CS_UNARYOPERATOR_HPP_

#include "CS_ExpressionBasis.hpp"

typedef enum{
  VERILOG_UNAR_NONE = 0, //
  VERILOG_UNAR_PLUS = 1, // +
  VERILOG_UNAR_MIN = 2,  // -
  VERILOG_UNAR_LOGNOT = 3, // !
  VERILOG_UNAR_REDNOT = 4, //~
  // see port_probe_function_call in LRM
  VERILOG_UNAR_PORT_ACCESS_OP = 5 //<x>
} UnaryOpType;

/** the class for expression operations that involve only one target expression
 *
 * see unary_operator in LRM */
class CS_UnaryOperator : public CS_ExpressionBasis
{
public:

  /** Ctor with the required parameters */
  CS_UnaryOperator(UnaryOpType ty,  CS_ExpressionBasis* expr,  const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_UnaryOperator() {;}

  virtual double evalConst() const;

  virtual int getClasses() const { return targetExpr_->getClasses(); }

  virtual int getTypes() const { return targetExpr_->getTypes(); }

  /** returns the expression from within this operator */
  CS_ExpressionBasis* getExpression() { return targetExpr_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_UnaryOperator &)(*this)); }

  /** propagate the dependencies  */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // create new object
    return (new CS_UnaryOperator( opType_, targetExpr_->copy(), this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

  /** @returns the type of the unary operator */
  UnaryOpType getOpType() const { return opType_; }

  /** the size of the expression */
  virtual int getExprSize() const { return targetExpr_->getExprSize();}

private:

  /** type of the operator */
  UnaryOpType opType_;

  /** the target expression of the operator*/
  CS_ExpressionBasis* targetExpr_;
};

#endif /* CS_UNARYOPERATOR_HPP_ */
