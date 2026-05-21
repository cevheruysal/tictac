/*
 * CS_ValueRange.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#include "CS_ValueRange.hpp"

  CS_ValueRange::CS_ValueRange(
      const CS_ModuleDeclaration* module, int startLine, int startColumn,
      CS_ExpressionBasis* v1, CS_ExpressionBasis* v2,
      ConstraintType ct, VarType valType, bool strictLeft, bool strictRight ):
      startLine_(startLine), startColumn_ (startColumn), module_(module), ctType_(ct)
  {
    minVal_ = v1;
    maxVal_ = v2;
    valType_ = valType;
    strictLeft_ = strictLeft;
    strictRight_ = strictRight;
  }
