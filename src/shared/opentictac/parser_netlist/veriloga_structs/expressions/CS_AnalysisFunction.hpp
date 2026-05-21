/*
 * CS_AnalysisFunction.hpp
 *
 *  Created on: Jan 9, 2013
 *      Author: benk
 */

#ifndef CS_ANALYSISFUNCTION_HPP_
#define CS_ANALYSISFUNCTION_HPP_

#include "CS_FunctionCall.hpp"

/** Function class for analysis type function expressions
 *
 * see analysis_function_call in LRM
 * e.g.,  analysis ( " analysis_identifier " { , " analysis_identifier " } ) */
class CS_AnalysisFunction : public CS_FunctionCall
{
public:

  /** Ctor, only one function argument for this call  */
  CS_AnalysisFunction(CS_ExpressionBasis** args, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_AnalysisFunction() {;}

  virtual int getClasses() const;

  virtual int getTypes() const;

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_AnalysisFunction &)(*this)); }

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
    return (new CS_AnalysisFunction( arg,
        this->getNrArg(), this->getModule(),
        this->getStartLine(), this->getStartColumn()));
  }

  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
    VERILOG_PRINT_L2( verb(), " doDifferentiate function not implemented for CS_AnalysisFunction");
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0;
  }

private:

};

#endif /* CS_ANALYSISFUNCTION_HPP_ */
