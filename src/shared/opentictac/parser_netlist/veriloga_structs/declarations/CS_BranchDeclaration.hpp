/*
 * CS_BranchDeclaration.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#ifndef CS_BRANCHDECLRATION_HPP_
#define CS_BRANCHDECLRATION_HPP_

#include "verilog_comp.hpp"
#include "CS_ModuleDeclaration.hpp"

#include "CS_AnalogPortReference.hpp"
#include "CS_CompilerTreeNode.hpp"
#include "CS_ErrorInfo.hpp"

/** class for branch declaration.
 * See branch_declaration in the LRM */
class CS_BranchDeclaration : public CS_CompilerTreeNode, public CS_ErrorInfo
{
public:

  /** Ctor for a user specific branch declaration */
  CS_BranchDeclaration(const CS_ModuleDeclaration* module, int startLine, int startColumn, std::string name,
      CS_AnalogPortReference *pin1, CS_AnalogPortReference *pin2 );

  /** empty Dtor*/
  virtual ~CS_BranchDeclaration() {;}

  /** return the expression index of the first port */
  const CS_AnalogPortReference* getP1() const { return pin1_; }
  CS_AnalogPortReference* getP1() { return pin1_; }

  /** return the expression index of the second port */
  const CS_AnalogPortReference* getP2() const { return pin2_; }
  CS_AnalogPortReference* getP2() { return pin2_; }

  /** returns the name of the declared branch */
  const std::string& getName() const { return name_; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_BranchDeclaration &)(*this)); }

  /** returns the module */
  const CS_ModuleDeclaration* getModule() const { return module_;}

  /** line nr in the Verilog file*/
  int getStartLine() const { return startLine_; }

  /** column number in the Verilog file */
  int getStartColumn() const { return startColumn_; }

private:

  /** name of the declared branch */
  std::string name_;

  /** the first pin of the declaration */
  CS_AnalogPortReference *pin1_;

  /** the second pin of the declaration (this should also ne non-zero)*/
  CS_AnalogPortReference *pin2_;

  /** start line of the expression */
  const int startLine_;

  /** start column */
  const int startColumn_;

  /** the module that contains this expression */
  const CS_ModuleDeclaration* module_;
};

#endif /* CS_BRANCHDECLRATION_HPP_ */
