/*
 * CS_AnalogConditionalExpression.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#include "CS_AnalogConditionalExpression.hpp"
#include "CS_Number.hpp"

CS_AnalogConditionalExpression::CS_AnalogConditionalExpression(CS_ExpressionBasis *ifExpr, CS_ExpressionBasis *thenExpr, CS_ExpressionBasis *elseExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_ExpressionBasis(module, startLine, startColumn), ifExpr_(ifExpr), thenExpr_(thenExpr), elseExpr_(elseExpr)
{
  // nothing to do yet
}


double CS_AnalogConditionalExpression::evalConst() const {
  double val  = ifExpr_->evalConst();
  // evaluate the expressions accordingly
  if (val != 0)
    return thenExpr_->evalConst();
  else
    return elseExpr_->evalConst();
}

CS_AKDependency CS_AnalogConditionalExpression::propagateDependencies(){
  // todo: we could optimize this if ifExpr_ is constant, or completely eliminate
  // as a condition
  CS_AKDependency ret_set;
  CS_AKDependency tmpSet1 = this->thenExpr_->propagateDependencies();
  CS_AKDependency tmpSet2 = this->elseExpr_->propagateDependencies();
  // union of the two sets
  ret_set.uniteDependencies(tmpSet1);
  ret_set.uniteDependencies(tmpSet2);
  // store the combined dependency in the actual node
  setActVarDependency(ret_set);
  // return the dependencies
  return ret_set;
}

/** method for recursive differentiation with AD*/
CS_ExpressionBasis* CS_AnalogConditionalExpression::doDifferentiate(int sysVarIndex, int *flag){
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  CS_AnalogConditionalExpression* retExp = 0;
  CS_ExpressionBasis *arg1 = thenExpr_;
  CS_ExpressionBasis *arg2 = elseExpr_;
  CS_ExpressionBasis *arg1Deriv = 0;
  CS_ExpressionBasis *arg2Deriv = 0;
  // compute the derivatives of the arguments
  if (arg1->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0)
    {
      //std::cout << "-1-" << arg1->getActVarDependency().toString() << std::endl;
      arg1Deriv = arg1->doDifferentiate(sysVarIndex, flag);
    }
  else
    arg1Deriv = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());

  if (arg2->getActVarDependency().isSysVarIndexPresent(sysVarIndex) >= 0)
    {
      //std::cout << "-2-" << arg2->getActVarDependency().toString() << " sysI:" << sysVarIndex << std::endl;
      arg2Deriv = arg2->doDifferentiate(sysVarIndex, flag);
    }
  else
    arg2Deriv = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());

  // create and return the final expression
  retExp = new CS_AnalogConditionalExpression(ifExpr_, arg1Deriv, arg2Deriv,
      getModule(), getStartLine(), getStartColumn());
  return retExp;
}
