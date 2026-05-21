/*
 * TitanNetlist.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: benk
 */

#include "Netlist.hpp"
#include "AN_Analysis.hpp"

Netlist::Netlist(std::string netListName, MYINT pariIndex, MYINT nrPartitions )
: UT_SIMVerboseClass("Netlist"), nrDDTs_(0), netListName_(netListName)
{
  // initialize the netlist
  allDevices_.resize(0);
  nrExternalNodes_ = 0;
  nrTotalNodes_ = 0;
  nrCurrentVariables_ = 0;
  allNodeNames_.resize(0);
  nodesFlag_.resize(0);
  partiID_ = pariIndex;
  nrPartitions_ = nrPartitions;
  //this->setVerb(6);
  nrDDTs_ = 0;
}

void Netlist::flushNetList()
{
  allDevices_.resize(0); // TODO: this might be a memory leak!!! (use smart pointers, or DELETE)
  nrExternalNodes_ = 0;
  nrTotalNodes_ = 0;
  nrCurrentVariables_ = 0;
  allNodeNames_.resize(0);
  nodesFlag_.resize(0);
  partiID_ = 0;
  nrPartitions_ = 1;
  nrDDTs_ = 0;
}

void Netlist::addInternalPin(MYINT pinIndex, const std::string& name)
{
  //just set the pin name
  allNodeNames_[nrExternalNodes_+pinIndex] = name;
}

void Netlist::buildNetlist(const AN_Analysis* analysis)
{
  MYINT i, j, nrInternalNode = 0, nrExternalNodes = 0, currentVars = 0;
  // empty set of all nodes
  std::set<MYINT> nodesSet;
  // count how often an external is used
  std::vector<MYINT> nodesUsed;

  MYINT ddtCounter = 0;
  nrDDTs_ = 0;

  // count all the nodes first
  SIM_PRINT_L3(verb(),"Netlist::buildNetlist nrElements=" << allDevices_.size());
  for (i = 0; i<(MYINT)allDevices_.size(); i++)
    {
      // count and set the number of DDTs !!!
      nrDDTs_ += allDevices_[i]->getNrDDT();
      for (j = 0; j < allDevices_[i]->getNrDDT(); j++)
        allDevices_[i]->setDDTIndex(j, partiID_, ddtCounter++);

      //TODO: also count the maximal number of element stamps, matrix and RHS
      // OR maybe not, since this could be analysis dependent ....

      nrInternalNode += allDevices_[i]->getNrInternalPins(analysis);
      for (j = 0; j < (allDevices_[i]->getNrPins() - allDevices_[i]->getNrInternalPins(analysis)); j++)
        {
          nodesSet.insert( allDevices_[i]->getPin(j,partiID_) );
          //SIM_PRINT_L5(verb()," test");
        }
    }

  std::set<MYINT>::iterator it = nodesSet.begin();
  // count all the external nodes
  // it is IMPORTANT that GROUND always exists
  for (; it != nodesSet.end(); it++) {
    //if (*it > 0)
    nrExternalNodes++;
  }
  SIM_PRINT_L0(verb()," Netlist nrExtNodes=" << nrExternalNodes);
  // test if all external nodes are used at least twice
  nodesUsed.resize(nrExternalNodes,0);
  it = nodesSet.begin();
  // mark all external pins as arrays which are used
  for (i = 0; i<(MYINT)allDevices_.size(); i++)
    {
      for (j = 0; j < (allDevices_[i]->getNrPins() - allDevices_[i]->getNrInternalPins(analysis)); j++)
        {
          nodesUsed[allDevices_[i]->getPin(j,partiID_)]++; // GROUND also counts
          //SIM_PRINT_L5(verb(),"nodesUsed[" << allElements_[i]->getPin(j,partiID_) << "]="
          //    << nodesUsed[allElements_[i]->getPin(j,partiID_)]);
        }
    }
  // test if all pins are
  for (i = 0; i < nrExternalNodes ; i++)
    {
      // TODO: if this is a partition of the global netlist than such cases might happen
      SIM_ERROR_COND_STOP( nodesUsed[i] < 2 , " Node i="<<i<<" is used less than twice " << nodesUsed[i]);
    }

  // total number of nodes is nrExternalNodes + nrInternalNode
  nrTotalNodes_ = nrExternalNodes + nrInternalNode;
  nrExternalNodes_ = nrExternalNodes;
  nrCurrentVariables_ = 0;
  //resize the name vector of the pinns
  allNodeNames_.resize(nrTotalNodes_);
  nrInternalNode = nrExternalNodes;
  // set the ID of the internal nodes
  for (i = 0; i<(MYINT)allDevices_.size(); i++)
    { // ask each element how much current variable has
      currentVars = allDevices_[i]->getNrInternalPins(analysis);
      for (j = allDevices_[i]->getNrExteralPins(); j < allDevices_[i]->getNrPins(); j++)
        { // set the current ID of each current variable
          allDevices_[i]->setPin(j,nrInternalNode, this->partiID_);
          // we add one internal pin
          this->addInternalPin(nrInternalNode,
              allDevices_[i]->getInternalNodeName(j-allDevices_[i]->getNrExteralPins()));
          nrInternalNode++;
        }
    }
  // collect all the current variables
  for (i = 0; i<(MYINT)allDevices_.size(); i++)
    { // ask each element how much current variable has
      currentVars = allDevices_[i]->getNrCurrentVariable(analysis);
      for (j = 0; j < currentVars; j++)
        { // set the current ID of each current variable
          allDevices_[i]->setCurrentID(j,nrTotalNodes_ -1 + nrCurrentVariables_ + j,this->partiID_);
        }
      nrCurrentVariables_ += currentVars;
    }
  SIM_PRINT_L3(verb()," nrTotalNodes = " << nrTotalNodes_ );
  SIM_PRINT_L3(verb()," nrExternalNodes = " << nrExternalNodes_ );
  SIM_PRINT_L3(verb()," nrCurrentVariables = " << nrCurrentVariables_ );

  // TODO:
}

void Netlist::addDevice(TICTAC_SPTR<Device> device)
{
  allDevices_.push_back(device);
}
