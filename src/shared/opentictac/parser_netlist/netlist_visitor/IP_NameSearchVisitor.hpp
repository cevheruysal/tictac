#pragma once
#include "IP_NetlistStructBase.hpp"

#include <string>
#include <vector>

class foundCharacteristics
{
    public:
     foundCharacteristics(std::string const& name, int const& nrPins);

     std::string getName();
     int         getNrPins();

    private:
     std::string name_;
     int         nrPins_;
};

class IP_NameSearchVisitor : public IP_NetlistVisitBase
{
 public:
  /**
   * \brief constructor taking the name to search for
   */
  IP_NameSearchVisitor(std::string const& search);

  void Visit(IP_XCall& node) override;
  void Visit(IP_Element& node) override;

  std::vector<foundCharacteristics> getResults();

 private:
  std::vector<foundCharacteristics> foundOccurences_;  ///< vector holding all hits
  std::string                       searchString_;     ///< string to search for
};


std::string
    getCroppedString(std::string const& in);
