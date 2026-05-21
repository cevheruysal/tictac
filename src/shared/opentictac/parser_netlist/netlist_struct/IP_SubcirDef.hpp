/*
 * IP_SubcirDef.hpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#ifndef IP_SUBCIRDEF_HPP_
#define IP_SUBCIRDEF_HPP_

#include "IP_NetlistStructBase.hpp"
#include "IP_NetlistStructSequence.hpp"
#include "IP_Param.hpp"
#include "IP_Model.hpp"

/** this class represents a subcircuit definition */
class IP_SubcirDef  : public IP_NetlistStructBase
{
public:

  /** Ctor only with the tokens representing the header of the subcircuit */
  IP_SubcirDef(std::vector<IP_Token> &tokens,
      std::string &name,
      std::vector < TICTAC_SPTR<IP_Param> > &subCirParams,
      std::vector < IP_Token > &subCircPorts);

  /** empty Dtor */
  virtual ~IP_SubcirDef() {;}

  /** returns true if this subcricuit is on the top level */
  bool isSubcirTopLevel() const { return isTopLevel_; }

  /** sets the flag for top level, default is false */
  void setTopLevelFlag(bool b) { isTopLevel_ = b; }

  /** returns the body of the subcircuit */
  TICTAC_SPTR<IP_NetlistStructSequence>& getSubcktBody() { return subCirBody_; }
  const TICTAC_SPTR<IP_NetlistStructSequence>& getSubcktBody() const { return subCirBody_; }

  /** sets the body of the subcircuit */
  void setSubcktBody(TICTAC_SPTR<IP_NetlistStructSequence>& subCirBody) {  subCirBody_ = subCirBody; }

  /** returns a parameter of the subcircuit */
  TICTAC_SPTR<IP_Param>& getSubCircuitParams(MYINT i) { return subCirParams_[i]; }
  const TICTAC_SPTR<IP_Param>& getSubCircuitParams(MYINT i) const{ return subCirParams_[i]; }

  //// returns the name of the subcircuit call parameter
  const std::string& getParamName(MYINT i) const { return subCirParams_[i]->getName(); }

  /** returns the name of the subcircuit */
  const std::string& getName() const { return  subCirName_; }

  /** sets the subcircuit name */
  void setName(std::string name) { subCirName_ = name; }

  /** returns the number of pinns/ports*/
  MYINT getNrPorts() const { return (MYINT)subCircPorts_.size(); }

  /** returns the instantiated pinn names */
  const std::string& getPortInstName(MYINT i) const{ return subCircPorts_[i].getValue(); }

  /** returns the port token */
  IP_Token& getPortToken(int i) { return subCircPorts_[i]; }
  const IP_Token& getPortToken(int i) const { return subCircPorts_[i]; }

  /** get the number of parameters */
  MYINT getNrParams() const { return (MYINT)subCirParams_.size(); }

  TICTAC_SPTR<IP_Param>& getParam(MYINT i) { return subCirParams_[i]; }

  /** @returns true if this param is found, false otherwise */
  bool hasParam(const std::string &instParamName) {
    for (MYINT i = 0; i < (MYINT)subCirParams_.size(); i++)
      if (subCirParams_[i]->getName() == instParamName)
        return true;
    return false;
  }

  //
  TICTAC_SPTR<IP_SubcirDef> parent() { return parent_; }
  void parent(TICTAC_SPTR<IP_SubcirDef> parent)
  {
    parent_ = parent;
  }

  std::vector<TICTAC_SPTR<IP_SubcirDef>> children() { return children_; }
  void add_child_subckt(TICTAC_SPTR<IP_SubcirDef> child) { children_.push_back(child); }

  TICTAC_SPTR<IP_SubcirDef> find_subckt(std::string name);

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_SubcirDef &)(*this)); }

  /*
  std::unordered_map< std::string , IP_Param*>&      getLoalParamsMap() { return localParams_; }
  std::unordered_map< std::string , IP_Model*>&      getLocalModelMap() { return localModels_; }
  std::unordered_map< std::string , IP_Function*>&   getLocalFunctionsMap() { return localFunctions_;}
  std::unordered_map< std::string , IP_SubcirDef*>&  getLocalSubcktMap()    { return subcircuitDefs_; }*/

  void add_model(TICTAC_SPTR<IP_Model> model)
  {
    auto ret = models_.try_emplace(model->getName(), model);
    if (!std::get<1>(ret))
      SIM_ERROR_STOP("WARNING: Models already exists!! Do you have a conditional netlist??");
  }

  std::pair<int,TICTAC_SPTR<IP_Model>> find_model(std::string model_name);


  void add_body_param(TICTAC_SPTR<IP_Param> param)
  {
    auto ret = params_body_.try_emplace(param->getName(), param);
    if (!std::get<1>(ret))
      SIM_ERROR_STOP("WARNING: Parameter already exists!! Do you have a conditional netlist??");
  }

  const std::unordered_map< std::string, TICTAC_SPTR<IP_Param> >* get_header_params()
  {
    return &params_header_;
  }

  const std::unordered_map< std::string, TICTAC_SPTR<IP_Param> >* get_body_params()
  {
    return &params_body_;
  }

  /*TICTAC_SPTR<IP_Param> find_param(std::string param_name)
  {
    auto param = params_.find(param_name);
    if (param == params_.end())
      {
        // Let's see if there is a parent parameter available
        if (parent_ != nullptr)
          return parent_->find_param(param_name);
        else
        SIM_ERROR_STOP("Parameter could not be found.");
      }
    else
      {
        return param->second;
      }
  }*/

  void set_instance_level(int instance_level)
  {
    if (instance_level_ == -1)
      instance_level_ = instance_level;
    else
      SIM_ERROR_STOP("This subcircuit - " + subCirName_ + " - has been initialized already: Recursion is not allowed!!!");
  }

  void reset_instance_level()
  {
    instance_level_ = -1;
  }

 private:

  /** flag to signal if this subcircuit is top level */
  bool isTopLevel_;

  /** the name of the defined subcircuit */
  std::string subCirName_;

  /** this is the body of the subcircuit */
  TICTAC_SPTR<IP_NetlistStructSequence> subCirBody_;

  /** array with the parameters of the subcircuit definition */
  std::vector < TICTAC_SPTR<IP_Param> > subCirParams_;

  /** these tokens were recognized as ports by the parser */
  std::vector < IP_Token > subCircPorts_;

  // This makes only sense, if there are no conditional netlists (.IF THEN...)
  TICTAC_SPTR<IP_SubcirDef> parent_ = nullptr;
  std::vector<TICTAC_SPTR<IP_SubcirDef>> children_;

  int instance_level_ = -1; // This is level of the concrete instance of this subcircuit: every xcall increases it!1!

  // DUE TO CONDITIONED NETLISTING THESE CANNOT BE HERE !!!!
  // ==== these informations are set after the initial netlist parser ====
  std::unordered_map< std::string, TICTAC_SPTR<IP_Param> > params_header_;

  // This method is only for internal use
  void add_header_param(TICTAC_SPTR<IP_Param> param)
  {
    auto ret = params_header_.try_emplace(param->getName(), param);
    if (!std::get<1>(ret))
      SIM_ERROR_STOP("ERROR: Parameter already exists!! Do you have a conditional netlist?");
  }
  /// local parameter (not those from the subcircuit header)
  std::unordered_map< std::string, TICTAC_SPTR<IP_Param> > params_body_;
  /// list of local models
  std::unordered_map< std::string, TICTAC_SPTR<IP_Model> > models_;
  /// list of local functions
  // std::unordered_map< std::string , IP_Function*> localFunctions_;
};

#endif /* IP_SUBCIRDEF_HPP_ */
