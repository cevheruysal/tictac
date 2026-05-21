/*
 * IP_SubcirSettings.hpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#ifndef IP_SUBCIRSETTINGS_HPP_
#define IP_SUBCIRSETTINGS_HPP_

#include "IP_NetlistStructBase.hpp"

/** various stuff which can appear also within the subcircuit definition  */
typedef enum {
  IP_SSETTING_CONNECT = 0,   ///< .CONNECT
  IP_SSETTING_NODESET = 1,   ///< .NODESET
  IP_SSETTING_IC = 2,        ///< .IC
  IP_SSETTING_COMOPT = 3,    ///< .COMOPT ... just to ignore these
  IP_SSETTING_HDL    = 4,    ///< .HDL
  IP_SSETTING_ERR = 5        ///< error
} SubcktSettingType;

/** this class represents additional options which can appear within
 * the subcircuit definitions */
class IP_SubcirSettings  : public IP_NetlistStructBase
{
public:
  /** Ctor with tokens and with the type */
  IP_SubcirSettings(std::vector<IP_Token> &tokens, SubcktSettingType type);

  /** empty Dtor */
  virtual ~IP_SubcirSettings() {;}

  /** returns the type of the subcircuit setting */
  SubcktSettingType getType() const { return type_; }

  /** returns the type as a string */
  static std::string getTypeString(SubcktSettingType t)
  {
    switch (t) {
      case IP_SSETTING_CONNECT : return std::string("CONNECT");
      case IP_SSETTING_NODESET : return std::string("NODESET");
      case IP_SSETTING_IC      : return std::string("IC");
      case IP_SSETTING_COMOPT  : return std::string("COMOPT");
      case IP_SSETTING_HDL     : return std::string("HDL");
      default:                   return std::string("ERROR");
    }
  }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_SubcirSettings &)(*this)); }

private:

  /** type of this (within subcircuit allowed) settings */
  SubcktSettingType type_;
};

#endif /* IP_SUBCIRSETTINGS_HPP_ */
