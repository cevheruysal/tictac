#pragma once

#include "Device.hpp"

class IndependentCurrentSource : public Device
{
 public:
         /**
          * \brief construcor of a simple current source
          *
          * \param[in] p1               pin number 1
          * \param[in] p2               pin number 2
          * \param[in] dcValue          dcValue of constnat source
          * \param[in] name             name of the device
          * \param[in] mfact            multiplicity of the source
          */
  IndependentCurrentSource(MYINT       p1,
                           MYINT       p2,
                           MYREAL      dcValue,
                           std::string name,
                           MYREAL      mfact);
  /** empty Dtor */
  virtual ~IndependentCurrentSource() { ; }

  /** type is independent voltage source */
  virtual DeviceType getDeviceType() const { return IELEM_TYPE; }

  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 0; }


  /** see base class */
  virtual void getAnalysisStamp(std::vector<MYINT> &          stampColumn,
                                std::vector<MYINT> &          stampRow,
                                std::vector<MatrixEntryType> &entryType,
                                std::vector<MYREAL> &         potentialEntryValues,
                                MYINT &                       nrStampsElements,
                                const AN_Analysis *           analysis) const;

  /** see base class */
  virtual void getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                   std::vector<MatrixEntryType> &entryType,
                                   std::vector<MYREAL> &         potentialEntryValues,
                                   MYINT &                       nrRHSStampsElements,
                                   const AN_Analysis *           analysis) const;

  /** see base class */
  virtual void evalDevice(
      // ---------- INPUTS --------
      const AN_Analysis *       analysis,
      AN_SimulationControllers *simulationController,
      std::vector<MYREAL> &     localVars,
      MYINT                     partiID,
      // ----------- OUTPUTS ----------
      MYINT &              nrMatrixStamp,
      MYINT &              nrRHSStamp,
      std::vector<MYREAL> &matrixValues,
      std::vector<MYREAL> &rhsValues,
      MYINT &              flags);

 private:
  MYREAL dcValue_;
};

