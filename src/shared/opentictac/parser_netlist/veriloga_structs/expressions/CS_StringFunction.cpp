/*
 * CS_StringExpression.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: benk
 */

#include "CS_StringFunction.hpp"
#include "CS_VariableReference.hpp"

CS_StringFunction::CS_StringFunction(
    CS_VariableReference *targetVariable,
    CS_ExpressionBasis *targetExpression,
    StringFunctionType stringExprType,
    CS_ExpressionBasis** args, int nrArgs,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_FunctionCall(args, nrArgs, module, startLine, startColumn, false),
    stringExpression_(targetExpression), stringExprType_(stringExprType)
{
  for (int i = 0 ; i < this->getNrArg() ; i++)
    VERILOG_ERROR_COND_STOP( !this->getArg(i), "This function can not have default arguments", this);

  if (targetVariable)
    {
      VERILOG_ERROR_COND_STOP( (stringExpression_->getTypes() != STRING),
          "expression must be a string", stringExpression_ );
    }
  else
    // test, if the target variable is a string variable
    VERILOG_ERROR_COND_STOP( ((stringExpression_->getTypes() != STRING)
      && (stringExpression_->getTypes() != STRING_ARRAY)),
      "expression must be a string", stringExpression_ );

  // test if the number of arguments are OK
  switch (stringExprType_){
    case VERILOG_STRING_LEN :{
      VERILOG_ERROR_COND_STOP(  this->getNrArg() != 0,
            "string function LEN has no arguments ", this );
      break;
    }
    case VERILOG_STRING_SUBSTR :{
      VERILOG_ERROR_COND_STOP(  this->getNrArg() != 2,
            "string function SUBSTR has two arguments ", this );
      // both arguments should be integers
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() > INTEGER,
          "string function SUBSTR needs INTEGER as first argument "
          << this->getArg(0)->getTypes() , this->getArg(0) );
      VERILOG_ERROR_COND_STOP( this->getArg(1)->getTypes() > INTEGER,
          "string function SUBSTR needs INTEGER as second argument "
          << this->getArg(1)->getTypes() , this->getArg(1) );
      break;
    }
    case VERILOG_STRING_REPLIC :{
      VERILOG_ERROR_COND_STOP(  this->getNrArg() != 1,
            "The string replicator has only one argument", this );
      // first argument should be integer
      VERILOG_ERROR_COND_STOP( this->getArg(0)->getTypes() > REAL,
          "The string replicator needs INTEGER" << this->getArg(0)->getTypes() , this->getArg(0) );
      break;
    }
  }
}


int CS_StringFunction::getClasses() const
{
  // this is dependent on an internal variable
  return INTERNALVAR;
}


int CS_StringFunction::getTypes() const
{
  // return the corresponding type
  switch (stringExprType_){
  case VERILOG_STRING_LEN :{
    //std::cout << "INT\n";
    return INTEGER;
  }
  case VERILOG_STRING_REPLIC :
  case VERILOG_STRING_SUBSTR :{
    return STRING;
  }
  }
  // default return
  return STRING;
}


