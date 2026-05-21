/*
 * CS_VerilogModuleCollection.hpp
 *
 *  Created on: Nov 27, 2012
 *      Author: benk
 */

#ifndef CS_VERILOGMODULECOLLECTION_HPP_
#define CS_VERILOGMODULECOLLECTION_HPP_

#include "CS_ModuleDeclaration.hpp"

/** This class is at the top level of the parsing result.
 * Contains all the modules that the user wants to compile
 * at it should say at the disposal of the simulator.
 *
 * This class has no LRM correspondence, it only helps to collect all modules that
 * need to be compiled */
class CS_VerilogModuleCollection
{
public:

  CS_VerilogModuleCollection();

  virtual ~CS_VerilogModuleCollection() {;}

  /** adds one module to the module collection */
  void addModule( CS_ModuleDeclaration* newModule) {
    modules_.push_back(newModule);
    nrModules_++;
  }

  /** looks for the module with the specified name and returns the pointer to it.
   * If the module is not found the it returns NULL.*/
  CS_ModuleDeclaration* getModule( std::string name ){
    CS_ModuleDeclaration* ret = 0;
    // look for the module with the specified name
    for (int i = 0 ; i < nrModules_ ; i++){
        if ( name == modules_[i]->getName() ){
            ret = modules_[i];
        }
    }
    return ret;
  }

  /** returns the module at the given index. If the index is larger than
   * the actual vector size then it returns NULL*/
  CS_ModuleDeclaration* getModule(int index){
    if (index >= nrModules_ ) return 0;
    else return modules_[index];
  }

  /** returns the number of modules in this collection */
  int getNrModules() { return nrModules_; }

private:

  /** number of stored modules */
  int nrModules_;

  /** vector containing all the modules */
  std::vector<CS_ModuleDeclaration*> modules_;

};

#endif /* CS_VERILOGMODULECOLLECTION_HPP_ */
