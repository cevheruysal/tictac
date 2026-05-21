/*
 * IP_Model.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: benk
 */

#include "IP_Model.hpp"

IP_Model::IP_Model(std::vector<IP_Token> &tokens, std::string &name,
    std::string &type, std::vector < TICTAC_SPTR<IP_Param> > &modelParams)
: IP_NetlistStructBase(tokens), name_(name), type_(type), modelParams_(modelParams)
{
  // all is set
}

IP_Model::IP_Model(std::vector<IP_Token> &tokens, const std::string &&name,
                   const std::string &&type, std::vector < TICTAC_SPTR<IP_Param> > &modelParams)
    : IP_NetlistStructBase(tokens), name_(name), type_(type), modelParams_(modelParams)
{
  // all is set
}

IP_Model::IP_Model(std::vector<IP_Token> &tokens, const std::string &name,
                   const std::string &&type, std::vector < TICTAC_SPTR<IP_Param> > &modelParams)
    : IP_NetlistStructBase(tokens), name_(name), type_(type), modelParams_(modelParams)
{
  // all is set
}

IP_Model::IP_Model( const IP_Model &m) : IP_NetlistStructBase( m.getTokens() )
{
  name_ = m.name_;
  type_ = m.type_;
  // it is important to have a deep copy
  for (MYINT i = 0; i < (MYINT)m.modelParams_.size(); ++i)
    modelParams_.push_back(  TICTAC_SPTR<IP_Param>(new IP_Param( *(m.modelParams_[i].get()) ) ) );
}

void IP_Model::deleteParm(std::string paramName)
{
  MYINT i;
  for (i = 0; i < (MYINT)modelParams_.size(); ++i)
    {
      // if the parameter match than we delete this
      if (modelParams_[i]->getName() == paramName)
        {
          modelParams_.erase( modelParams_.begin() + i);
          i--;
        }
    }
}

void IP_Model::replaceParmName(std::string paramName, std::string newParamName)
{
  MYINT i;
  for (i = 0; i < (MYINT)modelParams_.size(); ++i)
    {
      // if the parameter match than we delete this
      if (modelParams_[i]->getName() == paramName)
        {
          modelParams_[i]->setName(newParamName);
          break;
        }
    }
}

const IP_Param* IP_Model::getModelParam(std::string paramName, bool caseSens) const
{
  MYINT i;
  for (i = 0; i < (MYINT)modelParams_.size(); ++i)
    {
      // if the parameter match than we delete this
      if (caseSens && modelParams_[i]->getName() == paramName)
        return modelParams_[i].get();
      if (!caseSens
          && sim_toLowerString(modelParams_[i]->getName()) == sim_toLowerString(paramName))
        return modelParams_[i].get();
    }
  return NULL;
}

void IP_Model::addParam( TICTAC_SPTR<IP_Param>& p)
{
  modelParams_.push_back(p);
}
