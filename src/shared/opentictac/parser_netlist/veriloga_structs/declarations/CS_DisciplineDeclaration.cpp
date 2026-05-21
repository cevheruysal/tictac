/*
 * CS_DisciplineDeclaration.cpp
 *
 *  Created on: Nov 21, 2015
 *      Author: benk
 */

#include "CS_DisciplineDeclaration.hpp"
#include "CS_ModuleDeclaration.hpp"

CS_DisciplineDeclaration::CS_DisciplineDeclaration(std::string disciplineName,
    std::string filename, int sR, int sC)
{
  disciplineName_ = disciplineName;
  filename_ = filename;
  startR_ = sR;
  startC_ = sC;
  dummyModule_ = MY_SMPO<const CS_ModuleDeclaration>(new CS_ModuleDeclaration(
        "dummyModuleJustForErrorMessage", filename, sR, sR));
  isContinuous_ = true;
}

const CS_ModuleDeclaration* CS_DisciplineDeclaration::getModule() const
{
  return dummyModule_.get();
}


void CS_DisciplineDeclaration::addFlowAtributes(std::string atribName, std::string atribValue)
{
  if (hasFlowNature()) {
      std::map< std::string, std::string >::iterator iter = flowAttributes_.find(atribName);
      if (iter != flowAttributes_.end())
        flowAttributes_.insert( std::pair<std::string, std::string>(atribName, atribValue) );
  }
}

void CS_DisciplineDeclaration::addPotentialAtributes(std::string atribName, std::string atribValue)
{
  if (hasPotentialNature()) {
      std::map< std::string, std::string >::iterator iter = potentialAttributes_.find(atribName);
      if (iter != potentialAttributes_.end())
        potentialAttributes_.insert( std::pair<std::string, std::string>(atribName, atribValue) );
  }
}


std::string CS_DisciplineDeclaration::getFlowAtributes(std::string atribName) const
{
  if (hasPotentialNature()) {
      if (potentialAttributes_.find(atribName) != potentialAttributes_.end())
        return (potentialAttributes_.find(atribName))->second;
      else
        return potentialNature_->getAttribute(atribName);
  }
  else
    return std::string("");
}

std::string CS_DisciplineDeclaration::getPotentialAtributes(std::string atribName) const
{
  if (hasFlowNature()) {
      if (flowAttributes_.find(atribName) != flowAttributes_.end())
        return (flowAttributes_.find(atribName))->second;
      else
        return potentialNature_->getAttribute(atribName);
  }
  else
    return std::string("");
}
