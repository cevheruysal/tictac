/*
 * Capacitor.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#ifndef CAPACITOR_HPP_
#define CAPACITOR_HPP_

#include "sim_utils.hpp"
#include "Device.hpp"

/** capacitor class */
class Capacitor : public Device
{
public:
  /** Ctor of the resistor */
  Capacitor(MYINT p1, MYINT p2, MYREAL capacity, MYREAL mFact, std::string name);

  /** empty Dtor*/
  virtual ~Capacitor() {;}

  /** returns the element type */
  virtual DeviceType getDeviceType() const { return CAP_TYPE; }

  /** returns the capacity of the cpacitor*/
  MYREAL getCapacity() const { return capacity_; }

  /** returns the number of current variables */
  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 0; }

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
  /** capacity */
  MYREAL capacity_;
};

#endif /* CAPACITOR_HPP_ */
