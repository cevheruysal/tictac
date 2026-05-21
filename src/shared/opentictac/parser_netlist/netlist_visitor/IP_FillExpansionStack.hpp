/*
 * IP_FillExpansionStack.hpp
 *
 *  Created on: Jan 9, 2017
 *      Author: benk
 */

#ifndef SRC_PARSER_NETLIST_NETLIST_VISITOR_IP_FILLEXPANSIONSTACK_HPP_
#define SRC_PARSER_NETLIST_NETLIST_VISITOR_IP_FILLEXPANSIONSTACK_HPP_

#include "sim_utils.hpp"
#include "IP_NetlistVisitBase.hpp"
#include "IP_ExpansionStack.hpp"

/**
 * \brief This class is used just to gather parameters and models
 * on the actual level
 *
 * Classes such as IP_SIMetrixCopy, which is a copy visitor based on IP_SubcktCopyVisitor,
 * need to access information of siblings or parent when walking through the tree. For example,
 * when an IP_Element is based on a model and so the model information need to be retrieved.
 *
 * By storing ExpansionStacks in an std::vector, also parent information (e.g., models or
 * subcircuits etc) can be retrieved.
 * Information in cousin nodes are never needed!
 */
class IP_FillExpansionStack : public IP_NetlistVisitBase
{
public :
  IP_FillExpansionStack(ExpansionStack* stack, IP_SubcirDef &node) :
      expansion_stack_(stack)
  {
    //this->setVerb(6);
    IP_FillExpansionStack::Visit(node);
  };

  virtual ~IP_FillExpansionStack() {;}

  virtual void   Visit(IP_AnaysisCard &node);
  virtual void   Visit(IP_SubcirDef &node);
  virtual void   Visit(IP_Element &node);
  virtual void   Visit(IP_Model &node);
  virtual void   Visit(IP_Param &node);
  virtual void   Visit(IP_XCall &node);
  virtual void   Visit(IP_ConditionedNetlist &node);
private:
  /// The ExpansionStack with the gathered parameters
  ExpansionStack* expansion_stack_;

  /** \brief This is not the absolute level!
   *
   *   The variable levelVisit_ is used so that IP_FillExpansionStack::Visit(IP_SubcirDef &node) only descends into the
   *   subcircuit body of the current node.
   */
  int levelVisit_ = 0;
};

#endif /* SRC_PARSER_NETLIST_NETLIST_VISITOR_IP_FILLEXPANSIONSTACK_HPP_ */
