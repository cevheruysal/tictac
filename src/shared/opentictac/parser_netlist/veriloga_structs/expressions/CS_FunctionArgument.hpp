/*
 * CS_FunctionArgument.hpp
 *
 *  Created on: Feb 21, 2013
 *      Author: benk
 */

#ifndef CS_FUNCTIONARGUMENT_HPP_
#define CS_FUNCTIONARGUMENT_HPP_

#include "CS_ExpressionBasis.hpp"
#include "CS_AnalogFunctionDeclaration.hpp"

/** class that models all the arguments within a function body.
 * It is important that these expressions are replaceable for different calls */
class CS_FunctionArgument : public CS_ExpressionBasis
{
public:

  /** empty Ctor*/
  CS_FunctionArgument(
      std::string name, CS_AnalogFunctionDeclaration *function, int argIndex,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor*/
  virtual ~CS_FunctionArgument() {;}

  /** return the argument */
  CS_ExpressionBasis* getArgument() { return argumentExpr_; }
  const CS_ExpressionBasis* getArgument() const { return argumentExpr_; }

  /** set the argument */
  void setArgument(CS_ExpressionBasis* argumentExpr) { argumentExpr_ = argumentExpr; }

   /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
   virtual int getClasses() const { return argumentExpr_->getClasses(); }

   /** returns the types (actual INTEGER or REAL)*/
   virtual int getTypes() const { return argumentExpr_->getTypes(); }

   /** constant evaluation */
   virtual double evalConst() const { return argumentExpr_->evalConst(); }

    /** this function is specific to propagate the system variable dependencies
     * (system variables of the AK generate) across expressions*/
    virtual CS_AKDependency propagateDependencies( ) { return argumentExpr_->propagateDependencies();    }

    /** virtual function for the automated differentiation (AD) with respect
     * to the system variable with index sysVarIndex.
     * The function also takes a second argument flag, which is currently not used.
     * This is a recursive function that calls the whole tree of expression */
    virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag)
    {
      //FIXME: ....
      return argumentExpr_->doDifferentiate(sysVarIndex,flag);
    }

    /** inherited from the super class */
    virtual void Accept(CS_CompilerVisitorBasis &visitor) {
      //VERILOG_ERROR_STOP(" Accept method of CS_FunctionArgument should be overwritten ", this);
      // no error to be thrown, since the visitor mechanism
      argumentExpr_->Accept(visitor);
    }

    /** returns the name of the argument */
    const std::string& getName() const { return name_; }

    /** returns the argument index */
    int getArgIndex() const { return argIndex_; }

    const CS_AnalogFunctionDeclaration* getFunctionDeclr() const { return functionDecr_; }

    /** function that creates a deep copy of the expression itself */
    virtual CS_ExpressionBasis* copy()
    {
      // we only copy this object not the containing expression
      //VERILOG_PRINT_L2( 6 , " Copy Arg" );
      return argumentExpr_->copy();
    }
private:

    /** pointer to the function declaration*/
    CS_AnalogFunctionDeclaration *functionDecr_;

    /** name of the argument */
    std::string name_;

    /** index of the argument */
    int argIndex_;

    /** */
    CS_ExpressionBasis* argumentExpr_;

};

#endif /* CS_FUNCTIONARGUMENT_HPP_ */
