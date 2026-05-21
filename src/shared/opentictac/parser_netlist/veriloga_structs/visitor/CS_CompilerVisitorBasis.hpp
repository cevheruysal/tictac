/*
 * CS_CompilerVisitorBasis.hpp
 *
 *  Created on: Dec 31, 2012
 *      Author: benk
 */

#ifndef CS_COMPILERVISITORBASIS_HPP_
#define CS_COMPILERVISITORBASIS_HPP_

#include "verilog_comp.hpp"

class CS_ModuleDeclaration;

class CS_CompilerTreeNode;

class CS_ParameterDeclaration;
class CS_ValueRange;
class CS_VariableDeclaration;
class CS_Port;
class CS_Range;
class CS_BranchDeclaration;
class CS_AnalogFunctionDeclaration;
class CS_ModuleInstantiation;

// ---------- instruction visitors -------------------
// first the abstract classes
class CS_AnalogStatement;
class CS_AnalogSeqBlock;
class CS_AnalogConditionalStatement;
class CS_AnalogEventControlStatement;
class CS_ContributionStatement;
class CS_AnalogVariableAssignment;
class CS_AnalogCaseStatement;
class CS_AnalogDispOrSysFunc;
class CS_AnalogLoopStatement;
class CS_IndirectContributionStatement;
class CS_AnalogLoopGenerateStatement;
class CS_AnalogInitialBlock;

// --------- expressions visitors --------------------
class CS_ExpressionBasis;
class CS_FunctionCall;
class CS_AnalogConcatenation;
class CS_AnalogEventFunction;
class CS_BranchProbeFunction;
class CS_AnalogFilterOperationFunction;
class CS_AnalogSmallSignalFunction;
class CS_AnalysisFunction;
class CS_BinaryOperator;
class CS_Number;
class CS_String;
class CS_Infinity;
class CS_IndexedIdentifier;
class CS_ParameterReference;
class CS_VariableReference;
class CS_AnalogPortReference;
class CS_AnalogBuiltInMathFunction;
class CS_AnalogConditionalExpression;
class CS_SystemFunction;
class CS_RandomFunction;
class CS_EnvironmentParameterFunction;
class CS_UnaryOperator;
class CS_FunctionArgument;
class CS_AnalogFunctionCall;
class CS_HierarchSysParamFunctions;
class CS_StringFunction;
class CS_Identifier;

/** the basis class for "our" visitor that visits our own data structure (not Verific data structures).
 * This class will visit all the instructions and expressions.
 *
 * The mechanism is similar from the Verific (we do not want to have two completely different concepts in one code)
 * A visitor class should overwrite the "Visit" methods where class has access to the information of one node.
 * If the visitor class wants to proceed with the child nodes of the "Visit" argument than it has to call the "Accept(node)" method!.
 * This "Accept" methods assures then for the further nodes of this class the visitor's methods are called.
 * "Accept" visits ONLY the children of the actual node.
 * Therefore there methods should not be overwritten.
 *
 * The combination of the "Visit" and "Accept" is an indirect recursion mechanism to traverse the tree.
 * In this case the tree is one first level made of instructions, and on the second level is made of expressions*/
class CS_CompilerVisitorBasis : public UT_VerboseClass
{
public:

  /** empty Ctor */
  CS_CompilerVisitorBasis();

  /** empty Dtor */
  virtual ~CS_CompilerVisitorBasis() {;}

  virtual void startVisit(CS_ModuleDeclaration* moduleToVisit);

// ---------- declaration visitors -------------------
  virtual void Visit(CS_ParameterDeclaration &node);
  virtual void Visit(CS_ValueRange &node);
  virtual void Visit(CS_VariableDeclaration &node);
  virtual void Visit(CS_Port &node);
  virtual void Visit(CS_Range &node);
  virtual void Visit(CS_BranchDeclaration &node);
  virtual void Visit(CS_AnalogFunctionDeclaration &node);
  virtual void Visit(CS_ModuleInstantiation &node);
// ---------- instruction visitors -------------------
  virtual void Visit(CS_AnalogStatement &node);
  virtual void Visit(CS_AnalogSeqBlock &node);
  virtual void Visit(CS_AnalogConditionalStatement &node);
  virtual void Visit(CS_AnalogEventControlStatement &node);
  virtual void Visit(CS_ContributionStatement &node);
  virtual void Visit(CS_AnalogVariableAssignment &node);
  virtual void Visit(CS_AnalogCaseStatement &node);
  virtual void Visit(CS_AnalogDispOrSysFunc &node);
  virtual void Visit(CS_AnalogLoopStatement &node);
  virtual void Visit(CS_IndirectContributionStatement &node);
  virtual void Visit(CS_AnalogLoopGenerateStatement &node);
  virtual void Visit(CS_AnalogInitialBlock &node);
// --------- expressions visitors --------------------
  virtual void Visit(CS_ExpressionBasis &node);
  virtual void Visit(CS_FunctionCall &node);
  virtual void Visit(CS_AnalogConcatenation &node);
  virtual void Visit(CS_AnalogEventFunction &node);
  virtual void Visit(CS_BranchProbeFunction &node);
  virtual void Visit(CS_AnalogFilterOperationFunction &node);
  virtual void Visit(CS_AnalogSmallSignalFunction &node);
  virtual void Visit(CS_AnalysisFunction &node);
  virtual void Visit(CS_BinaryOperator &node);
  virtual void Visit(CS_Number &node);
  virtual void Visit(CS_String &node);
  virtual void Visit(CS_Infinity &node);
  virtual void Visit(CS_IndexedIdentifier &node);
  virtual void Visit(CS_ParameterReference &node);
  virtual void Visit(CS_VariableReference &node);
  virtual void Visit(CS_AnalogPortReference &node);
  virtual void Visit(CS_AnalogBuiltInMathFunction &node);
  virtual void Visit(CS_AnalogConditionalExpression &node);
  virtual void Visit(CS_SystemFunction &node);
  virtual void Visit(CS_RandomFunction &node);
  virtual void Visit(CS_EnvironmentParameterFunction &node);
  virtual void Visit(CS_UnaryOperator &node);
  virtual void Visit(CS_FunctionArgument &node);
  virtual void Visit(CS_AnalogFunctionCall &node);
  virtual void Visit(CS_HierarchSysParamFunctions &node);
  virtual void Visit(CS_StringFunction &node);

/* --------NON VIRTUAL METHODS TO VISIT THE CHILDREN OF ONE NODE --------*/

  void VisitChildren(CS_ParameterDeclaration &node);
  void VisitChildren(CS_ValueRange &node);
  void VisitChildren(CS_VariableDeclaration &node);
  void VisitChildren(CS_Port &node);
  void VisitChildren(CS_Range &node);
  void VisitChildren(CS_BranchDeclaration &node);
  void VisitChildren(CS_AnalogFunctionDeclaration &node);
  void VisitChildren(CS_ModuleInstantiation &node);
// ---------- instruction visitors -------------------
  void VisitChildren(CS_AnalogStatement &node);
  void VisitChildren(CS_AnalogSeqBlock &node);
  void VisitChildren(CS_AnalogConditionalStatement &node);
  void VisitChildren(CS_AnalogEventControlStatement &node);
  void VisitChildren(CS_ContributionStatement &node);
  void VisitChildren(CS_AnalogVariableAssignment &node);
  void VisitChildren(CS_AnalogCaseStatement &node);
  void VisitChildren(CS_AnalogDispOrSysFunc &node);
  void VisitChildren(CS_AnalogLoopStatement &node);
  void VisitChildren(CS_IndirectContributionStatement &node);
  void VisitChildren(CS_AnalogLoopGenerateStatement &node);
  void VisitChildren(CS_AnalogInitialBlock &node);
// --------- expressions visitors --------------------
  void VisitChildren(CS_ExpressionBasis &node);
  void VisitChildren(CS_FunctionCall &node);
  void VisitChildren(CS_AnalogConcatenation &node);
  void VisitChildren(CS_AnalogEventFunction &node);
  void VisitChildren(CS_BranchProbeFunction &node);
  void VisitChildren(CS_AnalogFilterOperationFunction &node);
  void VisitChildren(CS_AnalogSmallSignalFunction &node);
  void VisitChildren(CS_AnalysisFunction &node);
  void VisitChildren(CS_BinaryOperator &node);
  void VisitChildren(CS_Number &node);
  void VisitChildren(CS_String &node);
  void VisitChildren(CS_Infinity &node);
  void VisitChildren(CS_IndexedIdentifier &node);
  void VisitChildren(CS_ParameterReference &node);
  void VisitChildren(CS_VariableReference &node);
  void VisitChildren(CS_AnalogPortReference &node);
  void VisitChildren(CS_AnalogBuiltInMathFunction &node);
  void VisitChildren(CS_AnalogConditionalExpression &node);
  void VisitChildren(CS_SystemFunction &node);
  void VisitChildren(CS_RandomFunction &node);
  void VisitChildren(CS_EnvironmentParameterFunction &node);
  void VisitChildren(CS_UnaryOperator &node);
  void VisitChildren(CS_FunctionArgument &node);
  void VisitChildren(CS_AnalogFunctionCall &node);
  void VisitChildren(CS_HierarchSysParamFunctions &node);
  void VisitChildren(CS_StringFunction &node);

protected:

  virtual void visitNode(CS_CompilerTreeNode *node);

  /** the actual Verilog module that we visit */
  CS_ModuleDeclaration* actModule_;

};

#endif /* CS_COMPILERVISITORBASIS_HPP_ */
