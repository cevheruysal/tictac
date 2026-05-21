/*
 * CS_AKDependency.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: benk
 */

#include "CS_AKDependency.hpp"

CS_AKDependency::CS_AKDependency(): UT_VerboseClass("AKDependency")
{
  // generate empty dependency list
  sysVars_.resize(0);
  typeDeps_.resize(0);
  nrElem_ = 0;
  this->setVerb(0);
}

CS_AKDependency::CS_AKDependency(const CS_AKDependency &targetEl) : UT_VerboseClass("AKDependency")
{
  // copy the list from the target
  nrElem_   = targetEl.nrElem_;
  sysVars_  = targetEl.sysVars_;
  typeDeps_ = targetEl.typeDeps_;
}

CS_AKDependency::CS_AKDependency(const CS_AKDependency *targetEl) : UT_VerboseClass("AKDependency")
{
  // copy the list from the target
  if ( targetEl != 0) {
      nrElem_   = targetEl->nrElem_;
      sysVars_  = targetEl->sysVars_;
      typeDeps_ = targetEl->typeDeps_;
  }
  else {
      sysVars_.resize(0);
      typeDeps_.resize(0);
      nrElem_ = 0;
  }
}

CS_AKDependency& CS_AKDependency::operator=(const CS_AKDependency& other)
{
  if(&other == this)
    return *this;

  nrElem_   = other.nrElem_;
  sysVars_  = other.sysVars_;
  typeDeps_ = other.typeDeps_;
  return *this;
}
void CS_AKDependency::addSystemVarIndex( int sysVarIndex, int dependencyType )
{
  // check first if the dependency already exists
  int foundPosition = this->isSysVarIndexPresent(sysVarIndex);
  // if found then only update the type of the dependency
  if (foundPosition >= 0)
    {
      // update the dependency type
      this->typeDeps_[foundPosition] =
          (this->typeDeps_[foundPosition] > dependencyType) ? this->typeDeps_[foundPosition] : dependencyType;
    }
  else
    {
      VERILOG_PRINT_L5( verb() , "Add Var:" << sysVarIndex << ", deptype:" << dependencyType );
      this->sysVars_.push_back(sysVarIndex);
      this->typeDeps_.push_back(dependencyType);
      this->nrElem_++;
    }
}

int CS_AKDependency::isSysVarIndexPresent(int sysVarIndex) const
{
  int ret_val = -1;
  for (int j = 0 ; j < this->nrElem_ ; j++)
    { // test each element in the arrray
      if ( this->sysVars_[j] == sysVarIndex)
        {
          ret_val = j;
        }
    }
  // return the found/not found index
  return ret_val;
}

void CS_AKDependency::uniteDependencies( CS_AKDependency &targetEl )
{
  for (int i = 0 ; i < targetEl.nrElem_ ; i++)
    { // for each element we try to add to the existing one
      this->addSystemVarIndex( targetEl.sysVars_[i], targetEl.typeDeps_[i]);
    }
}

std::string CS_AKDependency::toString() const {
  std::string retStr = "";
  for (int i = 0; i < this->nrElem_; i++)
    {
    if (i==0)
      retStr = verilog_integerToString(sysVars_[i]);
    else
      retStr = retStr + "," + verilog_integerToString(sysVars_[i]);
    }
  return "{" + retStr + "}";
}
