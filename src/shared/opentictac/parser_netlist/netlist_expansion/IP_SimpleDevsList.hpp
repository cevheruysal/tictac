/*
 * IP_SimpleDevsList.hpp
 *
 *  Created on: Feb 23, 2015
 *      Author: benk
 */

#ifndef IP_SIMPLEDEVSLIST_HPP_
#define IP_SIMPLEDEVSLIST_HPP_

#include "IP_DeviceMatchinRule.hpp"

// ===== SIMPLE RESISTOR MATCH =================
/** device matching for simple resistor */
class IP_MatchRes : public IP_DeviceMatchinRule {
public:

  IP_MatchRes();
  virtual ~IP_MatchRes() {;}
  virtual Device* createDevice( std::string &instanceName,
      std::vector< MYINT > &pinns, std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues, std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis>  > &MNAExprs, MYINT mfact = 1
      ) const ;
};

/** device matching for simple capacitor */
class IP_MatchCap : public IP_DeviceMatchinRule {
public:

  IP_MatchCap();
  virtual ~IP_MatchCap() {;}
  virtual Device* createDevice( std::string &instanceName,
      std::vector< MYINT > &pinns, std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues, std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis>  > &MNAExprs, MYINT mfact = 1
      ) const ;
};

/** device matching for simple inductor */
class IP_MatchInd : public IP_DeviceMatchinRule {
public:

  IP_MatchInd();
  virtual ~IP_MatchInd() {;}
  virtual Device* createDevice( std::string &instanceName,
      std::vector< MYINT > &pinns, std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues, std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis>  > &MNAExprs, MYINT mfact = 1
      ) const ;
};

/** device matching for simple DC voltage source */
class IP_MatchVDC : public IP_DeviceMatchinRule
{
 public:
  IP_MatchVDC();
  virtual ~IP_MatchVDC() { ; }
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;
};

/** device matching for SIN V-source */
class IP_MatchVSIN : public IP_DeviceMatchinRule {
public:

  IP_MatchVSIN();
  virtual ~IP_MatchVSIN() {;}
  virtual Device* createDevice( std::string &instanceName,
      std::vector< MYINT > &pinns, std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues, std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis>  > &MNAExprs, MYINT mfact = 1
      ) const ;
};

/** device matching for PULSE V-source */
class IP_MatchVPULSE : public IP_DeviceMatchinRule {
public:

  IP_MatchVPULSE();
  virtual ~IP_MatchVPULSE() {;}
  virtual Device* createDevice( std::string &instanceName,
      std::vector< MYINT > &pinns, std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues, std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis>  > &MNAExprs, MYINT mfact = 1
      ) const ;
};

/** device matching for PULSE V-source */
class IP_MatchVPWL : public IP_DeviceMatchinRule {
public:

  IP_MatchVPWL();
  virtual ~IP_MatchVPWL() {;}
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;
};

class IP_MatchIDC : public IP_DeviceMatchinRule
{
 public:
  IP_MatchIDC();
  virtual ~IP_MatchIDC() { ; }
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;

};

/** device matching for simple NMOS and PMOS MOSFETS */
class IP_MatchMOS0 : public IP_DeviceMatchinRule
{
 public:
  IP_MatchMOS0();
  virtual ~IP_MatchMOS0() { ; }
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;
};


class IP_MatchDIO : public IP_DeviceMatchinRule
{
 public:
  IP_MatchDIO();
  virtual ~IP_MatchDIO() { ; }
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;

};

class IP_MatchBJT : public IP_DeviceMatchinRule
{
 public:
  IP_MatchBJT();
  virtual ~IP_MatchBJT() { ; }
  virtual Device *createDevice(std::string &                                  instanceName,
                               std::vector<MYINT> &                           pinns,
                               std::vector<std::string> &                     paramNames,
                               std::vector<ConstExpressionValue> &            paramValues,
                               std::vector<MYINT> &                           paramFlags,
                               std::vector<TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
                               MYINT                                          mfact = 1) const;

};

#endif /* IP_SIMPLEDEVSLIST_HPP_ */
