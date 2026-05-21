/*
 * IP_FillExpansionStack.cpp
 *
 *  Created on: Jan 9, 2017
 *      Author: benk
 */

#include "IP_FillExpansionStack.hpp"

#include "IP_SubcirDef.hpp"

#include "IP_Element.hpp"
#include "IP_Function.hpp"
#include "IP_Model.hpp"
#include "IP_Param.hpp"
#include "IP_SubcirSettings.hpp"
#include "IP_XCall.hpp"
#include "IP_ConditionedNetlist.hpp"
#include "IP_GlobalSettings.hpp"
#include "IP_Option.hpp"
#include "IP_Output.hpp"
#include "IP_Measurement.hpp"
#include "IP_Assertion.hpp"
#include "IP_AnaysisCard.hpp"
#include "IP_NetlistStructSequence.hpp"
#include "IP_Token.hpp"

#include "IP_NetlistExpander.hpp"

void   IP_FillExpansionStack::Visit(IP_SubcirDef &node)
{
  if (levelVisit_ < 1)
    {
      // visit only the body of the subcircuit
      levelVisit_++;
      SIM_PRINT_L5(verb(), "FillExpansionStack: visit subckt " << node.getName() );
      visitNode( node.getSubcktBody().get() );
      levelVisit_--;
    }
  else
    {
      SIM_PRINT_L5(verb(), "FillExpansionStack: add subckt " << node.getName() );
      expansion_stack_->getExpansionLastLevel().addSubcircuitDef(&node);
    }
}
void   IP_FillExpansionStack::Visit(IP_AnaysisCard& node)
{
  // do nothing
  expansion_stack_->getExpansionLastLevel().addAnalysisCard(&node);
}
void   IP_FillExpansionStack::Visit(IP_Element &node)
{
  // do nothing
  expansion_stack_->getExpansionLastLevel().addElem( &node );
}
void   IP_FillExpansionStack::Visit(IP_Model &node)
{
  expansion_stack_->getExpansionLastLevel().addModel(&node);
}
void   IP_FillExpansionStack::Visit(IP_Param &node)
{
  IP_NetlistExpander expand;
  expansion_stack_->getExpansionLastLevel().addLocalParam(&node);
  // for each parameter we insert an expression
  const IP_Token& paramValue = node.getInitExprToken();
  std::string name = node.getName();
  auto expr = expand.parseExpression(&paramValue, expansion_stack_ );
  if (!expand.getExprError())
    expansion_stack_->getExpansionLastLevel().addLocalParamValue( name, expr);
}
void   IP_FillExpansionStack::Visit(IP_XCall &node)
{
  // do nothing
  expansion_stack_->getExpansionLastLevel().addXCall( &node );
}
void   IP_FillExpansionStack::Visit([[maybe_unused]] IP_ConditionedNetlist &node)
{
  // do nothing, do not visit anything
  // TODO: later we might try an evaluation of the condition ...
}
