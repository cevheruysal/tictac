/*
 * CS_AnalogFunctionCall.hpp
 *
 *  Created on: Feb 19, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGFUNCTIONCALL_HPP_
#define CS_ANALOGFUNCTIONCALL_HPP_

#include "CS_AnalogFunctionDeclaration.hpp"
#include "CS_ExpressionBasis.hpp"
#include "verilog_comp.hpp"

/** class that represents a function call within Verilog */
class CS_AnalogFunctionCall: public CS_ExpressionBasis
{
public:

  /** Dtor to create on call instance of a function */
  CS_AnalogFunctionCall(CS_AnalogFunctionDeclaration* funcDeclr,
      std::vector<CS_ExpressionBasis*>& arguments, int nrArgs,
      const CS_ModuleDeclaration* module, int startLine, int startColumn,
      int derivIndex = -1, bool isExpanded = false);

  /** empty Ctor */
  virtual ~CS_AnalogFunctionCall() {;}

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const;

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const;

  /** some expressions need to be evaluated during compilation, and for this
   * purpose we defined this function, that by default returns an error.
   * The resulting double value might be cased to integer or similar */
  virtual double evalConst() const {
    VERILOG_PRINT_L2( verb(), "Functions can not be evaluated in a constant context " );
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0.0;
  }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // here we do not copy the arguments
    CS_AnalogFunctionCall *ret = new CS_AnalogFunctionCall(
        funcDeclr_, arguments_, nrArgs_, this->getModule(),
        this->getStartLine(), this->getStartColumn(), this->derivIndex_, this->isExpanded_);
    // set the variable also!!!
    ret->setFunctionInstanceVariable(this->variable_);
    return ret;
  }

  /** this function is specific to propagate the system variable dependencies
   * (system variables of the AK generate) across expressions*/
  virtual CS_AKDependency propagateDependencies();

  /** virtual function for the automated differentiation (AD) with respect
   * to the system variable with index sysVarIndex.
   * The function also takes a second argument flag, which is currently not used.
   * This is a recursive function that calls the whole tree of expression */
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  /** inherited from the super class */
   virtual void Accept(CS_CompilerVisitorBasis &visitor) {
     visitor.Visit( (CS_AnalogFunctionCall&)(*this) );
   }

   /** returns the function declaration */
   CS_AnalogFunctionDeclaration* getFunctionDeclr() { return funcDeclr_; }
   const CS_AnalogFunctionDeclaration* getFunctionDeclr() const { return funcDeclr_; }

   /** returns the argument instance */
   CS_ExpressionBasis* getArgument(int i) { return arguments_[i]; }
   const CS_ExpressionBasis* getArgument(int i) const { return arguments_[i]; }

   /** true if function is already expanded */
   bool isFunctionExpanded() { return isExpanded_; }

   /** sets */
   void setFunctionExpanded(bool b) { isExpanded_ = b; }

   /** sets the "instance" variable that represents the returned value of the function
    * This must be instance specific */
   void setFunctionInstanceVariable (CS_VariableDeclaration* variable) {
     variable_ = variable;
   }

   /** @returns the number of arguements */
   int getNrArgs() const { return nrArgs_; }

   /** @returns since the functions gets expanded each funciton value at the
    * end will be a variable, and this functions returns this variable */
   CS_VariableDeclaration* getFuncVariable() { return variable_; }
   const CS_VariableDeclaration* getFuncVariable() const { return variable_; }

   /** @returns the derivative index (if this is in a derivative computation )*/
   int getDerivIndex() const { return derivIndex_; }

private:

  /** pointer to the declaration of the function */
  CS_AnalogFunctionDeclaration* funcDeclr_;

  /** the vector of arguments */
  std::vector<CS_ExpressionBasis*> arguments_;

  /** variable that represents the functions value */
  CS_VariableDeclaration* variable_;

  /** number of arguments */
  int nrArgs_;

  /** if this appears in a derivative expression */
  int derivIndex_;

  /** flag to signal if this function has already been expanded */
  bool isExpanded_;
};

#endif /* CS_ANALOGFUNCTIONCALL_HPP_ */
