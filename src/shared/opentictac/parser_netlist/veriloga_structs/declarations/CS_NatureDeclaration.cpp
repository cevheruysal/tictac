/*
 * CS_NatureDeclaration.cpp
 *
 *  Created on: Nov 21, 2015
 *      Author: benk
 */

#include "CS_NatureDeclaration.hpp"
#include "CS_ModuleDeclaration.hpp"
#include <boost/lexical_cast.hpp>

CS_NatureDeclaration::CS_NatureDeclaration(std::string natureName,
    std::string filename, int sR, int sC)
{
  natureName_ = natureName;
  filename_ = filename;
  startR_ = sR;
  startC_ = sC;
  dummyModule_ = MY_SMPO<const CS_ModuleDeclaration>(new CS_ModuleDeclaration(
      "dummyModuleJustForErrorMessage", filename, sR, sR));

  abstol_ = 1E-14; // default high abstol
}

const CS_ModuleDeclaration* CS_NatureDeclaration::getModule() const
{
  return dummyModule_.get();
}

void CS_NatureDeclaration::addAttribute(std::string attribName,std::string attribValue)
{
  if (attribName == "abstol") {
      try {
          this->setAbsTol( boost::lexical_cast<double>(attribValue));
      }
      catch (std::exception& e) {
          std::cout << "ERROR:\"" << attribValue << "\"\n";
      }
  }
  else if (attribName == "access")
    this->setAccess(attribValue);
  else if (attribName == "ddt_nature")
    this->setDDTNature(attribValue);
  else if (attribName == "idt_nature")
    this->setIDTNature(attribValue);
  else if (attribName == "units")
    this->setUnits(attribValue);
  else {
      furtherAttributes_.insert(std::pair<std::string, std::string>(attribName, attribValue));
  }
}

std::string CS_NatureDeclaration::getAttribute(std::string attribName) const
{
  if (attribName == "abstol")
      return std::string( boost::lexical_cast<std::string>(getAbsTol()) );
    else if (attribName == "access")
      return getAccess();
    else if (attribName == "ddt_nature")
      return getDDTNature();
    else if (attribName == "idt_nature")
      return getIDTNature();
    else if (attribName == "units")
      return getUnits();
    else {
        if (furtherAttributes_.find(attribName) != furtherAttributes_.end())
          return (furtherAttributes_.find(attribName))->second;
        else
          return std::string("");
    }
}
