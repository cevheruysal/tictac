/*
 * IP_NetlistStructInfo.hpp
 *
 *  Created on: Mar 16, 2015
 *      Author: benk
 */

#ifndef IP_NETLISTSTRUCTINFO_HPP_
#define IP_NETLISTSTRUCTINFO_HPP_

/** this class contrains information which is present in each netlist node (tree node).
 * Such (general) information can be used for filtering or other spe*/
class IP_NetlistStructInfo
{
public:
  /// Ctor with setting default values
  IP_NetlistStructInfo();
  IP_NetlistStructInfo(const IP_NetlistStructInfo &c);
  /// empty Dtor
  virtual ~IP_NetlistStructInfo() {;}
  /// @returns the flag for "is used"
  bool isUsed() const { return isUsed_;}
  /// sets the flag for used
  void setUsed(bool b) { isUsed_ = b; }
protected:
  /** to show if an elemen is "used" */
  bool isUsed_;
};

#endif /* IP_NETLISTSTRUCTINFO_HPP_ */
