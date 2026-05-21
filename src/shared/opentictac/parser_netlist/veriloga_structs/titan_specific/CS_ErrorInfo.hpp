/*
 * CS_ErrorInfo.hpp
 *
 *  Created on: Feb 4, 2013
 *      Author: benk
 */

#ifndef CS_ERRORINFO_HPP_
#define CS_ERRORINFO_HPP_

#include "verilog_comp.hpp"

class CS_ModuleDeclaration;

/** This class is required in order to throw an error in the connection with a Verilog context.
 * This abstract class contains the module class and the starting line and column of the Verilog construct */
class CS_ErrorInfo
{
public:

  /** empty Ctor*/
  CS_ErrorInfo();

  /** empty Dtor*/
  virtual ~CS_ErrorInfo() {;}

  /** returns the module */
  virtual const CS_ModuleDeclaration* getModule() const = 0;

  /** line nr in the Verilog file*/
  virtual int getStartLine() const = 0;

  /** column number in the Verilog file */
  virtual int getStartColumn() const = 0;
};

#endif /* CS_ERRORINFO_HPP_ */
