/*
 * IP_GlobalSettings.hpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#ifndef IP_GLOBALSETTINGS_HPP_
#define IP_GLOBALSETTINGS_HPP_

#include "IP_ControlBase.hpp"

/** various global setting types which models the class IP_GlobalSettings */
typedef enum {
  IP_GSETTING_GLOBAL = 0,     ///< .GLOBAL
  IP_GSETTING_SAVE   = 1,     ///< .SAVE
  IP_GSETTING_TEST   = 2,     ///< .TEST
  IP_GSETTING_TEMP   = 3,     ///< .TEMP
  IP_GSETTING_SYSTEMC= 4,     ///< .SYSTEMC
  IP_GSETTING_ERR = 5,        ///< error
} GlobalSettingType;

/** class to model various global setting in the netlist
 * such sa .GLOBAL or .SAVE, .TEST ... */
class IP_GlobalSettings : public IP_ControlBase
{
public:
  /** the Ctor with the array of strings */
  IP_GlobalSettings(std::vector<IP_Token> &tokens, GlobalSettingType type);

  /** empty Dtor */
  virtual  ~IP_GlobalSettings() {;}

  /** returns the type of the global setting */
  GlobalSettingType getGlobalSettingType() const { return type_; }


  /** returns the type as a string */
  static std::string getTypeString(GlobalSettingType t)
  {
    switch (t) {
      case IP_GSETTING_GLOBAL : return std::string("GLOBAL");
      case IP_GSETTING_SAVE :   return std::string("SAVE");
      case IP_GSETTING_TEST :   return std::string("TEST");
      case IP_GSETTING_TEMP :   return std::string("TEMP");
      case IP_GSETTING_SYSTEMC :return std::string("SYSTEMC");
      default:                  return std::string("ERROR");
    }
  }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_GlobalSettings &)(*this)); }

private:

   /** type of the global setting record */
   GlobalSettingType type_;
};

#endif /* IP_GLOBALSETTINGS_HPP_ */
