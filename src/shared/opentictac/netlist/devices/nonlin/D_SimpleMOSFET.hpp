/*
 * D_SimpleMOSFET.hpp
 *
 *  Created on: May 13, 2015
 *      Author: benk
 */

#ifndef D_SIMPLEMOSFET_HPP_
#define D_SIMPLEMOSFET_HPP_

#include "Device.hpp"

/** this device simply models a MOS level 0 device
 * where one can configure to be PMOS of NMOS, depending on one parameter
 * This MOSFET always has 4 pinns: D G S B
 * parameters are:
 * - N if equals 1 this is a NMOS, ptherwise PMOS
 * - W
 * - L
 * - KP
 * - VTO
 * - LAMBDA
 * - CGB capacity gate-bulk
 * - GMIN in cutt-off mode the conductivity btw S-B-D */
class D_SimpleMOSFET : public Device
{
public:

  /// Ctor with pinns and params
  D_SimpleMOSFET(std::vector<MYINT> &pins, std::vector<MYREAL> &params,
      std::string &instName, MYREAL mFact = 1.0);

  /// empty Dtor
  virtual  ~D_SimpleMOSFET() {;}

  // type of this device
  virtual DeviceType getDeviceType() const { return MOSFET;}
  // we have no internal voltage source (MNA current variable)
  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 0; }

  /** see super class */
  virtual void getAnalysisStamp(
       std::vector<MYINT>& stampColumn,
       std::vector<MYINT>& stampRow,
       std::vector<MatrixEntryType>& entryType,
       std::vector<MYREAL>& potentialEntryValues,
       MYINT &nrStampsElements,
       const AN_Analysis *analysis) const;

   /** see super class */
   virtual void getAnalysisStampRHS(
        std::vector<MYINT>& stampColumn,
        std::vector<MatrixEntryType>& entryType,
        std::vector<MYREAL>& potentialEntryValues,
        MYINT &nrRHSStampsElements,
        const AN_Analysis *analysis) const;

   /** see super class */
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
  /// the parameters in the given order (with their defaults if not given)
  std::vector<MYREAL> params_;

  MYREAL N_; ///< if equals 1 this is a NMOS, ptherwise PMOS
  MYREAL W_; ///<
  MYREAL L_; ///<
  MYREAL KP_; ///<
  MYREAL VTO_; ///<
  MYREAL LAMBDA_; ///<
  MYREAL CGB_; ///< capacity gate-bulk
  MYREAL GMIN_; ///< in cutt-off mode the conductivity btw S-B-D
};

#endif /* D_SIMPLEMOSFET_HPP_ */
