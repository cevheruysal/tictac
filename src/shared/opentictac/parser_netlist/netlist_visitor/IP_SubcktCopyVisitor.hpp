/*
 * IP_SubcktCopyVisitor.hpp
 *
 *  Created on: Mar 16, 2015
 *      Author: benk
 */

#ifndef IP_SUBCKTCOPYVISITOR_HPP_
#define IP_SUBCKTCOPYVISITOR_HPP_

#include "IP_NetlistVisitBase.hpp"

class IP_Token;

/** class to copy a subcircuit definition
 *
 * This class could later alse be used for "filtering" a given netlist */
class IP_SubcktCopyVisitor : public IP_NetlistVisitBase
{
public:

  IP_SubcktCopyVisitor();

  virtual ~IP_SubcktCopyVisitor() {;}

  virtual void   startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit);

  virtual void   Visit(IP_NetlistStructSequence &node);
  virtual void   Visit(IP_SubcirDef &node);
  virtual void   Visit(IP_Element &node);
  virtual void   Visit(IP_Function &node);
  virtual void   Visit(IP_Model &node);
  virtual void   Visit(IP_Param &node);
  virtual void   Visit(IP_SubcirSettings &node);
  virtual void   Visit(IP_XCall &node);
  virtual void   Visit(IP_ConditionedNetlist &node);
  virtual void   Visit(IP_GlobalSettings &node);
  virtual void   Visit(IP_Option &node);
  virtual void   Visit(IP_Output &node);
  virtual void   Visit(IP_Measurement &node);
  virtual void   Visit(IP_Assertion &node);
  virtual void   Visit(IP_AnaysisCard &node);

  /// returns the copied subcircuit
  TICTAC_SPTR<IP_SubcirDef> getResultingSubcircuitDef() { return resultedSubckt_; }

protected:

  /// method to create "tokens_" based on an existent element
  void getTokenSeqence(IP_NetlistStructBase* elem);

  /// the list of tokens to copy one element
  std::vector<IP_Token> tokens_;

  /// the returned objects
  TICTAC_SPTR<IP_NetlistStructBase> ret_;
  /// store the pointer which was added at the last point
  TICTAC_SPTR<IP_NetlistStructBase> last_added_;

  TICTAC_SPTR<IP_NetlistStructSequence> seq_;

  /// the resulted subcircuit definition
  TICTAC_SPTR<IP_SubcirDef> resultedSubckt_;

  /// For flattening out the digital elements, we need to a sense of the hierarchy
  /// Since we exlude conditional netlists, we can think purely in terms of subcircuits
  /// Note that NetlistStructSequences can be contained in each other without a
  /// Subcircuit separating them. This "trick" is used several times in IP_SIMetrixCopy.cpp
  TICTAC_SPTR<IP_SubcirDef> currentSubckt_ = nullptr;
  TICTAC_SPTR<IP_SubcirDef> parentSubckt_ = nullptr;
};

#endif /* IP_SUBCKTCOPYVISITOR_HPP_ */
