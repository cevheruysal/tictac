/*
 * CS_StringExpression.hpp
 *
 *  Created on: Apr 22, 2013
 *      Author: benk
 */

#ifndef CS_STRINGEXPRESSION_HPP_
#define CS_STRINGEXPRESSION_HPP_

#include "CS_FunctionCall.hpp"

/** enum types of the string function */
typedef enum {
    VERILOG_STRING_LEN = 0,    /* strlen function*/
    VERILOG_STRING_SUBSTR = 1, /* substr function*/
    VERILOG_STRING_REPLIC = 2  /* string replicator operator */
} StringFunctionType;

/** class to model all string functions/operations
 *
 * this class has no LRM correspondence, furthermore this feature is not even
 * in the LRM, but only in the System-Verilog, but seems to be used in-house,
 * therefore we offer this feature
 *
 * Example:
 *  string var;
 *  var = "Hallo";
 *  var.len();
 *  var.substr(2,3);
 *  var = { 3{"001"} }; //string replication operator */
class CS_StringFunction : public CS_FunctionCall
{
public:

    /** Ctor with the target variable */
    CS_StringFunction(
      CS_VariableReference *targetVariable,
      CS_ExpressionBasis *targetExpression,
      StringFunctionType stringExprType,
      CS_ExpressionBasis** args, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

    /** empty Dtor */
    virtual ~CS_StringFunction() {;}

    /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
    virtual int getClasses() const;

    /** returns the types (actual INTEGER or REAL)*/
    virtual int getTypes() const;

    /** some expressions need to be evaluated during compilation, and for this
     * purpose we defined this function, that by default returns an error.
     * The resulting double value might be cased to integer or similar */
    virtual double evalConst() const {
      VERILOG_PRINT_L3(verb()," evalConst of CS_StringFunction is not implemented ")
      VERILOG_ERROR_STOP("Internal Error", this);
      return 0.0;
    }

    /** this function is specific to propagate the system variable dependencies
     * (system variables of the AK generate) across expressions*/
    virtual CS_AKDependency propagateDependencies( ) {
      // no dependency propagation
      return CS_AKDependency();
    }

    /** function that creates a deep copy of the expression itself */
    virtual CS_ExpressionBasis* copy() {
      // copy the arguments
      CS_ExpressionBasis *arg[2];
      for (int i = 0; i < this->getNrArg(); i++)
        arg[i] = this->getArg(i)->copy();

      // create new object
      return (new CS_StringFunction( 0,
          stringExpression_->copy(), stringExprType_,
          arg, this->getNrArg(), this->getModule(),
          this->getStartLine(), this->getStartColumn()));
    }

    /** inherited from the super class */
    virtual void Accept(CS_CompilerVisitorBasis &visitor) {
      visitor.Visit( (CS_StringFunction &)(*this));
    }

    /** virtual function for the automated differentiation (AD) with respect
     * to the system variable with index sysVarIndex.
     * The function also takes a second argument flag, which is currently not used.
     * This is a recursive function that calls the whole tree of expression */
  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
    VERILOG_PRINT_L3(verb()," doDifferentiate function not implemented for CS_FunctionCall ");
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0;
  }

  /** returns the string expression  */
  CS_ExpressionBasis* getStringExpression() { return stringExpression_; }
  const CS_ExpressionBasis* getStringExpression() const { return stringExpression_; }

  /** returns the string function type */
  StringFunctionType getFunctionType() { return stringExprType_; }

private:

  /** the string expression on which this function is applied to  */
  CS_ExpressionBasis *stringExpression_;

  /** type of the expression */
  StringFunctionType stringExprType_;

};

#endif /* CS_STRINGEXPRESSION_HPP_ */
