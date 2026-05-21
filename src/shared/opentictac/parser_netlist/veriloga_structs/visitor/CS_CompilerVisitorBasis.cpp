/*
 * CS_CompilerVisitorBasis.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: benk
 */

#include "CS_CompilerVisitorBasis.hpp"

#include "CS_ModuleDeclaration.hpp"

#include "CS_CompilerTreeNode.hpp"

// declarations
#include "CS_ParameterDeclaration.hpp"
#include "CS_ValueRange.hpp"
#include "CS_VariableDeclaration.hpp"
#include "CS_Port.hpp"
#include "CS_Range.hpp"
#include "CS_BranchDeclaration.hpp"

// instructions
#include "CS_AnalogConditionalStatement.hpp"
#include "CS_AnalogSeqBlock.hpp"
#include "CS_AnalogStatement.hpp"
#include "CS_AnalogEventControlStatement.hpp"
#include "CS_ContributionStatement.hpp"
#include "CS_AnalogVariableAssignment.hpp"
#include "CS_ModuleInstantiation.hpp"
#include "CS_AnalogCaseStatement.hpp"
#include "CS_AnalogDispOrSysFunc.hpp"
#include "CS_AnalogLoopStatement.hpp"
#include "CS_IndirectContributionStatement.hpp"
#include "CS_AnalogLoopGenerateStatement.hpp"
#include "CS_AnalogInitialBlock.hpp"

// expressions
#include "CS_ExpressionBasis.hpp"
#include "CS_FunctionCall.hpp"
#include "CS_AnalogEventFunction.hpp"
#include "CS_AnalogFilterOperationFunction.hpp"
#include "CS_AnalogSmallSignalFunction.hpp"
#include "CS_AnalysisFunction.hpp"
#include "CS_BinaryOperator.hpp"
#include "CS_AnalogConditionalExpression.hpp"
#include "CS_AnalogConcatenation.hpp"
#include "CS_Number.hpp"
#include "CS_IndexedIdentifier.hpp"
#include "CS_ParameterReference.hpp"
#include "CS_VariableReference.hpp"
#include "CS_AnalogPortReference.hpp"
#include "CS_AnalogBuiltInMathFunction.hpp"
#include "CS_SystemFunction.hpp"
#include "CS_RandomFunction.hpp"
#include "CS_EnvironmentParameterFunction.hpp"
#include "CS_UnaryOperator.hpp"
#include "CS_String.hpp"
#include "CS_FunctionArgument.hpp"
#include "CS_AnalogFunctionCall.hpp"
#include "CS_HierarchSysParamFunctions.hpp"
#include "CS_StringFunction.hpp"

#include <typeinfo>

CS_CompilerVisitorBasis::CS_CompilerVisitorBasis():
UT_VerboseClass("CompilerVisitorBasis")
{
  actModule_ = 0; // initial no module to traverse
}

void CS_CompilerVisitorBasis::visitNode(CS_CompilerTreeNode *node){
  // visit the node if it not null
  if (node) node->Accept(*this) ;
}


void CS_CompilerVisitorBasis::startVisit(CS_ModuleDeclaration* moduleToVisit) {
  actModule_ = moduleToVisit;
  // first traverse the declarations
  for (int i = 0; i < actModule_->getNrParam() ; i++)
    {
      //VERILOG_PRINT_L3( verb(), "NrP:" << actModule_->getNrParam() << " ,i=" << i);
      visitNode(actModule_->getParamDeclr(i));
    }
  for (int i = 0; i < actModule_->getNrPort() ; i++)
    {
      visitNode(actModule_->getPort(i));
    }
  for (int i = 0; i < actModule_->getNrVars() ; i++)
    {
      visitNode(actModule_->getVar(i));
    }
  for (int i = 0; i < actModule_->getBranchDeclrs() ; i++)
    {
      visitNode(actModule_->getBranchDeclr(i));
    }
  for (int i = 0; i < actModule_->getNrInsts(); i++)
    {
      visitNode(actModule_->getInstance(i));
    }
  // recursive traversal of the analog initial instructions
  visitNode(actModule_->getInitInstrSequence());
  // recursive traversal of the analog instructions
  visitNode(actModule_->getInstrSequence());
}

void CS_CompilerVisitorBasis::Visit(CS_ParameterDeclaration &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_ValueRange &node){ VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_VariableDeclaration &node)   { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_Port &node)       { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_Range &node) { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_BranchDeclaration &node){ VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_AnalogFunctionDeclaration &node) { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_ModuleInstantiation &node) { VisitChildren(node);  }
// ---------- instruction visitors -------------------
void CS_CompilerVisitorBasis::Visit(CS_AnalogStatement &node)             { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogSeqBlock &node)          { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogConditionalStatement &node)      { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogEventControlStatement &node)       { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_ContributionStatement &node){ VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogVariableAssignment &node)      { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogCaseStatement &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogDispOrSysFunc &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogLoopStatement &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_IndirectContributionStatement &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogLoopGenerateStatement &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogInitialBlock &node) { VisitChildren(node); }
// --------- expressions visitors --------------------
void CS_CompilerVisitorBasis::Visit(CS_ExpressionBasis &node)           { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_FunctionCall &node)        { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_AnalogConcatenation &node)          { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_AnalogEventFunction &node)       { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_BranchProbeFunction &node)          { VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_AnalogFilterOperationFunction &node){ VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogSmallSignalFunction &node){ VisitChildren(node);   }
void CS_CompilerVisitorBasis::Visit(CS_AnalysisFunction &node){ VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_BinaryOperator &node)        { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_Number &node)           { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_String &node)          { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_Infinity &node)        { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_IndexedIdentifier &node)  { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_ParameterReference &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_VariableReference &node)  { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogPortReference &node){ VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogBuiltInMathFunction &node)    { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogConditionalExpression &node)      { VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_SystemFunction &node)  { VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_RandomFunction &node)  { VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_EnvironmentParameterFunction &node) {  VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_UnaryOperator &node)         { VisitChildren(node);  }
void CS_CompilerVisitorBasis::Visit(CS_FunctionArgument &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_AnalogFunctionCall &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_HierarchSysParamFunctions &node) { VisitChildren(node); }
void CS_CompilerVisitorBasis::Visit(CS_StringFunction &node) { VisitChildren(node); }

/* ---------- Childe node Visitors  ------------------- */
void CS_CompilerVisitorBasis::VisitChildren(CS_ParameterDeclaration &node)
{
  visitNode(node.getDefaultExpr());
  for (int i = 0; i < node.getNrConstr(); i++) { visitNode(node.getConstraint(i)); }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_ValueRange &node)
{
  visitNode(node.getLeftExpr());
  visitNode(node.getRightExpr());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_VariableDeclaration &node)
{
  visitNode(node.getInitialExpr());
  if (node.getRangeObj()) { visitNode(node.getRangeObj()->getLeftExpr());
                            visitNode(node.getRangeObj()->getRightExpr()); }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_Port &node)
{
  if (node.getRangeObj()) { visitNode(node.getRangeObj()->getLeftExpr());
                            visitNode(node.getRangeObj()->getRightExpr()); }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_Range &node)
{
  visitNode(node.getLeftExpr()); visitNode(node.getRightExpr());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_BranchDeclaration &node)
{
  visitNode(node.getP1()); visitNode(node.getP2());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogFunctionDeclaration &node)
{
  // argument declarations
  for (int i = 0; i < node.getNrAgrs(); i++)
    visitNode(node.getArgumentDeclaration(i));
  // local variable declarations
  for (int i = 0; i < node.gerNrLocalVars() ; i++)
    visitNode(node.getLocalDeclr(i));
  // visit the body
  visitNode(node.getInstrSequence());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_ModuleInstantiation &node)
{
  // visit the port expression and the parameter values
  for (int i = 0; i < node.getNrPorts(); i++)
    visitNode( node.getPort(i) );
  for (int i = 0; i < node.getNrParameters(); i++)
    visitNode( node.getParamValue(i) );
}

// ---------- instruction visitors -------------------
void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogStatement &node) {
  VERILOG_PRINT_L3(verb(), "VisitorBasis::Accept(InstrBasis *node) called but "
      << typeid(node).name() << " is the argument")
  VERILOG_ERROR_STOP("Internal Error" , &node);
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogSeqBlock &node)
{
  for (int i = 0; i < node.getNrInstruction(); i++)  { visitNode(node.getInstruction(i) ); }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogConditionalStatement &node)
{
  visitNode(node.getExpr()); visitNode(node.getTrueBranch()); visitNode(node.getFalseBranch());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogEventControlStatement &node)
{
  for (int i = 0; i < node.getNrEventConditions() ; i++) {
      visitNode(node.getEventFunction(i));
  }
  visitNode(node.getInstruction());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_ContributionStatement &node)
{
  visitNode(node.getTargetExpr()); visitNode(node.getExpression() );
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogVariableAssignment &node)
{
  visitNode(node.getTargetVariable()); visitNode(node.getExpression());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogCaseStatement &node)
{
  // visit the main expression
  visitNode(node.getSwitchExpression());
  // then visit all the expressions
  for (int i = 0; i < node.getNrCases(); i++)
    {
      for (int j = 0; j < node.getExpressionsPerCase(i); j++)
        {
          // visit the case expression
          visitNode( node.getExpression(i,j) );
        }
    }

  // finally visit the statements
  for (int i = 0; i < node.getNrCases(); i++)
    {
      // visit the statements
      visitNode(node.getStatement(i));
    }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogDispOrSysFunc &node)
{ // visit the function
  visitNode(node.getFunctionCalled());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogLoopStatement &node)
{
  visitNode(node.getLoopInitialStatement());
  visitNode(node.getLoopCondition());
  visitNode(node.getLoopBody());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_IndirectContributionStatement &node)
{
  visitNode(node.getLeftExpr());
  visitNode(node.getInDirectExpression());
  visitNode(node.getRightExpression());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogLoopGenerateStatement &node)
{
  visitNode(node.getLoopInitStatement() );
  visitNode(node.getLoopCondition() );
  visitNode(node.getLoopIncrement() );
  visitNode(node.getLoopBody());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogInitialBlock &node)
{
  visitNode(node.getInitInstrSequence());
}

// --------- expressions visitors --------------------
void CS_CompilerVisitorBasis::VisitChildren(CS_ExpressionBasis &node)
{
  VERILOG_PRINT_L3(verb() ,"VisitorBasis::Accept(ExprBasis &node) called but " << typeid(node).name()
      << " is the argument");
  VERILOG_ERROR_STOP("Internal Error" , &node); /*Visit(node);*/
}
void CS_CompilerVisitorBasis::VisitChildren(CS_FunctionCall &node) {
  VERILOG_PRINT_L3(verb() ,"VisitorBasis::Accept(FunctionExpr *node) called but " << typeid(node).name()
      << " is the argument");
  VERILOG_ERROR_STOP("Internal Error", &node);
}
void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogConcatenation &node)
{
  for (int i = 0; i < node.getLength() ; i++) visitNode(node.getExpr(i));
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogEventFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) visitNode(node.getArg(i));
}

void CS_CompilerVisitorBasis::VisitChildren(CS_BranchProbeFunction &node)
{
  visitNode(node.getP1()); visitNode(node.getP2());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogFilterOperationFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogSmallSignalFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalysisFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_BinaryOperator &node)
{
  visitNode(node.getLeftExpr()); visitNode(node.getRightExpr());
}

void CS_CompilerVisitorBasis::VisitChildren([[maybe_unused]] CS_Number &node)   { /* nothing to call */ }

void CS_CompilerVisitorBasis::VisitChildren([[maybe_unused]] CS_String &node)  { /* nothing to call */ }

void CS_CompilerVisitorBasis::VisitChildren([[maybe_unused]] CS_Infinity &node){ /* nothing to call */ }

void CS_CompilerVisitorBasis::VisitChildren([[maybe_unused]] CS_HierarchSysParamFunctions &node)
{
  /* nothing to call */
}

// TODO: for CS_IndexedIdentifier we might throw an error
void CS_CompilerVisitorBasis::VisitChildren(CS_IndexedIdentifier &node) { visitNode(node.getIndexerExpr()); }
void CS_CompilerVisitorBasis::VisitChildren(CS_ParameterReference &node) { visitNode(node.getIndexerExpr()); }
void CS_CompilerVisitorBasis::VisitChildren(CS_VariableReference &node)  { visitNode(node.getIndexerExpr()); }
void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogPortReference &node){ visitNode(node.getIndexerExpr()); }

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogBuiltInMathFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogConditionalExpression &node)
{
  visitNode(node.getCondition()); visitNode(node.getThenExpr()); visitNode(node.getElseExpr());
}

void CS_CompilerVisitorBasis::VisitChildren(CS_SystemFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) {
      // one argument can be NULL, e.g. $strobe("  ",a,b, ,c);
      VERILOG_ERROR_COND_STOP(node.getArg(i) == 0, "Argument can not be NONE", &node);
      visitNode(node.getArg(i));
  };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_RandomFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) {
      // one argument can be NULL, e.g. $dist_normal( ,a,b, );
      VERILOG_ERROR_COND_STOP(node.getArg(i) == 0, "Argument can not be NONE", &node);
      visitNode(node.getArg(i));
  };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_EnvironmentParameterFunction &node)
{
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}

void CS_CompilerVisitorBasis::VisitChildren(CS_UnaryOperator &node) { visitNode(node.getExpression()); }

void CS_CompilerVisitorBasis::VisitChildren(CS_FunctionArgument &node) { visitNode(node.getArgument()); }

void CS_CompilerVisitorBasis::VisitChildren(CS_AnalogFunctionCall &node)
{
  for (int i = 0; i < node.getFunctionDeclr()->getNrAgrs() ; i++)
    {
      visitNode(node.getArgument(i));
    }
}

void CS_CompilerVisitorBasis::VisitChildren(CS_StringFunction &node)
{
  visitNode(node.getStringExpression());
  for (int i = 0; i < node.getNrArg() ; i++) { visitNode(node.getArg(i)); };
}
