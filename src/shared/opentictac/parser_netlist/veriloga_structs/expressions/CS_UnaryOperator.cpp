/*
 * CS_UnaryOperator.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: benk
 */

#include "CS_UnaryOperator.hpp"

#include <math.h>

CS_UnaryOperator::CS_UnaryOperator(UnaryOpType ty, CS_ExpressionBasis* expr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
CS_ExpressionBasis(module, startLine, startColumn), opType_(ty), targetExpr_(expr)
{

}

double CS_UnaryOperator::evalConst() const
{
  // based on the type we call the evaluation function and return the value
  double val = targetExpr_->evalConst();
  switch (opType_){
    case VERILOG_UNAR_PLUS:   { return val;  }
    case VERILOG_UNAR_MIN:    { return -val; }
    case VERILOG_UNAR_LOGNOT: { return (val)?1.0:0.0; }
    case VERILOG_UNAR_REDNOT: { return (double)(~(verilog_nearestInt(val))); }
    default: {
      VERILOG_ERROR_STOP("Unknown or not supported unary operation", this);
      return val;
    }
  }
}


CS_AKDependency CS_UnaryOperator::propagateDependencies()
{
  CS_AKDependency ret_tmp; // initialize an empty set

  // based on the type we generate the string
  switch (opType_){
    case VERILOG_UNAR_PLUS:
    case VERILOG_UNAR_MIN: {
      // get the dependency list and return it back
      ret_tmp = targetExpr_->propagateDependencies();
      break;
    }
    case VERILOG_UNAR_LOGNOT:
    case VERILOG_UNAR_REDNOT: {
      // call recursively but do not use the result
      targetExpr_->propagateDependencies();
      break;
    }
    default:
      {
        VERILOG_ERROR_STOP("Unknown or not supported unary operation", this);
        break;
      }
  }
  // store it in the actual node
  setActVarDependency(ret_tmp);
  // we return the dependency set
  return ret_tmp;
}

CS_ExpressionBasis* CS_UnaryOperator::doDifferentiate(int sysVarIndex, int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  CS_ExpressionBasis *retExpr = 0;
  CS_ExpressionBasis *argDeriv = targetExpr_->doDifferentiate(sysVarIndex,flag);

  switch (opType_){
    case VERILOG_UNAR_PLUS:{
      // simple dependency
      retExpr = argDeriv;
      break;
    }
    case VERILOG_UNAR_MIN: {
      // get the dependency list and return it back
      retExpr = new CS_UnaryOperator( VERILOG_UNAR_MIN, argDeriv, getModule(), getStartLine(), getStartColumn());
      break;
    }
    case VERILOG_UNAR_LOGNOT:
    case VERILOG_UNAR_REDNOT:
    default:
      {
        VERILOG_ERROR_STOP("Unknown or not supported unary operation", this);
        break;
      }
  }
  // return the created expression
  return retExpr;
}
