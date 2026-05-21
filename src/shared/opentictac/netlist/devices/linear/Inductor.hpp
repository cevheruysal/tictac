/*
 * Inductor.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#ifndef INDUCTOR_HPP_
#define INDUCTOR_HPP_

#include "sim_utils.hpp"
#include "Device.hpp"

/** inductor class */
class Inductor : public Device
{
public:
  /** Ctor of the resistor */
  Inductor(MYINT p1, MYINT p2, MYREAL inductance,
      MYREAL mFact, std::string name);

  /** empty Dtor*/
  virtual ~Inductor() {;}

  /** returns the element type */
  virtual DeviceType getDeviceType() const { return IND_TYPE; }

  /** returns the inductance */
  MYREAL getInductance() const { return inductance_; }

  /** inductors have one current variable ( U = L*dI/dt)*/
  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 1; }

  virtual MYINT getCurrentID(MYINT i, MYINT parti = 0) const {
    SIM_ERROR_COND_STOP(i != 0, "getCurrentID: Inductor has only one current variable i=" << i);
    return currentID_[parti];
  }

  /** sets the current variable index of a given partition */
  virtual void setCurrentID(MYINT i, MYINT curID, MYINT parti = 0) {
    SIM_ERROR_COND_STOP(i != 0, "setCurrentID: Inductor has only one current variable i=" << i);
    currentID_[parti] = curID;
  }

  /** need to resize the array of currents */
  virtual void setPartiNumber(MYINT nrParti){
    Device::setPartiNumber(nrParti);
    currentID_.resize(nrParti);
  }

  /** see base class */
  virtual void getAnalysisStamp(
      std::vector<MYINT>& stampColumn,
      std::vector<MYINT>& stampRow,
      std::vector<MatrixEntryType>& entryType,
      std::vector<MYREAL>& potentialEntryValues,
      MYINT &nrStampsElements,
      const AN_Analysis *analysis) const;

  /** see base class */
  virtual void getAnalysisStampRHS(
       std::vector<MYINT>& stampColumn,
       std::vector<MatrixEntryType>& entryType,
       std::vector<MYREAL>& potentialEntryValues,
       MYINT &nrRHSStampsElements,
       const AN_Analysis *analysis) const;

  /** see base class */
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
      );

private:

  /** inductance */
  MYREAL inductance_;

  /** the current IDs of the inductor */
  std::vector<MYINT> currentID_;
};

#endif /* INDUCTOR_HPP_ */
