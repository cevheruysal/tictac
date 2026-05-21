/*
 * CS_AnalogFunctionCall.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: benk
 */

#include "CS_AnalogFunctionCall.hpp"
#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_VariableReference.hpp"

CS_AnalogFunctionCall::CS_AnalogFunctionCall(
    CS_AnalogFunctionDeclaration* funcDeclr,
    std::vector<CS_ExpressionBasis*>& arguments, int nrArgs,
    const CS_ModuleDeclaration* module,
    int startLine, int startColumn,
    int derivIndex , bool isExpanded )
: CS_ExpressionBasis(module, startLine, startColumn),
  funcDeclr_(funcDeclr), arguments_(arguments),  variable_(NULL), nrArgs_(nrArgs),
  derivIndex_(derivIndex), isExpanded_(isExpanded)
{
  for (int j = 0; j < this->nrArgs_ ; j++ )
    {
      if (funcDeclr_->getArgumentType(j) == VF_FUNCVAR_INOUT)
        {
          // in such cases the argument must be a variable itself
          CS_VariableReference *tmp = dynamic_cast<CS_VariableReference*>(arguments_[j]);
          VERILOG_ERROR_COND_STOP( tmp == 0, "INOUT arguments of functions must be variables " , this );
        }
    }
}

/** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
int CS_AnalogFunctionCall::getClasses() const
{
  int i, classOut = 0;
  for (i = 0; i < nrArgs_; i++)
    {
      classOut = classOut | arguments_[i]->getClasses();
    }
  return classOut;
}

/** returns the types (actual INTEGER or REAL)*/
int CS_AnalogFunctionCall::getTypes() const
{
  return (int)funcDeclr_->getFunctionType();
}


CS_AKDependency CS_AnalogFunctionCall::propagateDependencies( ) {
  // add all
  CS_AKDependency ret;
   int j = 0;
   for (j = 0; j < this->nrArgs_ ; j++ )
     {
       // only if the input is not INTEGER
       if (arguments_[j]->getTypes() != INTEGER)
         {
           CS_AKDependency tmp = arguments_[j]->propagateDependencies();
           ret.uniteDependencies( tmp );
         }
     }
   for (j = 0; j < this->nrArgs_ ; j++ )
     {
       if (  (funcDeclr_->getArgumentType(j) == VF_FUNCVAR_INOUT)
           ||(funcDeclr_->getArgumentType(j) == VF_FUNCVAR_OUT)  )
         {
           // in such cases the argument must be a variable itself
           CS_VariableReference *tmp = dynamic_cast<CS_VariableReference*>(arguments_[j]);
           VERILOG_ERROR_COND_STOP( tmp == 0, "INOUT arguments of function must be variables " , this );
           // only if this variable is of real type
           if (arguments_[j]->getTypes() == REAL)
             tmp->getActVarDependency().uniteDependencies(ret);
         }
     }

   if (this->funcDeclr_->getFunctionVar()->getType() != INTEGER)
     { // ONLY when the function is not of integer type
       // store the dependencies
       this->setActVarDependency(ret);
       // add this dependency also to the module variable
       this->funcDeclr_->getFunctionVar()->getVariableScalarDependency().uniteDependencies(ret);
       if (variable_ == NULL) {
           VERILOG_PRINT_L3(verb(),"The auxiliar variable is NULL (2)");
       }
       VERILOG_ERROR_COND_STOP(variable_ == NULL, "Internal Error (function 2)", this);
       variable_->getVariableScalarDependency().uniteDependencies(ret);
     }
   else
     { // if the function is of integer type then no dependencies
       ret = CS_AKDependency();
     }
   // return the combined dependencies
   return ret;
}

CS_ExpressionBasis* CS_AnalogFunctionCall::doDifferentiate(
    int sysVarIndex, [[maybe_unused]] int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  // create a variable expression that will return the derivative with respect to sysVarIndex
  CS_AnalogFunctionCall *retExpr = new CS_AnalogFunctionCall( funcDeclr_ , arguments_,
      nrArgs_, getModule(), getStartLine(), getStartColumn(), sysVarIndex , true );
  // we should also set the variable
  retExpr->setFunctionInstanceVariable(this->variable_);

  // return the created object
  return retExpr;
}
