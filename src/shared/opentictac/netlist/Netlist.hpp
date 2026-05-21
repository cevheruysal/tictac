/*
 * Netlist.hpp
 *
 *  Created on: Jun 17, 2013
 *      Author: benk
 */

#ifndef NETLIST_HPP_
#define NETLIST_HPP_

#include "sim_utils.hpp"
#include "Device.hpp"
#include "Resistor.hpp"
#include "Capacitor.hpp"
#include "Inductor.hpp"
#include "IndependentVoltageSource.hpp"

class AN_Analysis;

/** class containing the flat netlist read in from the CDF file (global netlist).
 * Furthermore this class could contain only one part of the CDF-file (global) netlist.
 * This class should also be used for partitioned simulations. */
class Netlist : public UT_SIMVerboseClass
{
public:

  /** Ctor */
  Netlist(std::string netListName, MYINT pariIndex = 0, MYINT nrPartitions = 1);

  /** empty Dtor */
  virtual ~Netlist() {;}

// -------------- METHODS TO BUILD THE NETLIST ------------

  /** adds a general device to the netlist*/
  void addDevice(TICTAC_SPTR<Device> device);

// ---------------- END NETLIST BUILDING METHODS -------------

  /** sets the number of device external nodes */
  void setExtNodeNumber(MYINT n) { this->nrExternalNodes_ = n; allNodeNames_.resize(n); }

  /** adds a new external pinn
   * @param pinName the new pin name */
  void addNewExternalPin(const std::string& pinName) { allNodeNames_.push_back(pinName); nrExternalNodes_++; nrTotalNodes_++;}

  /** returns the number of external nodes */
  MYINT getNrExternalNodes() const { return this->nrExternalNodes_; }

  /** returns the number of internal nodes */
  MYINT getNrInternalNodes() const { return (this->nrTotalNodes_ - this->nrExternalNodes_); }

  /** return the number of all nodes including GND */
  MYINT getNrNodes() const { return this->nrTotalNodes_; }

  /** return the number of all nodes excluding GND */
  MYINT getNrNodesNoGnd() const { return this->nrTotalNodes_-1; }

  /** returns the number of current variables */
  MYINT getNrCurrents() const { return nrCurrentVariables_; }

  /** returns the total number of variables in the netlist*/
  MYINT getNrNetlistVariables() const { return getNrNodesNoGnd() + getNrCurrents();}

  /** sets the name of an external node in the netlist
   * Here the ground node is always the "0" node */
  void setNodeName(MYINT n, std::string name) { allNodeNames_[n] = name; }

  /** returns the name of an external node */
  const std::string& getNodeName(MYINT n) const { return allNodeNames_[n]; }

  /** returns the partition ID of this netlist */
  MYINT getPariID() const { return partiID_; }

  /** sets the partitionID of this netlist */
  void setPartiID(MYINT partiID) { partiID_ = partiID; }

 // -------------- END BUILD NETLIST ------------

  /** deletes the netlist and creates an empty one */
  void flushNetList();

  /** once all devices are added to the netlist THIS FUNCTION SHOULD BE CALLED!
   *  this builds up the internal informations about the netlist and also makes
   *  crutial checks
   *  Sets also the internal nodes IDs
   *  @param analysis the actual analysis */
  void buildNetlist(const AN_Analysis* analysis);

  /** returns the number of all devices within the netlist */
  inline MYINT getNrDevices() const { return (MYINT)allDevices_.size(); }

  /** sets the number of partitions for each devices of the netlist.
   * This should be called only from the global partition !!!
   * @param nrPartition new number of partitions */
  void setNrPartition(MYINT nrPartition) {
    nrPartitions_ = nrPartition;
    for (decltype(allDevices_.size()) i = 0; i < allDevices_.size(); i++)
        allDevices_[i]->setPartiNumber(nrPartition);
  }

  /** returns the total number of partitions */
  MYINT getNrPartitions() const { return nrPartitions_; }

  /** returns a device from the netlist
   * @param i the index of the device */
  const Device* getDevice(MYINT i) const { return allDevices_[i].get();}
  Device* getDevice(MYINT i) { return allDevices_[i].get();}

  /** returns the number of time derivatives */
  MYINT getNrDDTs() const { return nrDDTs_; }

  /// looks for a node ID (index) for one node name, -1 when is not found
  MYINT getPinID(std::string nodeName) const {
    // TODO: for larger circuits later me might use maps (hash tables)
    for (MYINT i = 0; i < (MYINT)allNodeNames_.size(); i++)
      if (nodeName == allNodeNames_[i])
        return i;
    return -1;
  }

private:

  /** adds one internal node of a netlist device. If a device has internal pinns
   *  TODO: implement this, and add more comments
   * @param pinIndex pin index
   * @param name name of the internal pin */
  void addInternalPin(MYINT pinIndex, const std::string& name);

  /** number of device external nodes */
  MYINT nrExternalNodes_;

  /** number of total nodes */
  MYINT nrTotalNodes_;

  /** the voltage sources need current variables, these are counted by the netlist*/
  MYINT nrCurrentVariables_;

  /** the partition ID of this netlist */
  MYINT partiID_;
  /** the total number of partitions */
  MYINT nrPartitions_;

  /** number of time derivatives required by this netlist */
  MYINT nrDDTs_;

  /** names of the external nodes*/
  std::vector<std::string> allNodeNames_;
  /** flag associated to each external/internal node*/
  std::vector<char>        nodesFlag_;

  /** name of the net-list */
  std::string netListName_;

  /** contains all the devices in the circuit */
  std::vector< TICTAC_SPTR<Device> > allDevices_;
};

#endif /* NETLIST_HPP_ */
