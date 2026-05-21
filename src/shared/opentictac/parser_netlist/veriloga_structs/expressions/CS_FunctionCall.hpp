/*
 * CS_FunctionCall.hpp
 *
 *  Created on: Dec 14, 2012
 *      Author: benk
 */

#ifndef CS_FUNCTIONCALL_HPP_
#define CS_FUNCTIONCALL_HPP_

#include "CS_ExpressionBasis.hpp"

/** abstract class to model the function calls and to store the arguments
 *
 * this class has no LRM correspondence it should only facilitate the efficient
 * data representation of function calls and its arguments  */
class CS_FunctionCall : public CS_ExpressionBasis
{
public:

  /** Ctor with zero arguments */
  CS_FunctionCall( CS_ExpressionBasis** arg1, int nrArg,
      const CS_ModuleDeclaration* module, int startLine, int startColumn,
      bool canHaveDefaultParams);

  virtual ~CS_FunctionCall() {;}

  /** returns the number of arguments */
  int getNrArg() const { return nrArgs_; }

  /** returns the number of arguments of the function */
  CS_ExpressionBasis* getArg(int i) { return (i < nrArgs_) ? args_[i] : 0; }
  const CS_ExpressionBasis* getArg(int i) const { return (i < nrArgs_) ? args_[i] : 0; }

  /** with this function one can also replace one function argument.
   * This function makes upper range check. */
  void setArg(int i, CS_ExpressionBasis* e) {
    if (i < nrArgs_)
      args_[i] = e;
  }

  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]]  int *flag) {
    VERILOG_PRINT_L3(verb()," doDifferentiate function not implemented for CS_FunctionCall");
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0;
  }

protected:

  /** function adds a new argument to the end of the argument list, number of
   * arguments is also incremented
   * @param newArg the new argument at the end of the arguemnt list */
  void addNewArgument(CS_ExpressionBasis* newArg) {
    args_.push_back(newArg);
    nrArgs_++;
  }

  /** number of arguments of the function */
  int nrArgs_;

  /** pointers to the arguments */
   std::vector<CS_ExpressionBasis*> args_;
};

#endif /* CS_FUNCTIONCALL_HPP_ */
