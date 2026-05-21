/*
 * IP_ExprConstEvaluator.hpp
 *
 *  Created on: Feb 24, 2015
 *      Author: benk
 */

#ifndef IP_EXPRCONSTEVALUATOR_HPP_
#define IP_EXPRCONSTEVALUATOR_HPP_

#include "sim_utils.hpp"
#include "CS_CompilerVisitorBasis.hpp"


/** the type of the param value
 * Interface for the toplevel expansion.
 * This must contain concrete values */
typedef struct {
  VarType  valueType_; ///< type of the parameter value
  MYREAL dval_; ///< double value
  std::string sval_; ///< string value
  std::vector< MYREAL > aval_; ///< array value like for PWLs
  std::vector< std::string > saval_; ///< string array value
} ConstExpressionValue;

/// forwards declaration
class ExpansionStack;
class CS_ErrorInfo;


/** this class should be able to evaluate const evaluable expressions in
 * a netlist expansion context */
class IP_ExprConstEvaluator : public CS_CompilerVisitorBasis
{
public:

  /** empty Ctor
   * @param is the depth of the evaluator */
  IP_ExprConstEvaluator(MYINT depth=0, bool ignoreActualLevel = false);

  /** empty Dtor */
  virtual ~IP_ExprConstEvaluator() {;}

  /** method to */
  ConstExpressionValue evalConstExpression(CS_ExpressionBasis *expr, const ExpansionStack* stack);

  virtual void Visit(CS_ExpressionBasis &node);
  virtual void Visit(CS_FunctionCall &node);
  virtual void Visit(CS_AnalogConcatenation &node);
  virtual void Visit(CS_BranchProbeFunction &node);
  virtual void Visit(CS_AnalogFilterOperationFunction &node);
  virtual void Visit(CS_BinaryOperator &node);
  virtual void Visit(CS_Number &node);
  virtual void Visit(CS_String &node);
  virtual void Visit(CS_Infinity &node);
  virtual void Visit(CS_ParameterReference &node);
  virtual void Visit(CS_AnalogPortReference &node);
  virtual void Visit(CS_AnalogBuiltInMathFunction &node);
  virtual void Visit(CS_AnalogConditionalExpression &node);
  virtual void Visit(CS_EnvironmentParameterFunction &node);
  virtual void Visit(CS_UnaryOperator &node);
  virtual void Visit(CS_FunctionArgument &node);
  virtual void Visit(CS_AnalogFunctionCall &node);
  virtual void Visit(CS_HierarchSysParamFunctions &node);

  bool hasError() const { return hasError_; }
  const std::string& getErrorMessage() const { return errorMessage_; }
  const CS_ErrorInfo* getFirstErrorNode() const { return errorInf_; }

private:

  /** this field stores the intermediate */
  ConstExpressionValue ret_;

  /** the depth of the stack for parameter resolution, in case of (indirect) recursive
   * dependance this number "explodes", therefore if we limit this than we might find out
   * such cases */
  MYINT idDepthResolution_;

  /** the actual expansion stack */
  const ExpansionStack* expansionStack_;

  /** the actual temperature */
  MYREAL temp_;
  /** the actual vt */
  MYREAL vt_;
  /** the actual abstime (probably zero, at the beginning) */
  MYREAL abstime_;

  /// if by parsing the actual level should be ignored
  bool ignoreActualLevel_;

  /** internal method to report error */
  void reportError(bool cond, std::string message, const CS_ErrorInfo *errorInf = NULL) ;
  /** flag signals if there was an error */
  bool hasError_;
  /** the accumulated error message */
  std::string errorMessage_;
  /** place of the last error */
  const CS_ErrorInfo *errorInf_;

};

#endif /* IP_EXPRCONSTEVALUATOR_HPP_ */
