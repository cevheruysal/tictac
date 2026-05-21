/*
 * CS_AttributeInstance.hpp
 *
 *  Created on: Jan 31, 2013
 *      Author: benk
 */

#ifndef CS_ATTRIBUTEINSTANCE_HPP_
#define CS_ATTRIBUTEINSTANCE_HPP_

#include "verilog_comp.hpp"

// forward declaration of the Expression basis
class CS_ExpressionBasis;

/** this class represents the list of attribute instances for one declaration or
 * statement in a Verilog file.
 * This feature is however currently only used for variable and parameter declaration.
 * Later, if we want so, we could use
 *
 * see "attribute_instance" in LRM*/
class CS_AttributeInstance : public UT_VerboseClass
{
public:
  /** Ctor */
  CS_AttributeInstance();

  /** Dtor */
  virtual ~CS_AttributeInstance() { if (myMapp_) delete myMapp_; }

  /** returns the number of attributes */
  int getNrAtributes() const { return (myMapp_ != 0) ? ((int)myMapp_->size()) : 0; }

  /** adds a new attribute to the list of attributes */
  void addAttribute(std::string key, CS_ExpressionBasis *value );

  /** returns an expression if the key exist in the map,
   * it returns NULL if not present */
  CS_ExpressionBasis* getAttribute(std::string key) const;

private:

  /** map of the attribute list */
  std::map< std::string, CS_ExpressionBasis* > *myMapp_;

};

#endif /* CS_ATTRIBUTEINSTANCE_HPP_ */
