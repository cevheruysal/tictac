/*
 * CS_AnalogStatement.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: benk
 */

#ifndef CS_ANALOGSTATEMENT_HPP_
#define CS_ANALOGSTATEMENT_HPP_

#include "verilog_comp.hpp"
#include "CS_ModuleDeclaration.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_GenerateInfo.hpp"
#include "CS_ErrorInfo.hpp"

/** Base class for the instructions.
 * This class contains the necessary interface functions to interact with the operations
 *
 * see analog_statement in the LRM */
class CS_AnalogStatement : public CS_CompilerTreeNode, public CS_GenerateInfo, public CS_ErrorInfo
{
public:

  /** Ctor of the instruction basis class with the required informations */
  CS_AnalogStatement(const CS_ModuleDeclaration* module, int startLine, int startColumn);

  virtual ~CS_AnalogStatement() {;}

  /** function that tells if this instruction is an elementary operation (assignment or contribution) */
  virtual bool isElementaryInsruction() const { return false; }

  /** inherited from the super class */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) = 0;

  /** returns the Verilog-A module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  int getStartLine() const { return startLine_; }
  int getStartColumn() const { return startColumn_; }

  /** returns the comments added to one statement */
  const std::string& getComment() const { return comments_; }

  /** adds comment to this statement (use without \* characters,
   * those will be added automatically) */
  void addStatementComment(std::string newComments) { comments_ += newComments; }

private:

  /** start line of the expression */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** the module that contains this expression */
  const CS_ModuleDeclaration* module_;

  /** comments that can be added to any statement and could be used
   * later for debug code generation */
  std::string comments_;
};

#endif /* CS_ANALOGSTATEMENT_HPP_ */
