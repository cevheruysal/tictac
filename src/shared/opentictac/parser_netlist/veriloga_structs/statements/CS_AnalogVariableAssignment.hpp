/*
 * CS_AnalogVariableAssignment.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGVARIABLEASSIGN_HPP_
#define CS_ANALOGVARIABLEASSIGN_HPP_

#include "CS_AnalogStatement.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_VariableReference.hpp"
#include "CS_FunctionArgument.hpp"

/** this class models the assignment instruction tarExpr = EXPR,
 * where tarExpr, should be either a Variable or an Indexed variable
 *
 * see analog_variable_assignment (or variable_assignment) in LRM */
class CS_AnalogVariableAssignment : public CS_AnalogStatement
{
public:

  CS_AnalogVariableAssignment(CS_VariableReference *tarVal, CS_ExpressionBasis *evalExpr, const CS_ModuleDeclaration* module, int startLine, int startColumn);

  CS_AnalogVariableAssignment(CS_FunctionArgument *argument, CS_ExpressionBasis *evalExpr, const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_AnalogVariableAssignment() {;}

  virtual bool isElementaryInsruction() const { return true; }

  /** returns the target variable of the assignment */
  CS_VariableReference* getTargetVariable() {
    if (targetVal_ != NULL)
      return targetVal_;
    else
      return dynamic_cast<CS_VariableReference*>(targetExpr_->getArgument());
  }

  /** returns the evaluation expression s*/
  CS_ExpressionBasis* getExpression() { return evalExpr_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogVariableAssignment &)(*this)); }

private:

  /** the target of the assignment */
  CS_VariableReference *targetVal_;

  /** target expression if this is inside a function */
  CS_FunctionArgument *targetExpr_;

  /** expression that needs to be evaluated */
  CS_ExpressionBasis *evalExpr_;
};

#endif /* CS_ANALOGVARIABLEASSIGN_HPP_ */
