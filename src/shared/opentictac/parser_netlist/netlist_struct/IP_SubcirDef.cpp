/*
 * IP_SubcirDef.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_SubcirDef.hpp"

IP_SubcirDef::IP_SubcirDef(std::vector<IP_Token> &tokens,
    std::string &name,
    std::vector < TICTAC_SPTR<IP_Param> > &subCirParams,
    std::vector < IP_Token > &subCircPorts)
: IP_NetlistStructBase(tokens)
{
  // default value is false
  isTopLevel_ = false;
  // initialize also the subcircuit body
  // subcktBody_ = TICTAC_SPTR<IP_NetlistStructSequence>(new IP_NetlistStructSequence(tokens));

  subCirName_   = name;
  subCirParams_ = subCirParams;
  subCircPorts_ = subCircPorts;

  // Also put the params into an unordered_map.
  for(const auto& elem : subCirParams_)
    add_header_param(elem);
}

TICTAC_SPTR<IP_SubcirDef> IP_SubcirDef::find_subckt(std::string name)
{
  std::string lowered_name = sim_toLowerString(name);
  // TODO: this might be strict, since nested subcircuits with identical names might be ok
  if (lowered_name == this->getName())
    SIM_ERROR_STOP("Recursive subcircuit calls are not allowed");
  TICTAC_SPTR<IP_SubcirDef> found_subckt = nullptr;
  IP_SubcirDef* current_parent_subckt = this;

  do
    {
      for (const auto& subckt : current_parent_subckt->children())
      {
        if (lowered_name == subckt->getName())
          {
            if (found_subckt == nullptr)
              found_subckt = subckt;
            else
              SIM_ERROR_STOP("Found several subcircuits matching this call");
          }
      }
      if (found_subckt != nullptr)
        return found_subckt;
      else
        current_parent_subckt = current_parent_subckt->parent_.get();
    }
  while  (current_parent_subckt != nullptr);

  return found_subckt;
}

std::pair<int,TICTAC_SPTR<IP_Model>> IP_SubcirDef::find_model(std::string model_name)
{
  auto model = models_.find(model_name);
  if (model == models_.end())
    {
      // Let's see if there is a parent model available
      if (parent_ != nullptr)
        return parent_->find_model(model_name);
      else
        SIM_ERROR_STOP(std::string("Model " + model_name + " could not be found"));
    }
  else
    return std::make_pair(this->instance_level_, model->second);
}