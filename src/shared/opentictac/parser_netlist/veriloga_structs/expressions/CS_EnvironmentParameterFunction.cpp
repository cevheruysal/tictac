/*
 * CS_EnvironmentParameterFunction.cpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#include "CS_EnvironmentParameterFunction.hpp"

#include "CS_ParameterReference.hpp"

CS_EnvironmentParameterFunction::CS_EnvironmentParameterFunction(
    CS_ExpressionBasis** args, int nrArgs,
    EnvironmentParameterFunctionType ty,
    const CS_ModuleDeclaration* module, int startLine, int startColumn) :
CS_FunctionCall( args, nrArgs, module, startLine, startColumn, false), functionType_(ty)
{
  for (int i = 0 ; i < this->getNrArg() ; i++)
    VERILOG_ERROR_COND_STOP( !this->getArg(i), "This function can not have default arguments", this);

  // TODO: do some checks
  switch (functionType_){
   case VERILOG_ENVIRONFUNC_TEMPERATURE:
     { // "temperature" ;
       VERILOG_ERROR_COND_STOP( this->getNrArg() > 1, "TEMPERATURE function takes at most one argument" , this);
       break;
     }
   case VERILOG_ENVIRONFUNC_VT :
     { //"vt" ;
       break;
     }
   case VERILOG_ENVIRONFUNC_SIMPARAM :
     { //"simparam" ;
       break;
     }
   case VERILOG_ENVIRONFUNC_PARAMGIVEN:
     { //"paramgiven" ;
       const CS_ParameterReference *tmp = dynamic_cast<const CS_ParameterReference*>(this->getArg(0));
       VERILOG_ERROR_COND_STOP( tmp == 0, "PARAMGIVEN function takes only one parameter as argument" , this);
       VERILOG_ERROR_COND_STOP( this->getNrArg() > 1, "PARAMGIVEN function takes at most one argument" , this);
       break;
     }
   case VERILOG_ENVIRONFUNC_ABSTIME:
     {
       // $abstime
       VERILOG_ERROR_COND_STOP( this->getNrArg() > 0, "ABSTIME function take no arguments" , this);
       break;
     }
   case VERILOG_ENVIRONFUNC_SIMPARAM_STR:
   case VERILOG_ENVIRONFUNC_PORTCONNECTED:
   default :
     { // these functions are actually not supported
       VERILOG_ERROR_STOP("Function at the time not supported", this);
       break;
     }
   }
}


int CS_EnvironmentParameterFunction::getClasses() const{
  // if there is a returned function it is a constant (no derivatives needed)
  return (int)PARAMETER;
}


int CS_EnvironmentParameterFunction::getTypes() const{
  // if there is a returned value then it is real
  return (int)REAL;
}


CS_AKDependency CS_EnvironmentParameterFunction::propagateDependencies()
{
  CS_AKDependency ret_tmp;
  // store it in the actual node
  setActVarDependency(ret_tmp);
  // we return an empty set, no propagation of dependencies
  return ret_tmp;
}
