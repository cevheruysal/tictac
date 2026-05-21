/*
 * CS_AnalogVariableAssignment.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#include "CS_AnalogVariableAssignment.hpp"

CS_AnalogVariableAssignment::CS_AnalogVariableAssignment(CS_VariableReference *tarVal, CS_ExpressionBasis *evalExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
CS_AnalogStatement( module, startLine, startColumn), targetVal_(tarVal), evalExpr_(evalExpr)
{
  // we got directly a variable, no argument is needed
  targetExpr_ = NULL;
  // todo: maybe do some checks ...
}


CS_AnalogVariableAssignment::CS_AnalogVariableAssignment(CS_FunctionArgument *argument,
    CS_ExpressionBasis *evalExpr, const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_AnalogStatement( module, startLine, startColumn),
    targetVal_(NULL),targetExpr_(argument), evalExpr_(evalExpr)
{

  // check the assignment
  CS_VariableReference *tarVal = dynamic_cast<CS_VariableReference*>(targetExpr_->getArgument());
  VERILOG_ERROR_COND_STOP( tarVal == 0, "In assignments only variables can be assigned " , this );
  // todo: maybe do some checks ...
}
