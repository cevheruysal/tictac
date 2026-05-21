/*
 * IP_DummyModule.hpp
 *
 *  Created on: Feb 27, 2015
 *      Author: benk
 */

#ifndef IP_DUMMYMODULE_HPP_
#define IP_DUMMYMODULE_HPP_

#include "sim_utils.hpp"
#include "CS_ModuleDeclaration.hpp"

/** this class is just there to model the CS_ModuleDeclaration
 * auch that the file informations van also passed through the Verilog-A structures*/
class IP_DummyModule : public CS_ModuleDeclaration
{
public:
  /** just a simple constructor with file name
   * other informations are not relevant */
  IP_DummyModule(std::string fileName) : CS_ModuleDeclaration("", fileName,0, 0) { ; }

  /** IMPORTANT empty Dtor */
  virtual ~IP_DummyModule() {;}
};

#endif /* IP_DUMMYMODULE_HPP_ */
