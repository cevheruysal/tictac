/*
 * IP_ExprConstEvaluator.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: benk
 */

#include "IP_ExprConstEvaluator.hpp"

#include "CS_ExpressionBasis.hpp"
#include "CS_FunctionCall.hpp"
#include "CS_AnalogConcatenation.hpp"
#include "CS_BranchProbeFunction.hpp"
#include "CS_AnalogFilterOperationFunction.hpp"
#include "CS_BinaryOperator.hpp"
#include "CS_Number.hpp"
#include "CS_String.hpp"
#include "CS_Infinity.hpp"
#include "CS_ParameterReference.hpp"
#include "CS_AnalogPortReference.hpp"
#include "CS_AnalogBuiltInMathFunction.hpp"
#include "CS_AnalogConditionalExpression.hpp"
#include "CS_EnvironmentParameterFunction.hpp"
#include "CS_UnaryOperator.hpp"
#include "CS_FunctionArgument.hpp"
#include "CS_AnalogFunctionCall.hpp"
#include "CS_HierarchSysParamFunctions.hpp"

#include "IP_NetlistExpander.hpp"

IP_ExprConstEvaluator::IP_ExprConstEvaluator(MYINT depth,bool ignoreActualLevel )
{
  ignoreActualLevel_ = ignoreActualLevel;
  idDepthResolution_ = depth;
  hasError_ = false;
  // here we test if this thing got out of control due to indirect or direct recursion
  reportError(idDepthResolution_ > 1000, "Recursive Parameter definition (Or similar)");
  if (this->hasError_) return;
  temp_ = 27.0;
  vt_ = 0.0; //TODO:
  abstime_ = 0.0;
  expansionStack_ = NULL;
  errorMessage_ = "";
  errorInf_ = NULL;
  //this->setVerb(7);
}

ConstExpressionValue IP_ExprConstEvaluator::evalConstExpression(
    CS_ExpressionBasis *expr, const ExpansionStack* stack)
{
  hasError_ = false;
  errorMessage_ = "";
  errorInf_ = NULL;
  expansionStack_ = stack;
  // start visiting this node
  this->visitNode(expr);
  expansionStack_ = NULL;
  return ret_;
}

void IP_ExprConstEvaluator::Visit(CS_AnalogConcatenation &node){
  VarType  ftype = (VarType)node.getExpr(0)->getTypes();
  // get the type of the first element (THERE SHOULD BE AT LEAST ONE!)
  if (ftype < REAL) ftype = REAL;

  ConstExpressionValue mret;

  // get the type of the array (either string array or real arrays)
  if (ftype == REAL)
    mret.valueType_ = REAL_ARRAY;
  else
    mret.valueType_ = STRING_ARRAY;
  // loop through each element
  for (MYINT i = 0; i < (MYINT)node.getLength(); i++ )
    {
      VarType  atype = (VarType)node.getExpr(i)->getTypes();
      if (atype < REAL) atype = REAL;
      reportError( (atype != ftype), "Concatenations MUST have the same types" , &node);
      if (this->hasError_) return;
      // Visit the node
      this->visitNode( node.getExpr(i) );
      reportError( (ret_.valueType_ != ftype), "Concatenations MUST have the same types" , &node);
      if (this->hasError_) return;
      // just store this element in an array
      if (ftype == REAL)
        mret.aval_.push_back(ret_.dval_);
      else
        mret.saval_.push_back(ret_.sval_);
    }

  // this is now the returned value
  ret_ = mret;
}
void IP_ExprConstEvaluator::Visit(CS_BinaryOperator &node){
  // TODO: implement this for strings, string array and arrays as well ...
  // TODO: for all the general case a lot of work, but stright forward
  // TODO: first check the compatibility of the two operators
  reportError(true,"CS_BinaryOperator not yet implemented", &node);
}
void IP_ExprConstEvaluator::Visit(CS_Number &node){
  ret_.valueType_ = REAL;
  ret_.dval_ = node.evalConst();
}
void IP_ExprConstEvaluator::Visit(CS_String &node){
  ret_.valueType_ = STRING;
  ret_.sval_ = node.getStringValue();
}
void IP_ExprConstEvaluator::Visit(CS_ParameterReference &node){
  if (this->hasError_) return;
  // We simply get the initial expression from a parameter from the expansion stack and
  // then in a separate evaluator we try to evaluate this
  // TODO: we could solve this without creating a new "IP_ExprConstEvaluator"
  std::string paramName = node.getParamObject()->getName();
  this->idDepthResolution_++;
  IP_ExprConstEvaluator evaluator(idDepthResolution_);
  // THERE MUST BE an *ExpressionBasis !!! for a parameter
  TICTAC_SPTR<CS_ExpressionBasis> initEpr =
      expansionStack_->getParameterInitialValue(paramName, ignoreActualLevel_);
  ret_ = evaluator.evalConstExpression(initEpr.get(), expansionStack_ );
  // check if this had error
  this->reportError( evaluator.hasError(), evaluator.getErrorMessage(), evaluator.getFirstErrorNode() );
  this->idDepthResolution_--;
}

void IP_ExprConstEvaluator::Visit(CS_AnalogBuiltInMathFunction &node){
  if (this->hasError_) return;
  reportError(true,"CS_AnalogBuiltInMathFunction not yet implemented", &node);
  // TODO:
}

void IP_ExprConstEvaluator::Visit(CS_AnalogConditionalExpression &node){
  if (this->hasError_) return;
  // visit the condition
  visitNode( node.getCondition() );
  // the condition's result must be scalar real
  reportError( (ret_.valueType_ != REAL), "Consdition expression must be scalar" , &node);
  if (this->hasError_) return;
  // if condition is true than just visit the true branch
  if (ret_.dval_ != 0.0)
    visitNode( node.getThenExpr() );
  else
    visitNode( node.getElseExpr() );
}
void IP_ExprConstEvaluator::Visit(CS_EnvironmentParameterFunction &node){
  switch (node.getFunctionType()) {
    // return the constant values ...
    case VERILOG_ENVIRONFUNC_TEMPERATURE : { ret_.valueType_=REAL; ret_.dval_ = temp_; break;}
    case VERILOG_ENVIRONFUNC_VT          : { ret_.valueType_=REAL; ret_.dval_ = vt_;   break;}
    case VERILOG_ENVIRONFUNC_ABSTIME     : { ret_.valueType_=REAL; ret_.dval_ = abstime_; break;}
    default: {
      reportError(true,"This function cannot be evaluated", &node);
    }
  };
}
void IP_ExprConstEvaluator::Visit(CS_UnaryOperator &node){
  if (this->hasError_) return;
  // visit children
  VisitChildren(node);
  //
  switch (node.getOpType()) {
    case VERILOG_UNAR_PLUS :{ /* do nothing*/ break; }//= 1, // +
    case VERILOG_UNAR_MIN :{ //= 2,  // -
      reportError( (ret_.valueType_ != REAL) && (ret_.valueType_ != REAL_ARRAY),
          "Expression must be real or array of reals" , &node);
      if (this->hasError_) return;
      // apply for reals
      if (ret_.valueType_ == REAL)
        ret_.dval_ = -ret_.dval_;
      // apply for arrays
      if (ret_.valueType_ == REAL_ARRAY) {
          for (MYINT i = 0; i < (MYINT)ret_.aval_.size(); i++)
            ret_.aval_[i] = - ret_.aval_[i];
        }
      break;
    }
    case VERILOG_UNAR_LOGNOT :{ //= 3, // !
      reportError( (ret_.valueType_ != REAL) && (ret_.valueType_ != REAL_ARRAY),
          "Expression must be real or array of reals" , &node);
      if (this->hasError_) return;
      // apply for reals
      if (ret_.valueType_ == REAL)
        ret_.dval_ = ( ((MYINT)(ret_.dval_) != 0) ? 0.0 : 1.0 );
      // apply for arrays
      if (ret_.valueType_ == REAL_ARRAY) {
          for (MYINT i = 0; i < (MYINT)ret_.aval_.size(); i++)
            ret_.aval_[i] = ( ((MYINT)(ret_.aval_[i]) != 0) ? 0.0 : 1.0 );
        }
      break;
    }
    case VERILOG_UNAR_REDNOT :{ //= 4, //~
      reportError( (ret_.valueType_ != REAL) && (ret_.valueType_ != REAL_ARRAY),
          "Expression must be real or array of reals" , &node);
      if (this->hasError_) return;
      // apply for reals
      if (ret_.valueType_ == REAL)
        ret_.dval_ = (!((MYINT)(ret_.dval_) != 0));
      // apply for arrays
      if (ret_.valueType_ == REAL_ARRAY) {
          for (MYINT i = 0; i < (MYINT)ret_.aval_.size(); i++)
            ret_.aval_[i] = (~((MYINT)(ret_.aval_[i])));
        }
      break;
    }
    default: {
      reportError(true, "UnaryOperator cannot be evaluated", &node);
    }
  };
}

void IP_ExprConstEvaluator::reportError(bool cond, std::string message, const CS_ErrorInfo *errorInf) {
  if (cond) {
      hasError_ = true;
      errorMessage_ += message + "\n";
      errorInf_ = errorInf;
  }
}

// ==== visitors with errors
void IP_ExprConstEvaluator::Visit(CS_BranchProbeFunction &node){  reportError(true,"Voltage values cannot be evaluated", &node);}
void IP_ExprConstEvaluator::Visit(CS_AnalogFilterOperationFunction &node){  reportError(true,"Analog filter functions cannot be evaluated", &node);}
void IP_ExprConstEvaluator::Visit(CS_Infinity &node){  reportError(true,"Analog filter functions cannot be evaluated", &node);}
void IP_ExprConstEvaluator::Visit(CS_AnalogPortReference &node){  reportError(true,"Voltage values cannot be evaluated", &node);}
void IP_ExprConstEvaluator::Visit(CS_ExpressionBasis &node) {  reportError(true,"Expression not supported", &node);}
void IP_ExprConstEvaluator::Visit(CS_FunctionCall &node){  reportError(true,"Function call not yet supported", &node);}
void IP_ExprConstEvaluator::Visit(CS_FunctionArgument &node){  reportError(true,"Function call not yet supported", &node);}
void IP_ExprConstEvaluator::Visit(CS_AnalogFunctionCall &node){  reportError(true,"Function call not yet supported", &node);}
void IP_ExprConstEvaluator::Visit(CS_HierarchSysParamFunctions &node){  reportError(true,"Hierarchical function calls not yet supported", &node);}
