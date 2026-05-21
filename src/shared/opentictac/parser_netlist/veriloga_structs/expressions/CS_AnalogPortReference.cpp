/*
 * CS_AnalogPortReference.cpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#include "CS_AnalogPortReference.hpp"

#include <math.h>

CS_AnalogPortReference::CS_AnalogPortReference(CS_Port *portObj,  CS_ExpressionBasis *indexExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
    CS_IndexedIdentifier( portObj, indexExpr, module, startLine, startColumn) ,
    port_(portObj)
{
  // nothing to do here
  VERILOG_ERROR_COND_STOP( portObj == 0 , "Identifier must be a port ", this);
}


CS_AKDependency CS_AnalogPortReference::propagateDependencies()
{
  // in this function is not determined what should be returned, therefore throw an error
  CS_AKDependency ret_set;
  VERILOG_PRINT_L2( verb(), " CS_AnalogPortReference::propagateDependencies should not be called" );
  VERILOG_ERROR_STOP("Port can not be used directly in expressions", this);
  // return the dependencies
  return ret_set;
}
