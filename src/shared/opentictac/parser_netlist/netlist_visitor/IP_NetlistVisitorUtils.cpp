/*
 * IP_NetlistVisitorUtils.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: benk
 */

#include "IP_NetlistVisitorUtils.hpp"

#include "IP_SubcirDef.hpp"
#include "IP_Element.hpp"

IP_HasSubcircuitInstance::IP_HasSubcircuitInstance() : IP_NetlistVisitBase()
{
  // set the flag correctly
  hasMatch_ = false;
}

bool  IP_HasSubcircuitInstance::hasSubcircuitInstance(IP_SubcirDef* toplevelToVisit,
      const std::string& instanceNameToLookFor)
{
  IP_HasSubcircuitInstance visit;
  visit.setStringToMatch(instanceNameToLookFor);
  visit.startTopLevelVisit(toplevelToVisit);
  // return the flag
  return visit.hadMatch();
}

void   IP_HasSubcircuitInstance::startTopLevelVisit(IP_SubcirDef* toplevelToVisit)
{
  hasMatch_ = false;
  // call the visitor
  //std::cout << " IP_HasSubcircuitInstance::startTopLevelVisit " << toplevelToVisit->getName() << "\n";
  IP_NetlistVisitBase::startTopLevelVisit(toplevelToVisit);
}

void   IP_HasSubcircuitInstance::VisitChildren(IP_SubcirDef &node){
  bool tmpsave = isTopLevel_;
  level_++;
  isTopLevel_ = (level_ > 0);
  //std::cout << " visit = " << isTopLevel_ << "\n";
  if (isTopLevel_)
    visitNode( node.getSubcktBody().get() );
  level_--;
  isTopLevel_ = tmpsave;
}
void   IP_HasSubcircuitInstance::VisitChildren(IP_Element &node)
{
  //std::cout << " Compare element = " << node.getInstName() << "\n";
  // compare instance instance name and set flag if there is a match
  if (node.getInstName() == elementNameToFind_)
    hasMatch_ = true;
}
