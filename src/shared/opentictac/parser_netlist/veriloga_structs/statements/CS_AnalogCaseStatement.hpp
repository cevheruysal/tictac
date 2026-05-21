/*
 * CS_AnalogCaseStatement.h
 *
 *  Created on: Feb 28, 2013
 *      Author: benk
 */

#ifndef CS_ANALOGCASESTATEMENT_H_
#define CS_ANALOGCASESTATEMENT_H_

#include "CS_AnalogStatement.hpp"
#include "CS_AnalogSeqBlock.hpp"

/**
 * Class representing the case statements (in C switch-case, but in Verilog-A only case)
 * It is a Verilog-A specialty that the expression in comparison all can be all
 * non-constants, hence
 *
 * in LRM see analog_case_statement */
class CS_AnalogCaseStatement : public CS_AnalogStatement
{
public:

  /** Ctor with the required input */
  CS_AnalogCaseStatement( CS_ExpressionBasis* switchExpr,
      const CS_ModuleDeclaration* module, int startLine, int startColumn);

  /** empty Dtor*/
  virtual ~CS_AnalogCaseStatement(){;}

  /** adds one case to this "switch-case" */
  void addCase( std::vector<CS_ExpressionBasis*>& cases,
                CS_AnalogSeqBlock* statement)
  {
    // we should not have two default cases
    VERILOG_ERROR_COND_STOP( (cases.size() == 0) && (defaultIndex_ > -1),
        "Case statement has already one default branch", statement);

    // if there is no expression then this is the default case
    if (cases.size() == 0)
        defaultIndex_ = nrCases_;

    // add to the vectors
    caseExpression_.push_back(cases);
    nrExpressionsPerCases_.push_back((int)cases.size());
    caseStatements_.push_back(statement);
    nrCases_++;
  }

  /** returns the number of expressions for one selected case */
  int getExpressionsPerCase(int c) const { return nrExpressionsPerCases_[c]; }

  /** returns the number of cases */
  int getNrCases() const { return nrCases_;}

  /** returns the expression for case "c" and index (within the case) "i" */
  const CS_ExpressionBasis* getExpression(int c, int i) const { return caseExpression_[c][i]; }
  CS_ExpressionBasis* getExpression(int c, int i) { return caseExpression_[c][i]; }

  /** returns the statement of a given index "c" */
  const CS_AnalogSeqBlock* getStatement(int c) const { return caseStatements_[c]; }
  CS_AnalogSeqBlock* getStatement(int c) { return caseStatements_[c]; }

  /** returns the index of the default case, -1 if there is none */
  int getDefaultIndex() const { return defaultIndex_; }

  /** returns the expression of the case statement */
  CS_ExpressionBasis* getSwitchExpression() { return switchExpr_; }
  const CS_ExpressionBasis* getSwitchExpression() const { return switchExpr_; }

  /** traverse method */
  virtual void Accept(CS_CompilerVisitorBasis &visitor)
  {
    visitor.Visit((CS_AnalogCaseStatement&)(*this));
  }

private:

  /** expression which decides the branching */
  CS_ExpressionBasis* switchExpr_;

  /** one case can contain several expressions, hence we have vector of vectors
   * if the last element exist but is empty then this is the default case */
  std::vector< std::vector<CS_ExpressionBasis*> > caseExpression_;

  /** number of expressions per case */
  std::vector<int> nrExpressionsPerCases_;

  /** the statements belonging to each case*/
  std::vector<CS_AnalogSeqBlock*> caseStatements_;

  /** number of cases */
  int nrCases_;

  /** internal flag to check weather we already have the default case*/
  int defaultIndex_;

};

#endif /* CS_ANALOGCASESTATEMENT_H_ */
