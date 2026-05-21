#include "IP_NameSearchVisitor.hpp"
#include "IP_XCall.hpp"
#include "IP_Element.hpp"

#include <algorithm>
#include <string>

foundCharacteristics::foundCharacteristics(std::string const &name, int const &nrPins = 2)
    : name_(name), nrPins_(nrPins)
{
}

std::string
    foundCharacteristics::getName()
{
  return name_;
}

int
    foundCharacteristics::getNrPins()
{
  return nrPins_;
}

IP_NameSearchVisitor::IP_NameSearchVisitor(std::string const &searchString)
    : searchString_(searchString)
{
  std::transform(searchString_.begin(), searchString_.end(), searchString_.begin(), ::tolower);
};

void
    IP_NameSearchVisitor::Visit(IP_XCall &node)
{
  auto fullName = node.getInstantionationXname();
  // std::cout << "start x visitor " << fullName << '\t' << getCroppedString(fullName) << std::endl;
  if (getCroppedString(fullName) == searchString_)
    foundOccurences_.push_back(foundCharacteristics(fullName,node.getNrPinns()));
}

void
    IP_NameSearchVisitor::Visit(IP_Element &node)
{
  auto fullName = node.getInstName();
  /*
   * std::cout << "start element visitor " << fullName << '\t' << getCroppedString(fullName)
   *           << std::endl;
   */
  if (getCroppedString(fullName) == searchString_)
    foundOccurences_.push_back(foundCharacteristics(fullName));
}

std::vector<foundCharacteristics>
    IP_NameSearchVisitor::getResults()
{
  return foundOccurences_;
}


std::string
    getCroppedString(std::string const &fullName)
{
  auto npos = fullName.find(std::string("$"));
  std::string outName;
  if (npos == std::string::npos)
    {
      outName = fullName;
    }
  else
    {
      outName = fullName.substr(npos + 1, std::string::npos);
    }

  /*
   * to lower case
   */
  std::string o("");
  std::transform(outName.begin(), outName.end(), o.begin(), ::tolower);
  return outName;
}
