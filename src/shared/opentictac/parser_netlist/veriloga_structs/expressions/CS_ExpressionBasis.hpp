/*
 * CS_ExpressionBasis.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#ifndef CS_EXPRESSIONBASIS_HPP_
#define CS_EXPRESSIONBASIS_HPP_

#include "verilog_comp.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_GenerateInfo.hpp"
#include "CS_AKDependency.hpp"
#include "CS_ErrorInfo.hpp"
#include <typeinfo>

// forward declaration of the module class
class CS_ModuleDeclaration;

/** basis class for the expressions.
 * It contains all the necessary basic informations and interfaces for
 * expressions. This is the super class for all Verilog-A expressions.
 *
 * see analog_expression (analog_primary) in LRM */
class CS_ExpressionBasis : public CS_CompilerTreeNode, public CS_GenerateInfo,
public CS_ErrorInfo, public UT_VerboseClass
{
public:

  /** Ctor of the expression basis class */
  CS_ExpressionBasis(const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_ExpressionBasis() {;}

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const = 0;

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const = 0;

  /** some expressions need to be evaluated during compilation, and for this
   * purpose we defined this function, that by default returns an error.
   * The resulting double value might be cased to integer or similar */
  virtual double evalConst() const {
    VERILOG_PRINT_L3(verb()," expression is not constant (evalConst function not implemented)");
    VERILOG_ERROR_STOP("Expression is not constant", this);
    return 0.0;
  }

  /** this function is specific to propagate the system variable dependencies
   * (system variables of the AK generate) across expressions*/
  virtual CS_AKDependency propagateDependencies( ) {
    VERILOG_PRINT_L3(verb()," propagateDependencies function not implemented");
    VERILOG_ERROR_STOP("Internal Error", this);
    return CS_AKDependency();
  }

  /** virtual function for the automated differentiation (AD) with respect
   * to the system variable with index sysVarIndex.
   * The function also takes a second argument flag, which is currently not used.
   * This is a recursive function that calls the whole tree of expression */
  virtual CS_ExpressionBasis* doDifferentiate([[maybe_unused]] int sysVarIndex, [[maybe_unused]] int *flag) {
    VERILOG_PRINT_L3(verb()," doDifferentiate function not implemented " << typeid(*this).name());
    VERILOG_ERROR_STOP("Internal Error", this);
    return 0;
  }

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() = 0;

  /** this is a final method, that should only be called for the root node of an expression.
   * It takes from the dependency list all the dependencies w.r.t. to system variables,
   * calls the doDifferentiate method and stores the result */
  void makeDiffentiation();

  /** inherited from the super class */
  virtual void Accept([[maybe_unused]] CS_CompilerVisitorBasis &visitor) {
    VERILOG_PRINT_L3(verb()," Accept method of ExprBasis should be overwritten ");
    VERILOG_ERROR_STOP("Internal Error", this);
  }

  /** returns the local type of the expression */
  VarType getLocalType() const { return actType_; }

  /** set the local type of the expression */
  void setLocalType( VarType vt) { actType_ = vt; }

  /** number of derivatives stored in this node (used usually only for root nodes)*/
  int getNrDerivs() const { return nrDerivs_;}

  /** adds one derivative expression including */
  void addDerivatives(CS_ExpressionBasis* derivExp, int sysIndex)
  {
    derivatives_.push_back(derivExp);
    derivSysVarIndex_.push_back(sysIndex);
    nrDerivs_++;
  }

  /** returns the derivative expression of a given index*/
  CS_ExpressionBasis* getDerivative(int i) { return derivatives_[i];}
  /** returns the derivative system variable index */
  int getDerivativeSysIndex(int i) const { return derivSysVarIndex_[i];}

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

  /** returns the size of the expressions, scalars are 1, arrays usually >= 1*/
  virtual int getExprSize() const { return 1;}

  /** returns the twin expression */
  const CS_ExpressionBasis* getTwin() const { return twin_;}
  CS_ExpressionBasis*       getTwin() { return twin_;}

  /** sets the twin expression */
  void setTwin(CS_ExpressionBasis* t) { twin_ = t; }

  /** returns the twin prefix, default is empty string */
  const std::string& getTwinPrefix() const { return twin_prefix_; }

  /** sets the prefix for the twin */
  void setTwinPrefix(std::string tp) { twin_prefix_ = tp; }

  /** sets the twin pointer to itself such that it points to itself
   * (happens often in expressions for derivation)*/
  void setTwinSelf(std::string tp = "") { twin_ = this; twin_prefix_ = tp;}

  /** returns the twin counter (local counter which is required in one expression )*/
  int getTwinCount() const { return twinCount_; }

  /** sets the twin counter */
  void setTwinCount(int c) { twinCount_ = c; }

private:

   /** start line of the expression */
   const int startLine_;

   /** start column */
   const int startColumn_;

   /** the module that contains this expression */
   const CS_ModuleDeclaration* module_;

   /** the type of this expression */
   VarType actType_;

   /** in this array we store the derivatives of the expression with respect to each
    * system variable.
    * this array will we set only in the root node of the expression */
   std::vector<CS_ExpressionBasis*> derivatives_;

   /** the index of the system variable of the computed derivative */
   std::vector<int> derivSysVarIndex_;

   /** actual number of derivatives */
   int nrDerivs_;

   /** to store intermediate results between derivate and this we store the
    * pointer to the original expression, which in many case can be itself */
   CS_ExpressionBasis* twin_;
   /** if the twin is set we might have optional string which might be used
    * to generate the temporary variables to store these values*/
   std::string         twin_prefix_;
   /** counter of the twin expression, prefix+counter will determine the
    * name of the temporary variable */
   int twinCount_;
};

#endif /* CS_EXPRESSIONBASIS_HPP_ */
