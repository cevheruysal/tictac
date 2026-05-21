/*
 * CS_BranchProbeFunction.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#ifndef BRANCHPROBEFUNCTION_HPP_
#define BRANCHPROBEFUNCTION_HPP_

#include "CS_ExpressionBasis.hpp"
#include "CS_AnalogPortReference.hpp"

/** we distinguish between 4 types of branch expressions, depending on the number
 * of arguments and on the type of the expression I/V
 * See analog_port_reference in LRM */
typedef enum {
  VERILOG_BRANCH_V_TWO = 0,
  VERILOG_BRANCH_V_ONE = 1,
  VERILOG_BRANCH_I_TWO = 2,
  VERILOG_BRANCH_I_ONE = 3,
  VERILOG_BRANCH_I_EXT_PROBE = 4 // I(<p>) port_probe_function_call in LRM
} BranchFunctionType;

/** this class models the V(p1,p2)/V(p1) or I(p1,p2)/I(p1) or I(<p1>) expressions
 *
 * see branch_probe_function_call (branch_lvalue) and port_probe_function_call in LRM
 *
 * p1 always mot be non NULL
 * if p2 is NULL then the second pin is by default the ground */
class CS_BranchProbeFunction: public CS_ExpressionBasis
{
public:

  /** Ctor */
  CS_BranchProbeFunction(const CS_ModuleDeclaration* module, int startLine, int startColumn,
      BranchFunctionType bt, std::string branchName = "",
      CS_AnalogPortReference* p1 = 0, CS_AnalogPortReference* p2 = 0);

  /** empty Dtor */
  virtual ~CS_BranchProbeFunction() {;}

  /** returns the number of ports involved into the branch expression */
  int getNrPortsInvolved() const { return  (p2_ == 0) ? 1 : 2 ;}

  /** returns the ports involved */
  const CS_AnalogPortReference* getP1() const { return p1_; }
  CS_AnalogPortReference* getP1() { return p1_; }
  const CS_AnalogPortReference* getP2() const { return p2_; }
  CS_AnalogPortReference* getP2() { return p2_; }

  /** returns the type of the branch expression */
  BranchFunctionType getBranchExpressionType() const { return bt_; }

  /** returns the name of the branch */
  const std::string& getBranchName() const { return branchName_; }

  /** sets type of the branch expression */
  void setBranchExpressionType(BranchFunctionType bt) { bt_ = bt; }

  /** returns the classes that can be found in an expression (constant, ports, parameters, variables)*/
  virtual int getClasses() const { return (int)PORT; }

  /** returns the types (actual INTEGER or REAL)*/
  virtual int getTypes() const { return (int)REAL; }

  /** method for the tree traversal */
  virtual void Accept(CS_CompilerVisitorBasis &visitor) { visitor.Visit( (CS_BranchProbeFunction &)(*this)); }

  /** propagate recursively the system dependency informations */
  virtual CS_AKDependency propagateDependencies();

  /** returns the instance sign of this branch */
  int getInstanceSign() const { return signInstance_; }

  /** sets the instance sign of this branch */
  void setInstanceSign(int newSign) { signInstance_ = newSign; }

  /** returns all the possible pins that this branch might affect:
   * In case of e.g., V(p[i],n[j]) it returns all the combinations that is the number
   * size(p)*size(n). In case of constant indexed expression the expressions will be evaluated
   * @returns the number of all possible combination including the name of the branch */
  int getPossibleBranches(std::vector<int>& arrayP1, std::vector<int>& arrayP2,
      std::vector<std::string>& brNames);

  /** method for recursive differentiation with AD*/
  virtual CS_ExpressionBasis* doDifferentiate(int sysVarIndex, int *flag);

  // --- for the AK generation ---
  /** return the string in the eval function for a contribution */
  std::string getAkTypeString() const;

  /** function that creates a deep copy of the expression itself */
  virtual CS_ExpressionBasis* copy() {
    // port items
    CS_AnalogPortReference *p1 = dynamic_cast<CS_AnalogPortReference*>((p1_)?p1_->copy():0);
    CS_AnalogPortReference *p2 = dynamic_cast<CS_AnalogPortReference*>((p2_)?p2_->copy():0);
    // create new object
    CS_BranchProbeFunction *ret = new CS_BranchProbeFunction(this->getModule(),
        this->getStartLine(), this->getStartColumn(), bt_ , branchName_, p1, p2 );
    ret->setInstanceSign(this->getInstanceSign());
    return ret;
  }

private:

  /** type of the branch Expression */
  BranchFunctionType bt_;

  /** the name of the branch, empty if it is defined by pinns, not
   * empty if it is given by a e.g. branch (p1,p2) br , than this strings will become "br" */
  std::string branchName_;

  /** the first port number within a module*/
  CS_AnalogPortReference *p1_;

  /** the second port number within the module */
  CS_AnalogPortReference *p2_;

  // -------- AK specific ------------
  /** for AK for e.g., I(p,n) and I(n,p) needs one branch instance, so for example
   * the second uses the same branch but with reversed sign */
  int signInstance_;
};

#endif /* BRANCHPROBEFUNCTION_HPP_ */
