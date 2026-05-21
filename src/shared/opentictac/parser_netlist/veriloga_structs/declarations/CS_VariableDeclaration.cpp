/*
 * CS_VariableDeclaration.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#include "CS_VariableDeclaration.hpp"
#include "CS_String.hpp"

#include <math.h>

CS_ExpressionBasis* CS_VariableDeclaration::noUNit_ = NULL;

CS_VariableDeclaration::CS_VariableDeclaration(const CS_ModuleDeclaration* module,
    int startLine, int startColumn,
    std::string name,
    int varIndex, CS_Range* range, CS_ExpressionBasis* initValue,
    VarType type, bool isGenerate, InternalVarType variableType): module_(module)
    , startLine_(startLine)
    , startColumn_(startColumn)
    , range_(range)
    , initValue_(initValue)
    , type_(type)
    , name_(name)
    , varIndex_(varIndex)
    , isReverseNumbered_(false)
    , isGenerateVariable_(isGenerate)
    , variableType_(variableType)
    , actVarDependency_()
    , actArrayVarDependency_()
{

  // this is for setting the static field for default units
  if (CS_VariableDeclaration::noUNit_ == NULL)
    {
      std::string empty = "";
      CS_VariableDeclaration::noUNit_ = new CS_String(empty, module,
        startLine, startColumn);
    }

  isVariableConsumedBeforeProd_ = true; // false would mean we do not need to save this
  size_ = 1;
  offset_ = 0;
  isArray_ = false;
  // genvars must be INTEGERS
  VERILOG_ERROR_COND_STOP( isGenerateVariable_ && type_ != INTEGER ,
      "Genvars must be an integer ", this);
  // evaluate the length of the range
  if (range)
    {
      isArray_ = true;
      int leftV = verilog_nearestInt( range_->getLeftExpr()->evalConst());
      int rightV = verilog_nearestInt( range_->getRightExpr()->evalConst());
      // swap values if left is grater
      if ( leftV > rightV ){
          isReverseNumbered_ = true;
          int tmpVal = leftV; leftV = rightV; rightV = tmpVal;
      }
      // left index can not be greater
      VERILOG_ERROR_COND_STOP(leftV > rightV, " In index range of the variable '" << name_
          << "' left index can not be greater than the right limit", this);
      size_ = rightV - leftV + 1; // set this value correctly
      VERILOG_ERROR_COND_STOP(size_ <= 0, " In index range of the variable '" << name_
          << "' size of the variable must be at least one but it is " << size_ , this);
      offset_ = leftV;
      // create the array of dependency with the given length
      actArrayVarDependency_.resize(size_);
    }
}


bool CS_VariableDeclaration::isVariableOutput(CS_ExpressionBasis **desc,
    CS_ExpressionBasis **unit, int &varSize) const
{
  bool isOut = false;
  *desc = getAttributeMap().getAttribute(std::string("desc"));
  *unit = getAttributeMap().getAttribute(std::string("units"));

  // if only "desc" is present and no "units" then return empty string
  // as defult for units
  if ( (*desc != 0) && (*unit == 0) )
    *unit = CS_VariableDeclaration::noUNit_;

  // only "desc" has to be not null and the variable must be a scalar
  if ( (*desc != 0) /* && (*unit != 0)*/ /*&& (!this->isArray())*/ )
    isOut = true;
  varSize = this->size_;
  // strings or string arrays cannot be output variables at the moment
  isOut = isOut && (STRING != this->getType() && STRING_ARRAY != this->getType());
  // return the flag
  return isOut;
}
