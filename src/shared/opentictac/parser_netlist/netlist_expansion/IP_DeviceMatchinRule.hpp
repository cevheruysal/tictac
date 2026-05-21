/*
 * IP_DeviceMatchinRule.hpp
 *
 *  Created on: Feb 20, 2015
 *      Author: benk
 */

#ifndef IP_DEVICEMATCHINRULE_HPP_
#define IP_DEVICEMATCHINRULE_HPP_

#include "Device.hpp"
#include "sim_utils.hpp"
#include "verilog_comp.hpp"
#include "CS_ExpressionBasis.hpp"

#include "IP_ExprConstEvaluator.hpp"

/** this class contains the informaiton, which is attached to a device and is used to
 * match it during the netlist expansion */
class IP_DeviceMatchinRule : public UT_SIMVerboseClass
{
public:

  /** Ctor */
  IP_DeviceMatchinRule();

  /** copy Ctor */
  IP_DeviceMatchinRule(IP_DeviceMatchinRule &c);

  /** Dtor */
  virtual ~IP_DeviceMatchinRule() {;}

  /** creates a device instance when there was a match. The netlist expansion will
   * need to provide.
   * @param instanceName the name of the created instance
   * @param pinns the pinns to connect to (real GID in the netlist)
   * @param paramNames the params which are given in the netlist
   * @param paramValues for each provided parameter also the computed value
   *        (IMPORTANT: these computed values, later, one instance of MC or alter)
   *        These parameters also contain the model parameters.
   *        ( Should we maybe store these parametrs separately ???
   *          Would this be a significant storage saving ??? )
   * @param paramFlags flags if e.g. this param is given on the record or from model
   *        etc ...
   * @param MNAExprs various (in the specified order) MNA expressions, which during
   *        the netlist expansion do not have concrete values. This also means
   *        they must contain MNA elements.
   * @param mfact the multiplicator factor
   * @returns an instance of the device */
  virtual Device* createDevice(
      std::string &instanceName,
      std::vector< MYINT > &pinns,
      std::vector< std::string > &paramNames,
      std::vector< ConstExpressionValue > &paramValues,
      std::vector< MYINT > &paramFlags,
      std::vector< TICTAC_SPTR<CS_ExpressionBasis> > &MNAExprs,
      MYINT mfact = 1
      ) const = 0 ;

  // ======== here come the gettes of the various fields
  DeviceType getDevType() const { return devType_; }
  MYINT getNrPinns() const { return nrPinns_; }

  const std::vector< std::string >& getPinNames() const { return optionalPinNames_; }
  const std::vector< MYINT >&       getPinFlags()  const  { return pinFlags_; }

  const std::string&                getFirstKeyWord() const { return  firstKeyWord_; }
  const std::string&                getFirstKeyValue() const { return firstKeyWordValue_; }
  const std::string&                getModelType() const { return modelType_; }

  const std::vector< std::string >&  getParamName() const { return paramNames_;}
  const       std::vector<VarType>&  getParamType() const { return paramTypes_;}
  const std::vector<ConstExpressionValue>& getParamsDefulatValue() const { return paramDefaultValue_; }

  const std::vector< MYINT >&       getParamAliasIndicies() const { return paramAliasInd_; }
  const std::vector< std::string >& getParamAliasNames()    const { return paramAliasNames_; }

  const std::string&                getModuleName() const { return moduleName_; }
  const std::string&                getMNAKeyword() const { return lastKeyWord_; }


  const std::string&  getDeviceTypeName() const { return deviceName_; }

protected:

  /** device type to match, used for instance */
  DeviceType devType_;
  /** the name of the device (type) (to provide some information)*/
  std::string deviceName_;

  /** number of pinns */
  MYINT nrPinns_;

  /** name of the pinns */
  std::vector< std::string > optionalPinNames_;
  /** optional pinn flags (at the moment not used, but later they might be used)
   * e.g. pure control pinns might come later (in hSpice e.g.)*/
  std::vector< MYINT > pinFlags_;

  /** after the pinns or between the pinns we might have contructs "keyword" or
   * "keyword=fixedvalue"*/
  std::string firstKeyWord_;
  std::string firstKeyWordValue_;

  /** the name of the model , if empty non is required */
  std::string modelType_;

  /** names of the parameters and their type, ATTENTION: there could be only one
   * array parameter ??? */
  std::vector< std::string >  paramNames_;
  std::vector<VarType>        paramTypes_;
  std::vector<ConstExpressionValue> paramDefaultValue_;

  /** datastructure for parameter aliases, this is the index in paramNames_ */
  std::vector< MYINT >       paramAliasInd_;
  /** the alias name (for index given previously in the original parametr list) */
  std::vector< std::string > paramAliasNames_;

  /** module name , such as "PWL" and  this is followed by an MNA expression
   * if empty than this is not there */
  std::string moduleName_;

  /** last keywor after which an MNA expression MUST stay with "=" */
  std::string lastKeyWord_;
};

#endif /* IP_DEVICEMATCHINRULE_HPP_ */
