/*
 * IP_NetlistVisitBase.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: benk
 */

#include "IP_NetlistVisitBase.hpp"

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

#include "IP_D_BaseElement.hpp"

IP_NetlistVisitBase::IP_NetlistVisitBase() : UT_SIMVerboseClass(std::string("NetlistVisitor"))
{
  // toplevel is NULL
  actTopLevel_ = NULL;
  isTopLevel_ = false;
  level_ = 0;
  isParamWithinSubcktDef_ = false;
  isParamWithinXCall_ = false;
  isParamWithinModel_ = false;
}

void   IP_NetlistVisitBase::startTopLevelVisit(TICTAC_SPTR<IP_SubcirDef> toplevelToVisit){
  // set the actual top-level
  actTopLevel_ = toplevelToVisit.get();
  isTopLevel_ = true;
  isParamWithinSubcktDef_ = false;
  isParamWithinXCall_ = false;
  isParamWithinModel_ = false;
  level_ = -1;
  // visit the top level
  visitNode(actTopLevel_);
  // set back the pointer to NULL
  actTopLevel_ = NULL;
}

void   IP_NetlistVisitBase::startTopLevelVisit(IP_SubcirDef* toplevelToVisit){
  // set the actual top-level
  actTopLevel_ = toplevelToVisit;
  isTopLevel_ = true;
  isParamWithinSubcktDef_ = false;
  isParamWithinXCall_ = false;
  isParamWithinModel_ = false;
  level_ = -1;
  // visit the top level
  visitNode(actTopLevel_);
  // set back the pointer to NULL
  actTopLevel_ = NULL;
}

void   IP_NetlistVisitBase::Visit(IP_NetlistStructBase &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_NetlistStructSequence &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_ControlBase &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_SubcirDef &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Element &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Function &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Model &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Param &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_SubcirSettings &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_XCall &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_ConditionedNetlist &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_GlobalSettings &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Option &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Output &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Measurement &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_Assertion &node){ VisitChildren(node); }
void   IP_NetlistVisitBase::Visit(IP_AnaysisCard &node){ VisitChildren(node); }

void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_NetlistStructBase &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren(IP_NetlistStructSequence &node){
  for(int i = 0; i < node.getSequenceLength() ; i++ )
    visitNode( node.getNetlistStruct(i).get() );
}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_ControlBase &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren(IP_SubcirDef &node){
  bool tmpsave = isTopLevel_;
  level_++;
  isTopLevel_ = (level_ > 0);
  this->isParamWithinSubcktDef_ = true;
  for(int i = 0; i < node.getNrParams() ; i++ )
    visitNode( node.getSubCircuitParams(i).get() );
  this->isParamWithinSubcktDef_ = false;
  // visit the body of the subcircuit
  visitNode( node.getSubcktBody().get() );
  level_--;
  isTopLevel_ = tmpsave;
}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Element &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Function &node) { ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren(IP_Model &node){
  this->isParamWithinModel_ = true;
  for(int i = 0; i < node.getNrModelParams(); i++ ) {
    //std::cout << " i=" << i << " n=" << node.getNrModelParams() << "\n";
    visitNode( node.getModelParam(i).get() );
  }
  this->isParamWithinModel_ = false;
}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Param &node){ ; /* nothing to do yet (this is just a name and an expression)*/}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_SubcirSettings &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren(IP_XCall &node){
  this->isParamWithinXCall_ = true;
  for(int i = 0; i < node.getNrParams() ; i++ )
    visitNode( node.getCalledParam(i).get() );
  this->isParamWithinXCall_ = false;
}
void   IP_NetlistVisitBase::VisitChildren(IP_ConditionedNetlist &node){
  visitNode( node.getTrueBranch().get() );
  if (node.hasFalseBranch())
    visitNode( node.getFalseBranch().get() );
}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_GlobalSettings &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Option &node){ ; /* nothing to do yet (this is just a pair of option name and eventl. value)*/}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Output &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Measurement &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_Assertion &node){ ; /* nothing to do yet */}
void   IP_NetlistVisitBase::VisitChildren([[maybe_unused]] IP_AnaysisCard &node){ ; /* nothing to do yet */}

void   IP_NetlistVisitBase::visitNode(IP_NetlistStructBase *node){
  // node should not be NULL
  //std::cout << "\n Visit IN \n";
  //std::cout << "Visit:" << node << "\n";
  if (node != NULL)
    node->Accept(*this) ;
}

void   IP_NetlistVisitBase::visitNode(IP_D_BaseElement *node){
  // node should not be NULL
  //std::cout << "\n Visit IN \n";
  //std::cout << "Visit:" << node << "\n";
  if (node != NULL)
    node->Accept(*this) ;
}
