/*
 * IP_SubcktCopyVisitor.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: benk
 */

#include "IP_SubcktCopyVisitor.hpp"

#include "IP_SubcirDef.hpp"

#include "IP_NetlistStructBase.hpp"
#include "IP_ControlBase.hpp"

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

IP_SubcktCopyVisitor::IP_SubcktCopyVisitor()
{
    //     nothing to do yet
}

void   IP_SubcktCopyVisitor::startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit)
{
  last_added_ = nullptr;
  IP_NetlistVisitBase::startTopLevelVisit(toplevelToVisit);
}

void IP_SubcktCopyVisitor::getTokenSeqence(IP_NetlistStructBase* elem)
{
  MYINT nrT = elem->getNrTokens(), i;
  tokens_.clear();
  tokens_.resize(nrT);
  for (i = 0; i < nrT; i++)
    tokens_[i] = elem->getToken(i);
  // end
}

void   IP_SubcktCopyVisitor::Visit(IP_NetlistStructSequence &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_NetlistStructSequence> seq = TICTAC_SPTR<IP_NetlistStructSequence>(
      new IP_NetlistStructSequence(tokens_) );
  //std::cout << " Start SUBCKT \n";
  for (MYINT i = 0; i < node.getSequenceLength(); ++i)
    {
      visitNode( node.getNetlistStruct(i).get() );
      // add this if there is a new element, otherwise just skip it
      if ( seq->getLastElementPointer() != ret_.get() && last_added_ != ret_)
        {
          //std::cout << " add: " << ret_->getToken(0).getValue() << "\n";
          seq->addNetlistStruct(ret_);
          last_added_ = ret_;
        }
    }
  seq_ = seq;
  ret_ = seq_;
}

void   IP_SubcktCopyVisitor::Visit(IP_SubcirDef &node)
{
  getTokenSeqence(&node);
  std::string name = node.getName();
  std::vector < TICTAC_SPTR<IP_Param> > subCirParams;
  std::vector < IP_Token > subCircPorts;
  // gather the parameters
  for (MYINT i = 0; i < node.getNrParams(); ++i) {
      TICTAC_SPTR<IP_Param> p = node.getSubCircuitParams(i);
      subCirParams.push_back( p );
  }
  // gather the pinn names
  for (MYINT i = 0; i < node.getNrPorts() ; ++i) {
      IP_Token t = node.getPortToken(i);
      subCircPorts.push_back( t );
  }
  // create the subcircuit
  TICTAC_SPTR<IP_SubcirDef> resultedSubckt =  TICTAC_SPTR<IP_SubcirDef>(
      new IP_SubcirDef(tokens_, name, subCirParams, subCircPorts) );
  resultedSubckt->setTopLevelFlag(node.isSubcirTopLevel());

  // Backup because we go on to body
  TICTAC_SPTR<IP_SubcirDef> old_parentSubckt = parentSubckt_;
  parentSubckt_ = currentSubckt_;
  currentSubckt_ = resultedSubckt;

  // Setup family relations
  currentSubckt_->parent(parentSubckt_);
  if (parentSubckt_ != nullptr)
    parentSubckt_->add_child_subckt(currentSubckt_);

  // visit the body
  visitNode(node.getSubcktBody().get());

  // Restore "family relations"
  currentSubckt_ = parentSubckt_;
  parentSubckt_ = old_parentSubckt;

  // add the resulted sequence
  resultedSubckt->setSubcktBody(seq_);
  resultedSubckt_ = resultedSubckt;
  // the returned element
  ret_ = resultedSubckt_;
}
void   IP_SubcktCopyVisitor::Visit(IP_Element &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Element> elem =  TICTAC_SPTR<IP_Element>( new IP_Element(tokens_, node.getElemType()) );
  ret_ = elem;
}
void   IP_SubcktCopyVisitor::Visit(IP_Function &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Function> func =  TICTAC_SPTR<IP_Function>( new IP_Function(tokens_) );
  ret_ = func;
}
void   IP_SubcktCopyVisitor::Visit(IP_Model &node)
{
  getTokenSeqence(&node);
  // array of parameters
  std::vector < TICTAC_SPTR<IP_Param> > modelParams;
  std::string name = node.getName();
  std::string type = node.getType();
  for (MYINT i = 0; i < node.getNrModelParams(); ++i)
    modelParams.push_back( node.getModelParam(i) );
  // create the model
  TICTAC_SPTR<IP_Model> model =  TICTAC_SPTR<IP_Model>(
      new IP_Model(tokens_, name, type, modelParams) );
  currentSubckt_->add_model(model);
  ret_ = model;
}
void   IP_SubcktCopyVisitor::Visit(IP_Param &node)
{
  getTokenSeqence(&node);
  // create the param
  TICTAC_SPTR<IP_Param> param =  TICTAC_SPTR<IP_Param>( new IP_Param(tokens_) );
  currentSubckt_->add_body_param(param);
  ret_ = param;
}
void   IP_SubcktCopyVisitor::Visit(IP_SubcirSettings &node)
{
  getTokenSeqence(&node);
  // create the settings, which can appear within subcircuit settings
  TICTAC_SPTR<IP_SubcirSettings> subsetting =  TICTAC_SPTR<IP_SubcirSettings>(
      new IP_SubcirSettings(tokens_, node.getType()) );
  ret_ = subsetting;
}
void   IP_SubcktCopyVisitor::Visit(IP_XCall &node)
{
  getTokenSeqence(&node);

  std::vector < TICTAC_SPTR<IP_Param> > callParams;
  std::string subcktName = node.getSubcktName();

  for (MYINT i = 0; i < node.getNrParams(); ++i)
    callParams.push_back( node.getCalledParam(i) );

  // create the x-calls
  auto xcall =  TICTAC_SPTR<IP_XCall>(
      new IP_XCall(tokens_, node.getNrPinns(), subcktName, callParams,
          node.getPinNameBinding()) );
  ret_ = xcall;
}
void   IP_SubcktCopyVisitor::Visit(IP_ConditionedNetlist &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_ConditionedNetlist> condNetlist
      = TICTAC_SPTR<IP_ConditionedNetlist>( new IP_ConditionedNetlist(tokens_) );

  visitNode( node.getTrueBranch().get() );
  condNetlist->setThanBranch(seq_);
  if (node.hasFalseBranch()) {
      visitNode( node.getFalseBranch().get() );
      condNetlist->setFalseBranch(seq_);
  }
  ret_ = condNetlist;
}
void   IP_SubcktCopyVisitor::Visit(IP_GlobalSettings &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_GlobalSettings> globSetting
      = TICTAC_SPTR<IP_GlobalSettings>( new IP_GlobalSettings(tokens_, node.getGlobalSettingType()) );
  ret_ = globSetting;
}
void   IP_SubcktCopyVisitor::Visit(IP_Option &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Option> option
      = TICTAC_SPTR<IP_Option>( new IP_Option(tokens_) );
  ret_ = option;
}
void   IP_SubcktCopyVisitor::Visit(IP_Output &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Output> output
      = TICTAC_SPTR<IP_Output>( new IP_Output(tokens_) );
  ret_ = output;
}
void   IP_SubcktCopyVisitor::Visit(IP_Measurement &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Measurement> measuerement
      = TICTAC_SPTR<IP_Measurement>( new IP_Measurement(tokens_) );
  ret_ = measuerement;
}
void   IP_SubcktCopyVisitor::Visit(IP_Assertion &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_Assertion> assertion
      = TICTAC_SPTR<IP_Assertion>( new IP_Assertion(tokens_) );
  ret_ = assertion;
}
void   IP_SubcktCopyVisitor::Visit(IP_AnaysisCard &node)
{
  getTokenSeqence(&node);
  TICTAC_SPTR<IP_AnaysisCard> analysis
      = TICTAC_SPTR<IP_AnaysisCard>( new IP_AnaysisCard(tokens_, node.getAnalysisType()) );
  ret_ = analysis;
}
