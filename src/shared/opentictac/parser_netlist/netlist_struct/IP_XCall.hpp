/*
 * IP_XCall.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_XCALL_HPP_
#define IP_XCALL_HPP_

#include "IP_NetlistStructBase.hpp"
#include "IP_Param.hpp"

/** this class represents the instantioation of a subcircuit */
class IP_XCall : public IP_NetlistStructBase
{
public:

  /** Ctor with the original tokens */
  IP_XCall(std::vector<IP_Token> &tokens,
      int nrPinns, std::string &sucktName,
      std::vector < TICTAC_SPTR<IP_Param> > &callParams,
      std::vector < std::string > &pinNameBinding);

  /** Ctor with the original tokens and a constant string*/
  IP_XCall(std::vector<IP_Token> &              tokens,
           int                                  nrPinns,
           std::string const &                  sucktName,
           std::vector<TICTAC_SPTR<IP_Param> > &callParams,
           std::vector<std::string> &           pinNameBinding);

  /** empty Dtor */
  virtual ~IP_XCall() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_XCall &)(*this)); }

  /** returns the number of pinns */
  int getNrPinns() const { return nrPinns_; }

  /// returns the instantion name of the pinns
  const std::string& getInstantionationPinn(MYINT i) const { return getToken(i+1).getValue(); }
  /// returns the X-call instantion
  const std::string& getInstantionationXname() const { return getToken(0).getValue(); }

  /** returnes the called subcircuit name */
  const std::string& getSubcktName() const { return sucktName_; }

  /** sets the name of the subcircuit name */
  void setSubcktName(std::string subcktName) { sucktName_ = subcktName; }

  /** returns the number of parameters on the X-call*/
  MYINT getNrParams() const {  return (MYINT)callParams_.size(); }

  /** returns the parameter on the X-call */
  TICTAC_SPTR<IP_Param>& getCalledParam(int i) { return callParams_[i]; }
  const TICTAC_SPTR<IP_Param>& getCalledParam(int i) const { return callParams_[i]; }
  const std::vector<TICTAC_SPTR<IP_Param>>& getCallParams() {return callParams_; };
  void add_param(TICTAC_SPTR<IP_Param> p);

  /** @returns positive number if the instantiation contains a given parameter, -1 otherwise*/
  MYINT hasInstParam(const std::string &paramName)
  {
    for (MYINT i = 0; i < (MYINT)callParams_.size(); i++)
      if (paramName == callParams_[i]->getName()) return i;
    return -1;
  }

  /** returns the size of the pin name binding vector, if this is given */
  int getPinNameBindingSize() const { return (int)pinNameBinding_.size(); }

  /** returns the vector of name binding, which is empty if there is no name
   * binding for the pins */
  std::vector < std::string >& getPinNameBinding() { return pinNameBinding_; }
  const std::vector < std::string >& getPinNameBinding() const { return pinNameBinding_; }

  const std::unordered_map< std::string, TICTAC_SPTR<IP_Param> >* get_params()
  {
    return &params_;
  }

  bool is_resolvable() {return is_resolvable_;};
  void is_resolvable(bool is_resolvable) {is_resolvable_ = is_resolvable;};

 private:
  // For digital parser: adc and dac calls shall not be resolved
  bool is_resolvable_ = true;

  /** nr pinns of the call */
  int nrPinns_;

  /** name of the called subcircuit */
  std::string sucktName_;

  /** all the parametres on the X-call */
  std::vector < TICTAC_SPTR<IP_Param> > callParams_;

  /** by an X-call the names can be bound not just by order but also by name,
   * In such case this vector will not be empty and will we filled with information
   * in the same order */
  std::vector < std::string > pinNameBinding_;

  std::unordered_map< std::string, TICTAC_SPTR<IP_Param> > params_;

  // This method is only for internal use
  void add_param_(TICTAC_SPTR<IP_Param> param)
  {
    auto ret = params_.try_emplace(param->getName(), param);
    if (!std::get<1>(ret))
      std::cout << "WARNING: Parameter already exists!! Do you have a conditional netlist??" << std::endl;
  }
};

#endif /* IP_XCALL_HPP_ */
