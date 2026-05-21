/*
 * CS_ParameterReference.cpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#include "CS_ParameterReference.hpp"

CS_ParameterReference::CS_ParameterReference(CS_Parameter *parObj,  CS_ExpressionBasis *indexExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
    CS_IndexedIdentifier(parObj, indexExpr, module, startLine, startColumn), parObj_(parObj)
{
  // indexExpr must be null otherwise there is an error
  VERILOG_ERROR_COND_STOP( (indexExpr != 0) && !parObj_->isLocalParam() &&
      !parObj->isVariableArrayLength() ,
      "Parameter '" << parObj_->getName() << "' can not be indexed ", this);
  VERILOG_ERROR_COND_STOP( parObj == 0 , "Identifier must be a parameter ", this);
}

