/*
 * CS_AnalogSmallSignalFunction.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#include "CS_AnalogSmallSignalFunction.hpp"
#include "CS_Number.hpp"

CS_AnalogSmallSignalFunction::CS_AnalogSmallSignalFunction(
    AnalysisFunctionType fT, CS_ExpressionBasis** args, int nrArgs,
          const CS_ModuleDeclaration* module, int startLine, int startColumn):
CS_FunctionCall(args, nrArgs, module, startLine, startColumn, true), funcType_(fT)
{
  // this function needs at least one argument
  VERILOG_ERROR_COND_STOP( !this->getArg(0), "This function needs one (non-default) argument", this);
}


int CS_AnalogSmallSignalFunction::getClasses() const
{
  // these functions usually return zero the analysis maybe not ...
  switch (funcType_){
    case VERILOG_ANALYSFUNC_AC_STIM :
    case VERILOG_ANALYSFUNC_FLICKER_NOISE :
    case VERILOG_ANALYSFUNC_NOISE_TABLE :
    case VERILOG_ANALYSFUNC_WHITE_NOISE : { return (int)REAL; break; }
  }
  return (int)REAL;
}

int CS_AnalogSmallSignalFunction::getTypes() const
{
  // some function have system unknowns (in AK) some not
  switch (funcType_){
    case VERILOG_ANALYSFUNC_AC_STIM :
    case VERILOG_ANALYSFUNC_FLICKER_NOISE :
    case VERILOG_ANALYSFUNC_NOISE_TABLE :
    case VERILOG_ANALYSFUNC_WHITE_NOISE : { return (int)INTERNALVAR; break; }
  }
  return (int)INTERNALVAR;
}

CS_AKDependency CS_AnalogSmallSignalFunction::propagateDependencies()
{
  CS_AKDependency ret_set;
  // add the unknown only when there is any, for instance AC_STIM does not have
  if (this->getAkSystemVarIndex() >= 0)
    ret_set.addSystemVarIndex( this->getAkSystemVarIndex(), this->getDependencyType() );
  // store the combined dependency in the actual node
  setActVarDependency(ret_set);
  // return the dependencies
  return ret_set;
}


CS_ExpressionBasis* CS_AnalogSmallSignalFunction::doDifferentiate(int sysVarIndex, [[maybe_unused]] int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  // the derivative with respect to this system variable is simple to compute, since these
  // function calls represent a function variable in itself
  if ( this->getAkSystemVarIndex() == sysVarIndex)
    return (new CS_Number(1.0, getModule(), getStartLine(), getStartColumn()));
  else
    return (new CS_Number(0.0, getModule(), getStartLine(), getStartColumn()));
}
