/*
 * IP_VerboseVisitor.hpp
 *
 *  Created on: Feb 6, 2015
 *      Author: benk
 */

#ifndef IP_VERBOSEVISITOR_HPP_
#define IP_VERBOSEVISITOR_HPP_

#include "IP_NetlistVisitBase.hpp"

/**
 * this visitor class is just there to visit a parset and print all the information
 * which is found */
class IP_VerboseVisitor : public IP_NetlistVisitBase
{
public:

  /** empty Ctor */
  IP_VerboseVisitor();

  /** empty Dtor */
  virtual ~IP_VerboseVisitor() {;}

  virtual void   startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit);

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

protected:

  /** get the tokens */
  virtual std::string toMyString(const IP_NetlistStructBase &node);

  /** tabls tor the printing */
  std::string tabs_;

};

#endif /* IP_VERBOSEVISITOR_HPP_ */
