/*
 * IP_ConditionedNetlist.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_CONDITIONEDNETLIST_HPP_
#define IP_CONDITIONEDNETLIST_HPP_

#include "IP_NetlistStructBase.hpp"
#include "IP_NetlistStructSequence.hpp"

/** this class represents the IF THEN ELSE constructions in the netlist
 * This should be an organic part of the netlist since we want to
 * allow general alterations */
class IP_ConditionedNetlist : public IP_NetlistStructBase
{
public:

  /** Ctor with tokens
   * IMPORTANT: the second token MUST be the expression for the condition
   * (first is ".IF" third is "THEN" )*/
  IP_ConditionedNetlist(std::vector<IP_Token> &tokens);

  /** empty Dtor */
  virtual ~IP_ConditionedNetlist() {;}

  /** returns true if there is a false branch */
  bool hasFalseBranch() const { return hasFalseBranch_; }

  /** activates the false branch */
  void setFalseBranch() { hasFalseBranch_ = true; }

  /** returns the true branch */
  TICTAC_SPTR<IP_NetlistStructSequence>& getTrueBranch() { return  thenBody_;}
  const TICTAC_SPTR<IP_NetlistStructSequence>& getTrueBranch() const { return  thenBody_;}

  /** returns the false branch */
  TICTAC_SPTR<IP_NetlistStructSequence>& getFalseBranch() { return  falseBody_;}
  const TICTAC_SPTR<IP_NetlistStructSequence>& getFalseBranch() const { return  falseBody_;}

  /** set the sequence for the true branch */
  void setThanBranch(TICTAC_SPTR<IP_NetlistStructSequence>& newThanBr) {
    thenBody_ = newThanBr;
  }

  /** set the sequence for the false branch, if there is one specified */
  void setFalseBranch(TICTAC_SPTR<IP_NetlistStructSequence>& newFalseBr) {
    falseBody_ = newFalseBr;
    hasFalseBranch_ = true;
  }

  /** returns the condition as it is read in as token */
  IP_Token& getConditionToken() { return condExpression_; }
  const IP_Token& getConditionToken() const { return condExpression_; }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_ConditionedNetlist &)(*this)); }

private:

  /** the true branch of the IF condition */
  TICTAC_SPTR<IP_NetlistStructSequence> thenBody_;

  /** the else branch if there is any, otherwise NULL*/
  TICTAC_SPTR<IP_NetlistStructSequence> falseBody_;

  /** true if there is a false branch */
  bool hasFalseBranch_;

  /** the expression in the condition */
  IP_Token condExpression_;

};

#endif /* IP_CONDITIONEDNETLIST_HPP_ */
