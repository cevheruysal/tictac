/*
 * IP_NetlistVisitorUtils.hpp
 *
 *  Created on: Oct 11, 2016
 *      Author: benk
 */

#ifndef IP_NETLISTVISITORUTILS_HPP_
#define IP_NETLISTVISITORUTILS_HPP_

#include "IP_NetlistVisitBase.hpp"

/** visitot just to test if a given element is within a subcircuit or not */
class IP_HasSubcircuitInstance : public IP_NetlistVisitBase
{
public:
  IP_HasSubcircuitInstance();

  virtual  ~IP_HasSubcircuitInstance() {;}

  /** static method to see if a subcircuit has a specific instance given by
   * the instance name*/
  static bool hasSubcircuitInstance(IP_SubcirDef* toplevelToVisit,
      const std::string& instanceNameToLookFor);

  /** setting the instance name what we are looking for*/
  void setStringToMatch(const std::string &elementNameToFind) {
    elementNameToFind_ = elementNameToFind;
  }

  /// returns true if the last search had sucess
  bool   hadMatch() const { return hasMatch_; }

  /** the main entry point to visit a netlist
   * @param toplevelToVisit the top-lvel subcircuit */
  void   startTopLevelVisit(IP_SubcirDef* toplevelToVisit);

  void   VisitChildren(IP_SubcirDef &node);
  void   VisitChildren(IP_Element &node);

private:

  /// the instance name what we are looking for within the current subcircuit
  std::string elementNameToFind_;

  /// true if the search was successful
  bool hasMatch_;
};

#endif /* IP_NETLISTVISITORUTILS_HPP_ */
