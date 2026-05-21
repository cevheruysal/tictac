/*
 * IP_VerboseVisitor.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: benk
 */

#include "IP_VerboseVisitor.hpp"

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

#define MY_VERB_OUT(X) { std::cout << tabs_ << X; };

IP_VerboseVisitor::IP_VerboseVisitor() {
  tabs_ = "";
  // nothing to do here
}

void   IP_VerboseVisitor::startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit)
{
  MY_VERB_OUT("Start visiting " << toplevelToVisit->getName() << "\n" );
  tabs_ = "";
  IP_NetlistVisitBase::startTopLevelVisit(toplevelToVisit);
  MY_VERB_OUT("End visiting" << toplevelToVisit->getName() << "\n" );
}

void   IP_VerboseVisitor::Visit(IP_SubcirDef &node)
{
  MY_VERB_OUT("Subcircuit " << node.getName() << "\n" );

  MY_VERB_OUT("Pinns: ");
  for(int i = 0; i < node.getNrPorts() ; i++ )
    MY_VERB_OUT( node.getPortToken(i).getValue() << " , ");
  MY_VERB_OUT(" \n");

  MY_VERB_OUT("Params: ");
  for(int i = 0; i < node.getNrParams() ; i++ )
      visitNode( node.getSubCircuitParams(i).get() );
  MY_VERB_OUT("\n");

  std::string tmp = tabs_;
  tabs_ += "  ";
  // visit the body of the subcircuit
   visitNode( node.getSubcktBody().get() );
  // store back the tab string
  tabs_ = tmp;
}

void   IP_VerboseVisitor::Visit(IP_Element &node)
{
  MY_VERB_OUT("Element(device): " << node.getElemType() << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Function &node)
{
  MY_VERB_OUT("Function: " << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Model &node)
{
  MY_VERB_OUT("Model: name=" << node.getName() << " type=" << node.getType() << " " << toMyString(node) << "\n");
  VisitChildren(node);
  MY_VERB_OUT("\n");
}
void   IP_VerboseVisitor::Visit(IP_Param &node)
{
  MY_VERB_OUT("P(" << node.getName() << "|" << node.getInitExprToken().getValue() << ")");
  //<< ")(" << *(node.getToken(0).getFileName()) << ")");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_SubcirSettings &node)
{
  MY_VERB_OUT("SubcircuitSetting:" << node.getTypeString(node.getType() ) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_XCall &node)
{
  MY_VERB_OUT("X-call of " << node.getSubcktName() << " nr.Pinns=" << node.getNrPinns()
      << " " << toMyString(node) << " nrParam=" << node.getNrParams() << "\n");
  MY_VERB_OUT(" Xcall Params: ");
  for(MYINT i = 0; i < node.getNrParams() ; ++i )
      visitNode( node.getCalledParam(i).get() );
  MY_VERB_OUT("\n");
}
void   IP_VerboseVisitor::Visit(IP_ConditionedNetlist &node)
{
  MY_VERB_OUT("Conditioned netlist, expr=" << node.getConditionToken().getValue() << "\n");
  std::string tmp = tabs_;
  tabs_ += "  ";
  visitNode( node.getTrueBranch().get() );
  if (node.hasFalseBranch()) {
      tabs_ = tmp;
      MY_VERB_OUT("Conditioned netlist FALSE:\n");
      tabs_ += "  ";
      visitNode( node.getFalseBranch().get() );
  }
  tabs_ = tmp;
}
void   IP_VerboseVisitor::Visit(IP_GlobalSettings &node)
{
  MY_VERB_OUT("Global Setting:" << node.getTypeString(node.getGlobalSettingType() )
      << " " << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Option &node)
{
  MY_VERB_OUT("Option:" << node.getOptionName() << " " << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Output &node)
{
  MY_VERB_OUT("Output:" << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Measurement &node)
{
  MY_VERB_OUT("Measurement:" << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_Assertion &node)
{
  MY_VERB_OUT("Assretion:" << toMyString(node) << "\n");
  VisitChildren(node);
}
void   IP_VerboseVisitor::Visit(IP_AnaysisCard &node)
{
  MY_VERB_OUT("analysis:" << node.getAnalysisType() << " " << toMyString(node) << "\n");
  VisitChildren(node);
}

std::string IP_VerboseVisitor::toMyString(const IP_NetlistStructBase &node)
{
  std::string ret = "\"" + node.getToken(0).getValue();
  for (int i = 1; i < node.getNrTokens(); i++)
    ret += "," + node.getToken(i).getValue();
  ret += "\"";
  //return the string
  return ret;
}
