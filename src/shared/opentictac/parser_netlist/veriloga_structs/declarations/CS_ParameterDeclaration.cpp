/*
 * CS_ParameterDeclaration.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#include "CS_ParameterDeclaration.hpp"
#include "CS_Parameter.hpp"

CS_ParameterDeclaration::CS_ParameterDeclaration( CS_Parameter* param,  CS_ExpressionBasis* defValue ):
  param_(param), defValue_(defValue), constraints_(0)
{
  // nothing to do here now
}


const std::string CS_ParameterDeclaration::getName() const {
  return param_->getName();
}


int CS_ParameterDeclaration::getIndex() const {
  return param_->getIndex();
}

