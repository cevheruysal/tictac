/*
 * CS_Identifier.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: benk
 */

#ifndef CS_IDENTIFIER_HPP_
#define CS_IDENTIFIER_HPP_

#include "verilog_comp.hpp"
#include "CS_GenerateInfo.hpp"


/** virtual super class for all entities that during run-time might have a
 * dynamic value (ports, parameters, variables)
 * represents in LRM all the identifiers (port_identifier, parameter_identifier, variable_identifier, ...)
 * that are not seen as constants
 * This class is an abstract class and the functionality is put in the classes:
 * port_identifier, parameter_identifier, variable_identifier*/
class CS_Identifier : public CS_GenerateInfo, public UT_VerboseClass
{

public:

  CS_Identifier() : UT_VerboseClass("Identifier") { }

  virtual ~CS_Identifier() {;}

  /** returns the string of the actual entity (e.g., the corresponding C variable of a Verilog parameter) */
  virtual std::string getGenerate([[maybe_unused]] const VarType resType) const { return "";}

  /** returns the class of the value (Port,Variable or Parameter) */
  virtual VariableClass getClass() const = 0;

  /** returns the name of the port/parameter/variable */
  virtual const std::string& getName() const = 0;

  /** the type of the value */
  virtual VarType getType() const = 0;

  /** returns the size of the variable (used mostly only for arrays) */
  virtual int getSize() const = 0;

  /** returns the offset of an array that is specified in the declaration, */
  virtual int getOffset() const = 0;

  /** this function shows if the pins are numbered in reverse order */
  virtual bool isInReversedOrder() const = 0;

  /** @returns true if this is an array, false otherwise */
  virtual bool isArray() const = 0;

  /** for correct and unified array indexing this function should be used
   * this function. This maps the Verilog-A index to C index
   * @returns integer C index */
  virtual int getCorrectArrayIndex_int(int index) const {
    if ( this->isInReversedOrder() )
      // reverse numbering
      return (this->getSize() -1  + this->getOffset() - index );
    else
      // normal numbering
      return  (index - this->getOffset());
  }

  /** for correct and unified array indexing this function should be used
   * this function. This maps the Verilog-A index to C index
   * @returns string C index  */
  virtual std::string getCorrectArrayIndex(int index) const {
    if ( this->isInReversedOrder() )
      // reverse numbering
      return verilog_integerToString(getSize() - 1  + getOffset() - index );
    else
      // normal numbering
      return  verilog_integerToString(index - this->getOffset());
  }

  /** for correct and unified array indexing this function should be used
   * this function. This maps the C index to Verilog-A index
   * @returns string C index  */
  virtual std::string getCorrectArrayIndex(std::string index) const {
    if ( this->isInReversedOrder() )
      // reverse numbering
      return verilog_integerToString(getSize()-1 + getOffset()) + " - " + index;
    else
      // normal numbering ( - 0 could be saved in order to avoid unnecessary code)
      if (this->getOffset() != 0)
        return  index + " - " + verilog_integerToString(this->getOffset());
      else
        return  index;
  }

  /** for correct and unified array indexing this function should be used
   * this function. This maps the C index to Verilog-A index
   * @returns string Verilog-A index  */
  virtual std::string getCorrectVerilogArrayIndex(int index) const {
    if ( this->isInReversedOrder() )
      // reverse numbering
      return verilog_integerToString( getSize()-1 + getOffset() - index );
    else
      // normal numbering
      return  verilog_integerToString(this->getOffset() + index);
  }

};


#endif /* CS_IDENTIFIER_HPP_ */
