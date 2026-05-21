/*
 * CS_AnalysisFunction.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: benk
 */

#include "CS_AnalysisFunction.hpp"
#include "CS_String.hpp"
#include <algorithm>

CS_AnalysisFunction::CS_AnalysisFunction(
    CS_ExpressionBasis** args, int nrArgs,
    const CS_ModuleDeclaration* module, int startLine, int startColumn):
CS_FunctionCall(args, nrArgs, module, startLine, startColumn, false)
{
  // arguments should be a string,
  for (int i = 0 ; i < this->getNrArg() ; i++ )
    {
      VERILOG_ERROR_COND_STOP( (!getArg(i)) || getArg(i)->getTypes() != STRING,
           "Argument " << i<<" of analysis must be a string ", this);

      const CS_String* str = dynamic_cast<const CS_String*>( getArg(i));

      VERILOG_ERROR_COND_STOP( str == NULL,  "Argument " << i<<" of analysis must be a string ", this);
      std::string strVal = str->getStringValue();

      std::transform( strVal.begin(), strVal.end(),strVal.begin(), ::tolower);
      if (    (strVal != "ac") && (strVal != "dc") && (strVal != "noise")
           && (strVal != "tran") && (strVal != "ic") && (strVal != "static")
          && (strVal != "nodeset"))
      VERILOG_ERROR_STOP( "Arguments of the analysis function should be"
            << "only ac, dc, noise, tran, ic, static, or nodeset  but it is '"
            << strVal <<"'", this);
    }
}

int CS_AnalysisFunction::getClasses() const
{
  // these functions usually return zero
  return (int)INTEGER;
}

int CS_AnalysisFunction::getTypes() const
{
  // some functions might depend from the input parameters
  return (int)CONSTANT;
}

CS_AKDependency CS_AnalysisFunction::propagateDependencies(){
  // generate an empty set
  CS_AKDependency ret_set;
  // store it in the actual node
  setActVarDependency(ret_set);
  // return the combined dependency list
  return ret_set;
}
