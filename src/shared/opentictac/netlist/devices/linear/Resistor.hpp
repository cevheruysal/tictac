/*
 * Resistor.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#ifndef RESISTOR_HPP_
#define RESISTOR_HPP_

#include "sim_utils.hpp"
#include "Device.hpp"

/** resistor class */
class Resistor : public Device
{
public:
  /** Ctor of the resistor */
  Resistor(MYINT p1, MYINT p2, MYREAL resistance,
      MYREAL mFact, std::string name);

  /** empty Dtor*/
  virtual ~Resistor() {;}

  /** returns the element type */
  virtual DeviceType getDeviceType() const { return RES_TYPE; }

  /** returns the conductance of the resistor */
  MYREAL getConductance() const { return conductance_; }

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
  /** the conductance of the resistor */
  MYREAL conductance_;
};

#endif /* RESISTOR_HPP_ */
