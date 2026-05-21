/*
 * CS_IndexedIdentifier.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: benk
 */

#include "CS_IndexedIdentifier.hpp"
#include <math.h>

CS_IndexedIdentifier::CS_IndexedIdentifier( CS_Identifier *valObj,  CS_ExpressionBasis *indexExpr,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
    CS_ExpressionBasis(module, startLine, startColumn), valueObj_(valObj), indexExpr_(indexExpr)
{
  // make a check the type of indexExpr_
  constIndexVal_ = -1;
  isConstantIndexingExpression_ = false;
  if (indexExpr) {
      VarType indexTy = (VarType)indexExpr->getTypes();
      if (indexTy & (REAL | INTEGER_ARRAY | REAL_ARRAY | STRING | STRING_ARRAY) ){
          // throw an error
          VERILOG_ERROR_STOP("Expression in the index should be an integer ", this);
      }
      // if the expression is constant then just evaluate and store this index
      VariableClass indexCalss = (VariableClass)indexExpr->getClasses();
      if ( indexCalss <= CONSTANT){
          constIndexVal_ = verilog_nearestInt(indexExpr->evalConst());
          isConstantIndexingExpression_ = true;
      }
  }
}

