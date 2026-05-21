/*
 * IP_SubcktElabor.hpp
 *
 *  Created on: Feb 24, 2015
 *      Author: benk
 */

#ifndef IP_SUBCKTELABOR_HPP_
#define IP_SUBCKTELABOR_HPP_

#include "IP_NetlistVisitBase.hpp"

/** this visitor class adds informatio to the subcircuit definitions
 * such that they have the direct information about different instances
 * in a sorted way (in hashtable)*/
class IP_SubcktElabor : public IP_NetlistVisitBase
{
public:

  IP_SubcktElabor();

  virtual ~IP_SubcktElabor() {;}

  /*
  virtual void   startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit);

  virtual void   Visit(IP_NetlistStructBase &node);
  virtual void   Visit(IP_NetlistStructSequence &node);
  virtual void   Visit(IP_ControlBase &node);
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
  virtual void   Visit(IP_AnaysisCard &node); */

};

#endif /* IP_SUBCKTELABOR_HPP_ */
