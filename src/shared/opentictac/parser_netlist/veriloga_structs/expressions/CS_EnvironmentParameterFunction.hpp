/*
 * CS_EnvironmentParameterFunction.hpp
 *
 *  Created on: Jan 10, 2013
 *      Author: benk
 */

#ifndef CS_ENVIRONMENTPARAMETERFUNCTION_HPP_
#define CS_ENVIRONMENTPARAMETERFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** the various types of the evironment function */
typedef enum {
  VERILOG_ENVIRONFUNC_TEMPERATURE        = 0,   //"temperature" ;
  VERILOG_ENVIRONFUNC_VT                 = 1,   //"vt" ;
  VERILOG_ENVIRONFUNC_SIMPARAM           = 2,   //"simparam" ;
  VERILOG_ENVIRONFUNC_SIMPARAM_STR       = 4,   //"simparam_str" ;
  VERILOG_ENVIRONFUNC_PARAMGIVEN         = 5,   //"$param_given" ;
  VERILOG_ENVIRONFUNC_PORTCONNECTED      = 6,   //"$port_connected" ;
  VERILOG_ENVIRONFUNC_ABSTIME            = 8    // $abstime
} EnvironmentParameterFunctionType;

/** class to model environment function calls
 *
 * see  environment_parameter_functions
 * ,genvar_system_function and simulation time function
 * ($abstime in Table 9-7) in LRM */
class CS_EnvironmentParameterFunction : public CS_FunctionCall
{
public:

  /** empty Ctor */
  CS_EnvironmentParameterFunction(
      CS_ExpressionBasis** args, int nrArgs,
      EnvironmentParameterFunctionType ty,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_EnvironmentParameterFunction() {;}

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const;

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) {
    visitor.Visit( (CS_EnvironmentParameterFunction &)(*this));
  }

  /** returns the type of this environment function */
  EnvironmentParameterFunctionType getFunctionType() const { return functionType_; }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // copy the arguments
    CS_ExpressionBasis *arg[10];
    for (int i = 0; i < this->getNrArg(); i++)
      if (getArg(i))
        arg[i] = this->getArg(i)->copy();
      else
        arg[i] = 0;
    // here we do not copy the arguments
    return (new CS_EnvironmentParameterFunction( arg,
        this->getNrArg(), functionType_,
        this->getModule(), this->getStartLine(), this->getStartColumn()));
  }

  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
    VERILOG_PRINT_L3(verb()," doDifferentiate function not implemented for CS_EnvironmentParameterFunction");
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0;
  }

private:

  /** the function type */
  EnvironmentParameterFunctionType functionType_;
};

#endif /* CS_ENVIRONMENTPARAMETERFUNCTION_HPP_ */
