/*
 * CS_AnalogConcatenation.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: benk
 */

#include "CS_AnalogConcatenation.hpp"

CS_AnalogConcatenation::CS_AnalogConcatenation(const CS_ModuleDeclaration* module, int startLine,
    int startColumn): CS_ExpressionBasis(module,startLine,startColumn), exprs_(), setLength_(0)
{
  // nothing to do yet
}


CS_AKDependency CS_AnalogConcatenation::propagateDependencies()
{
  CS_AKDependency ret_tmp;

  // add each item of the concatenation
  for (int i = 0; i < setLength_; i++)
    {
      CS_AKDependency ret_tmp1 = exprs_[i]->propagateDependencies();
      ret_tmp.uniteDependencies( ret_tmp1 );
    }

  // store it in the actual node
  setActVarDependency(ret_tmp);
  // we return an empty set, no propagation of dependencies
  return ret_tmp;
}


int CS_AnalogConcatenation::getTypes() const
{
  int tmpV = 0;
  for (int i = 0; i < setLength_ ; i++) { tmpV = tmpV | exprs_[i]->getTypes(); }
  // if this is a real concat/array, then the returned type is an array
  if (setLength_ >= 1)
    {
      //std::cout << "type: " << tmpV << "\n";
      if ( tmpV <= INTEGER) return INTEGER_ARRAY;
      else {
          if ( tmpV <= REAL) return REAL_ARRAY;
          else               return STRING;
      }
    }
  else
    return tmpV;
}
