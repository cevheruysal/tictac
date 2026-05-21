/*
 * CS_AttributeInstance.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: benk
 */

#include "CS_AttributeInstance.hpp"
#include "CS_ExpressionBasis.hpp"

//#include "VT_EvalVisitor.hpp"

CS_AttributeInstance::CS_AttributeInstance(): UT_VerboseClass("AttributeInstance")
{
  // create the map only when somebody tries to add to it
  myMapp_ = 0;
}

void CS_AttributeInstance::addAttribute(std::string key, CS_ExpressionBasis *value )
{
  // if key is zero then return
  if ( value == 0 ) return;

  // create the map if it was not created previously
  if ( myMapp_ == 0 )
    myMapp_ = new std::map<std::string,CS_ExpressionBasis*>();

  //VERILOG_PRINT_L4(verb(), <<  "\n ADD Key:" << key << " expr:" << VT_EvalVisitor::evalExpr(value, GET_STRING) );
  // create a pair of values and then add it to the map
  //std::pair<std::string, CS_ExpressionBasis*> pair(key, value);
  myMapp_->insert(std::pair<std::string, CS_ExpressionBasis*>(key, value));
}

CS_ExpressionBasis* CS_AttributeInstance::getAttribute(std::string key) const
{
  // if the map is not created then return null
  if ( myMapp_ == 0 )
    return 0;

  std::map< std::string, CS_ExpressionBasis* >::iterator iter = myMapp_->find(key);
  // if the map is created then return the value
  //VERILOG_PRINT_L4(verb(), "\nGet Key:" << key << " found:" << (iter != myMapp_->end())
  //<< " size:" << myMapp_->size() ); //" expr:" << VT_EvalVisitor::evalExpr(value, GET_STRING) );
  if (iter != myMapp_->end())
    return iter->second;
  else
    return 0;
}
