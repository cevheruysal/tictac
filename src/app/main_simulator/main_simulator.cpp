/*
 * main_parser.cpp
 *
 *  Created on: Oct 30, 2014
 *      Author: benk
 */

#include "sim_utils.hpp"
#include "IP_ParserBase.hpp"
#include "IP_VerboseVisitor.hpp"
#include "IP_NetlistExpander.hpp"

int main(int argc, char** argv)
{
  //
  if (argc < 2)
    {
      std::cout << " Not enough arguments, please specify netlist file \n";
      return 1;
    }

  // this is the file to parse
  std::string myfile(argv[1]);
  std::cout << " Parsing the file: " << myfile << "\n";

  //UT_SIMVerboseLevelHandle::setGlobalVerbose(6);

  // create the parser
  IP_ParserBase parser;
  IP_VerboseVisitor visitor;
  IP_NetlistExpander netlistExpander;

  // start the parser
  parser.parseFile(myfile);

  // make a test tree transition
  visitor.startTopLevelVisit( parser.getTopLevel() );

  // start the netlist expansion
  netlistExpander.doNetlistExpansion( parser.getTopLevel() );

  // start the simulations/analysis
  netlistExpander.getTicTacComputer()->executeRequiredAnalyses();

}


