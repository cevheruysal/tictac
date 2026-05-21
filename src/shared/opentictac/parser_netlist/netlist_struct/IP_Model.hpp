/*
 * IP_Model.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_MODEL_HPP_
#define IP_MODEL_HPP_

#include "IP_NetlistStructBase.hpp"
#include "IP_Param.hpp"

/** this class models an arbitrary model declaration
 * in the netlist */
class IP_Model : public IP_NetlistStructBase
{
public:

  /** Ctor: the array of tokens contain the whole record
   * @param name the name of the model
   * @param type the type of the mode
   * @param modelParams the model parameters */
  IP_Model(std::vector<IP_Token> &tokens,
      std::string &name, std::string &type,
      std::vector < TICTAC_SPTR<IP_Param> > &modelParams);

  IP_Model(std::vector<IP_Token> &tokens,
           const std::string &&name, const std::string &&type,
           std::vector < TICTAC_SPTR<IP_Param> > &modelParams);

  IP_Model(std::vector<IP_Token> &tokens,
           const std::string &name, const std::string &&type,
           std::vector < TICTAC_SPTR<IP_Param> > &modelParams);

  /** Copy Ctor */
  IP_Model( const IP_Model &m);

  virtual ~IP_Model() {;}

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_Model &)(*this)); }

  /** get the number of model parameters */
  MYINT getNrModelParams() const { return (MYINT)modelParams_.size(); }

  TICTAC_SPTR<IP_Param>& getModelParam(int i) { return modelParams_[i]; }
  const TICTAC_SPTR<IP_Param>& getModelParam(int i) const { return modelParams_[i]; }

  /** looks for a parameter , returns NULL if not found */
  const IP_Param* getModelParam(std::string paramName, bool caseSens = false) const;

  /** returns the model in lower case stored (if no case sensitive)*/
  const std::string& getName() const { return name_; }
  /** sets a new name to this model */
  void setName(std::string newName)  { name_ = newName; }

  /** returns the type of the the model stored as string (lower case (if no case sensitive))*/
  const std::string& getType() const { return  type_; }

  /** set the type of the model */
  void setType(std::string const& type) { type_ = type; }

  /// deletes a parameter if this exists in this model
  void deleteParm(std::string paramName);

  /// deletes a parameter if this exists in this model
  void replaceParmName(std::string paramName, std::string newParamName);

  /// adds a new parameter to the model
  void addParam( TICTAC_SPTR<IP_Param>& p);

  /// returns the arrayf of parameters
  const std::vector < TICTAC_SPTR<IP_Param> >& getParamArray() const { return modelParams_; }

private:

  /** the name of model in lower case stored (if no case sensitive)*/
  std::string name_;

  /** the type of the model stored as string (lower case (if no case sensitive))*/
  std::string type_;

  /** all the specified parameters with their (default) values */
  std::vector < TICTAC_SPTR<IP_Param> > modelParams_;
};

#endif /* IP_MODEL_HPP_ */
