/*
 * CS_AnalogSmallSignalFunction.hpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGSMALLSIGNALFUNCTION_HPP_
#define CS_ANALOGSMALLSIGNALFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** various function types for the analysis */
typedef enum {
    VERILOG_ANALYSFUNC_AC_STIM = 0,
    VERILOG_ANALYSFUNC_FLICKER_NOISE = 1,
    VERILOG_ANALYSFUNC_NOISE_TABLE = 2,
    VERILOG_ANALYSFUNC_WHITE_NOISE = 3
} AnalysisFunctionType;

/** Function class for analysis type function expressions
 *
 * see analog_small_signal_function_call in LRM*/
class CS_AnalogSmallSignalFunction : public CS_FunctionCall
{
public:

  /** Ctor, maximal two function arguments for these calls */
  CS_AnalogSmallSignalFunction(AnalysisFunctionType fT,
      CS_ExpressionBasis** args, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_AnalogSmallSignalFunction() {;}

  virtual int getClasses() const;

  virtual int getTypes() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalogSmallSignalFunction &)(*this)); }

  /** returns the type of the function */
  AnalysisFunctionType getFunctionType() const { return funcType_; }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // copy the arguments
    CS_ExpressionBasis *arg[10];
    for (int i = 0; i < this->getNrArg(); i++)
      if (getArg(i))
        arg[i] = this->getArg(i)->copy();
      else
        arg[i] = 0;

    // create new object
    return (new CS_AnalogSmallSignalFunction(funcType_, arg,
        this->getNrArg(), this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

private:

  /** the type of the function */
  AnalysisFunctionType funcType_;

};

#endif /* CS_ANALOGSMALLSIGNALFUNCTION_HPP_ */
