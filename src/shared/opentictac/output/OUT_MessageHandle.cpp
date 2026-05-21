/*
 * OUT_MessageHandle.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: benk
 */

#include "OUT_MessageHandle.hpp"
#include "LS_SolverBase.hpp"
#include "Netlist.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "AN_Analysis.hpp"

OUT_MessageHandle::OUT_MessageHandle()
{
  // Nothing to do since this will be only a static field
}

void OUT_MessageHandle::writeProtocol(
    [[maybe_unused]] Analysis_type analyType,
    [[maybe_unused]] Output_type outputType,
    std::string output,
    [[maybe_unused]] MYINT levelOut,
    [[maybe_unused]] MYINT thread )
{
  // now just simply rpintthe message
  // TODO: later make more sophisticated output
  std::cout << output << std::endl;
}


void OUT_MessageHandle::writeProtocolError(
    [[maybe_unused]] Analysis_type analyType,
    [[maybe_unused]] Output_type outputType,
    std::string output,
    [[maybe_unused]] MYINT thread)
{
  // TODO: improve this later
  std::cout << "\nERROR: " << output << std::endl;
  exit(1);
}


void OUT_MessageHandle::writeProtocolWarning(
    [[maybe_unused]] Analysis_type analyType,
    [[maybe_unused]] Output_type outputType,
    std::string output,
    [[maybe_unused]] MYINT thread )
{
  // TODO: improve this later
  std::cout << "WARNING: " << output << std::endl;
}


void OUT_MessageHandle::printDCState(
    const AN_Analysis *analysis,
    const LS_SolverBase<MYREAL>* dcLinearSolver,
    const Netlist *netlist,
    const std::vector<MYREAL> &actualState)
{
  MYINT i, j, tmpI = 0;;
  std::string tabs = "  ";
  const LA_MatrixNetlistMap* map = dcLinearSolver->getSolverMatrix()->getMatrixMap();

  // start writing the protocol
  SIM_PROTOCOL_L0( DC_TYPE , "======= DC POINT ============");

  // starting from "1" since "0" is the ground
  for( i = 0; i < netlist->getNrExternalNodes()-1; i++)
    { // the first node is always the ground therefore start with the second node
      SIM_PROTOCOL_L0( DC_TYPE , tabs << "V(" << netlist->getNodeName(i+1) << ") : "
          << actualState[ map->getColIndexForUnkID(i)] << "[V]");
    }

  // now for each device plot the voltage on the internal node and the current variables
  // within this device
  for (i = 0; i < netlist->getNrDevices(); i++)
    {
      // plot the internal node voltages
      for (j = 0; j < netlist->getDevice(i)->getNrInternalPins(analysis); j++)
        {
          MYINT nrExternalPins = netlist->getDevice(i)->getNrExteralPins();
          tmpI = netlist->getDevice(i)->getPin(nrExternalPins + j, netlist->getPariID() );
          SIM_PROTOCOL_L0( DC_TYPE , tabs << netlist->getDevice(i)->getInstanceName()
              << " V(" << netlist->getDevice(i)->getInternalNodeName(j) << ") : "
              << actualState[ map->getColIndexForUnkID(tmpI)] << "[V]");
        }
      // plot the internal current values
      for (j = 0; j < netlist->getDevice(i)->getNrCurrentVariable(analysis); j++)
        {
          tmpI = netlist->getDevice(i)->getCurrentID(j, netlist->getPariID());
          SIM_PROTOCOL_L0( DC_TYPE , tabs << netlist->getDevice(i)->getInstanceName()
              << " I(" << j << ") : "
              << actualState[ map->getColIndexForUnkID(tmpI)] << "[A]");
        }
    }
}
