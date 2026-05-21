/*
 * AN_DTController.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: benk
 */

#include "AN_DTController.hpp"


AN_DTController::AN_DTController(const Netlist *netlist, const IN_SimOptions *simoptions)
: UT_SIMVerboseClass("DTController")
{
  // allocate memory
  MYINT i;
  //this->setVerb(6);
  nrHistoryStamp_ = simoptions->getMaxRequiredTimeHistory();
  nrDDTs_ = netlist->getNrDDTs();
  ctrVectors_.resize(nrHistoryStamp_);
  for (i = 0; i < nrHistoryStamp_; i++)
    { // create the vectors
      ctrVectors_[i].resize(2*nrDDTs_,0.0);
    }

  // limit the size of the history
  SIM_ERROR_COND_STOP(nrHistoryStamp_ > 5, "AN_DTController nr history steps should not be grater than 5")
  swapMap_.resize(nrHistoryStamp_, -1);

  for (i = 0; i < nrHistoryStamp_; i++)
    swapMap_[i] = i;
  timeStamps_.resize(nrHistoryStamp_, 0.0);
  timeDeltas_.resize(nrHistoryStamp_, 0.0);
  actualDeltaTime_ = 0.0;
  actualTime_ = 0.0;
  actualAlpha_ = 0.0;
  nrValidHistoryStamps_ = 0;
  integrationScheme_ = IE_SCHEME;
  //this->setVerb(10);
}


AN_DTController::~AN_DTController()
{
  // nothing to do at the moment
}

