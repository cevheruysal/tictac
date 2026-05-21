/*
 * Device.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include "sim_utils.hpp"

// forward declaration of the analysis class
class AN_Analysis;
class AN_SimulationControllers;

/** enumeration type of the various device types */
typedef enum {
  ERR_DEV_TYPE = -1,
  RES_TYPE = 1,
  CAP_TYPE = 2,
  IND_TYPE = 3,
  VELEM_TYPE = 4,
  MOSFET = 5,
  IELEM_TYPE = 6,
  DIODE_TYPE = 7,
  BJT_TYPE = 8,
}  DeviceType;

/** enum type for the device flags*/
typedef enum {
  DEVICE_BREAKPOINT = 1,
  DEVICE_REFUSESTEP = 2,
  DEVICE_ERROR = 4
}  DeviceInterfaceFlags;


/** the basis class for all devices in the netlist (circuit)
 * Important that the first pins are always the external pins and the last ones
 * must be always the internal pinns.
 * IMPORTANT: the pin "0" is considered as GROUND */
class Device{
public:
  /** Ctor with no pin number specification
   * @param numPin total (intern+extern) number of pins
   * @param name name of the instance */
  Device(MYINT numPin, std::string name = std::string("NO NAME"),
      MYINT numParti = 1);

  /** Ctor with two pins */
  Device(MYINT p1, MYINT p2, std::string name = std::string("NO NAME"),
      MYINT numParti = 1);

  /** Ctor with four pins */
  Device(std::vector<MYINT> &pins,
      std::string name = std::string("NO NAME"),
      MYINT numParti = 1);

  /** empty Dtor*/
  virtual ~Device() {;}

  /** returns the number of pins*/
  MYINT getNrPins() const { return numPins_; }
  /** returns the number of partitions*/
  MYINT getNrPartis() const { return nrParti_; }
  /** returns the number of internal pins*/
  virtual MYINT getNrInternalPins([[maybe_unused]] const AN_Analysis *analysis) const { return ((internalPinsIndex_ < 0)? 0 : (internalPinsIndex_));}
  /** returns the number of external pins*/
  MYINT getNrExteralPins() const { return ((internalPinsIndex_ < 0)? numPins_ : (numPins_-internalPinsIndex_));}
  /** get the i-th pin number of partition "parti"*/
  MYINT getPin(MYINT i, MYINT parti = 0) const { return pins_[parti*numPins_+i]; }
  /** sets the i-th pin number of partition "parti" */
  void setPin(MYINT i, MYINT p, MYINT parti = 0) { pins_[parti*numPins_+i] = p; }

  /** sets the number of partition of this device,
   * @param nrParti new number of partition */
  virtual void setPartiNumber(MYINT nrParti) {
    // test the number of pins
    SIM_ERROR_COND_STOP(nrParti < 1,"setPartiNumber number of parti must be at least one nrParti=" << nrParti);
    nrParti_ = nrParti; pins_.resize(nrParti_*numPins_,-1);
    ddtsIndex_.resize(nrDDTs_*nrParti_,-1);
  }

  /** returns the name of the internal node (if there are any)
   * @param internalNodeIndex index of the internal node (0 .. nrInternalNode-1) */
  virtual const std::string& getInternalNodeName([[maybe_unused]] MYINT internalNodeIndex) const {
    SIM_ERROR_STOP("gerInternalNodeName not implemented");
    return this->getInstanceName();
  }

  /** virtual method to return the type of the device */
  virtual DeviceType getDeviceType() const = 0;

  /** returns the number of current variables */
  virtual MYINT getNrCurrentVariable(const AN_Analysis *analysis) const = 0;
  /** get the i-th current index of the device (index within the partition)
   *  of partition "parti"*/
  virtual MYINT getCurrentID([[maybe_unused]] MYINT i, [[maybe_unused]] MYINT parti = 0) const {
    //int *p = 0; p[3]=4; // provoce segfault to trace down this call
    SIM_ERROR_STOP("getCurrentID not implemented"); return -1;
  }
  /** sets the i-th current index of partition "parti"
   * @param curID new current ID of the actual partition */
  virtual void setCurrentID([[maybe_unused]] MYINT i, [[maybe_unused]] MYINT curID, [[maybe_unused]] MYINT parti = 0)
  {
    SIM_ERROR_STOP("setCurrentID not implemented");
  }

  /** this function maps the local variables of a device to the global
   * variables of a partition
   * @param localUnknownIndex local unknown index according to the convention
   * (first are the external pin voltages, next the internal pin voltages,
   * and last the current variables )
   * @param parti index of the partition
   * @param analysis the analysis
   * @return the global variable index (in the convention where the first are the
   * voltage on the external pins, next the voltages on the internal pins, last the currents),
   * it it is -1 then it is GND */
  MYINT getMapLocalVarsToGlobalVar(
      MYINT localUnknownIndex,
      MYINT parti,
      const AN_Analysis *analysis) const {
    //SIM_PRINT_L4(6, " localUnknownIndex=" << localUnknownIndex << " parti=" << parti);
    if (getVariableType(localUnknownIndex,analysis) == INT_CURRENT)
      return (getCurrentID(localUnknownIndex-this->getNrPins(), parti));
    else
      return (getPin(localUnknownIndex, parti) - 1); // -1 Since "0" is the GND pin
  }

  /** virtual method to get the local stamp of the device
   * The local variable indices are ordered as follows:
   * first come all the node voltages, and on the last places are
   * the current variables.
   * IF one of the index is >= numPins_ then this must be a current variable
   * otherwise this is a node voltage.
   * Furthermore if this number is "< numPins_ and isPinInternal() returns true
   * than this variable is an internal voltage.
   * This function is device specific
   * @param stampColumn vector containing the column of the touple
   * @param stampRow vector containing the row of the touple
   * @param entryType type of the entry (dynamic or static)
   * @param potentialEntryValues vector containing an estimate of the values,
   *                             which will stand in the matrix stamp of this element
   *                             These values should be given in absolute values!!!
   * @param nrStampsElements number of stamp elements (number of tuples)
   * @param analysis the actual analysis */
  virtual void getAnalysisStamp(
      std::vector<MYINT>& stampColumn,
      std::vector<MYINT>& stampRow,
      std::vector<MatrixEntryType>& entryType,
      std::vector<MYREAL>& potentialEntryValues,
      MYINT &nrStampsElements,
      const AN_Analysis *analysis) const = 0;

  /** virtual method to get the local stamp of the device into the RHS vector
   * @param stampColumn vector containing the column index of the RHS vector
   * @param entryType type of the entry (dynamic or static)
   * @param potentialEntryValues vector containing an estimate of the values,
   *                             this is for the RHS not so relevant
   * @param nrRHSStampsElements number of stamp elements into the RHS vector
   * @param analysis the actual analysis */
  virtual void getAnalysisStampRHS(
       std::vector<MYINT>& stampColumn,
       std::vector<MatrixEntryType>& entryType,
       std::vector<MYREAL>& potentialEntryValues,
       MYINT &nrRHSStampsElements,
       const AN_Analysis *analysis) const = 0;


  /** virtual method for device evaluation.
   * This method always assumes (for performance reasons) that the vectors are presized
   *
   * @param analysis               the actual analysis
   * @param simulationController  object with all the controllers required
   * @param localVars vector with all the local variables required for the device evaluation
   *                  (network variables such as voltages and currents.
   *                  Charges should be extracted from simulationController, since this does the ddt)
   * @param partiID the index of the partition (required for the DDTs)
   *
   * @param nrMatrixStamp        number of matrix stamp
   * @param nrRHSStamp           number of RHS stamps, where the indices
   * @param matrixValues         values of the matrix stamp
   * @param rhsValues            contributions to the RHS vector
   * @param flags                returned (by OR) the state of the device (e.g., refusing a time step)*/
  virtual void evalDevice(
      // ---------- INPUTS --------
      const AN_Analysis *analysis,
      AN_SimulationControllers* simulationController,
      std::vector<MYREAL> &localVars,
      MYINT partiID,
      // ----------- OUTPUTS ----------
      MYINT &nrMatrixStamp,
      MYINT &nrRHSStamp,
      std::vector<MYREAL> &matrixValues,
      std::vector<MYREAL> &rhsValues,
      MYINT &flags
      ) = 0;

  /** function which tells if the device needs after each transient time step a
   * post processing, e.g., if a device can generate break points or directly wants
   * to limit the size of the next time step */
  virtual bool needsTranPostProcesseing() const { return false; }

  /** function to be called after each converged time step
   * @param  simulationController all the simulation controllers
   * @param  actualTime time value at the converged
   * @param  proposedDT proposed next time step
   * @param  flags the flag to show different states of the device (if break point is required by this device)
   * @param  startDT if a device causes BP then it is its responsability to provide next time step */
  virtual void tranPostProcess([[maybe_unused]] AN_SimulationControllers* simulationController,
                               [[maybe_unused]] MYREAL actualTime,
                               [[maybe_unused]] MYREAL& proposedDT,
                               [[maybe_unused]] MYINT& flags,
                               [[maybe_unused]] MYREAL& startDT)
  {
    SIM_ERROR_STOP("tranPostProcess() not implemented");
  }

  /** each device has several variables from the global, which the corresponding device
   * is bound to. Such are voltages on internal or external nodes, or current variables.
   * @param varI the variable index (0 till getNrVariables())
   * @param analysis the analysis mode (actual is not used)*/
  virtual MNAVarType getVariableType(MYINT varI, [[maybe_unused]] const AN_Analysis *analysis) const
  {
    // SIM_PRINT_L4(6, " getVariableType varI=" << varI << " numPins_=" << numPins_);
    if (varI >= numPins_)
      return INT_CURRENT;
    else
      {
        if (isPinInternal(varI))
          return INT_VOLTAGE;
        else
          return EXT_VOLTAGE;
      }
  }

  /** each device has a set of global variable which the device is bound to, such
   * as node voltages and current variables. The total number total variables is
   * returned by this function.
   * @param analysis the given anlysis (actual is not considered )*/
  virtual MYINT getNrVariables(const AN_Analysis *analysis) const { return (numPins_ + this->getNrCurrentVariable(analysis)); }

  /** returns the global ID of this element*/
  MYINT getDeviceGID() const { return deviceGID_; }

  /** sets the global ID of this device */
  void setDeviceGID(MYINT newID) { deviceGID_ = newID; }

  /** returns the local ID of this device of a given partition ID*/
  MYINT getDeviceLID(MYINT partiID) const { return deviceLIDs_[partiID]; }

  /** sets the local ID of this device of a given partition ID*/
  void setDeviceLID(MYINT partiID, MYINT newID) { deviceLIDs_[partiID] = newID; }

  /** resize the array containing the LID's for various partitioning  */
  void resizePariLIDs(MYINT partiNr) { deviceLIDs_.resize(partiNr); }

  /** returns the instance name of the device */
  const std::string& getInstanceName() const { return instanceName_; }

  /** returns true if the given pin is internal to the device or external*/
  bool isPinInternal(MYINT pinIndex) const {
    return ((internalPinsIndex_ < 0) ? false : (pinIndex >= internalPinsIndex_));
  }

  /** returns the DDT index (global for one netlist) for a given local DDT index
   * and for a parition index (if there are many partitions )*/
  MYINT getDDTIndex(MYINT localDDTIndex, MYINT pariID) const
    { return ddtsIndex_[pariID*nrDDTs_ + localDDTIndex]; }

  /** setst the DDT index for a device local DDT operator */
  void setDDTIndex(MYINT localDDTIndex, MYINT pariID, MYINT globalDDTindex)
    { ddtsIndex_[pariID*nrDDTs_ + localDDTIndex] = globalDDTindex; }

  /** returns the number of local DDTs which are required */
  inline MYINT getNrDDT() const { return nrDDTs_; }
  /** returns the actual ID */
  static MYINT getCurrentMaximalID() { return deviceGIDCount_; }

  /** get the temperature of the device */
  MYREAL getTemp() const { return temp_; }
  /** get the temperature difference */
  MYREAL getDTemp() const { return dtemp_; }
  /** set the temperature of the device */
  void setTemp(MYREAL t) { temp_ = t; }
  /** set the difference of the temperature */
  void setDTemp(MYREAL t) { dtemp_ = t; }

  /** returns the M factor of this device */
  MYREAL getMFactor() const { return mFactor_; }
  /** sets the M factor */
  void setMFactor(MYREAL newMFact) {mFactor_ = newMFact;}


protected:

  /** resets the pin containing */
  void resetPinNumber(MYINT pinNum) {
    // test the number of pins
    SIM_ERROR_COND_STOP(pinNum < 2,"resetPinNumber number of pins must be at least 2 but pinNum=" << pinNum);
    numPins_ = pinNum; pins_.resize(nrParti_*numPins_,-1);
  }

  /** set the number of local time derivation operators */
  void setNrDDTs(MYINT nrLocalDDTs) {
    nrDDTs_ = nrLocalDDTs; ddtsIndex_.resize(nrDDTs_*nrParti_,-1);
  }

  /** number of total pins, external and internals*/
  MYINT numPins_;

  /** field is set > 0 when an device contains intern
   * otherwise is -1
   * IMPORTANT: the first pins MUST be the externals and the last ones are the
   * internal pins */
  MYINT internalPinsIndex_;

  /** vectors containing the pin numbers, size is nrPari_*numPins_ */
  std::vector<MYINT> pins_;

  /** global device ID*/
  MYINT deviceGID_;

  /** local device IDs for different partitions */
  std::vector<MYINT> deviceLIDs_;
  /* number of partitions */
  MYINT nrParti_;

  /** instance name of the device */
  std::string instanceName_;

  /** number of time derivatives required in this device*/
  MYINT nrDDTs_;

  /** vector containing the DDT indices, which
   * size is nrDDTs_*nrParti_ */
  std::vector<MYINT> ddtsIndex_;

  /** m-factor of the device */
  MYREAL mFactor_;

  /** temperature of the device */
  MYREAL temp_;

  /** temperature difference of this device to the temp */
  MYREAL dtemp_;

  /** static device counter for unique IDs */
  static MYINT deviceGIDCount_;
};

#endif /* DEVICE_HPP_ */
