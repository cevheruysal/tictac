#ifndef SIMPLEBJT_HPP
#define SIMPLEBJT_HPP
#include "Device.hpp"

class SimpleBJT : public Device
{
 public:
  SimpleBJT(std::vector<MYINT> & pins,
            std::vector<MYREAL> &params,
            std::string &        instName,
            MYREAL               mFact = 1.0);

  virtual ~SimpleBJT(){;}

  virtual DeviceType getDeviceType() const { return BJT_TYPE; }

  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 0; }

  
  /** see super class */
  virtual void getAnalysisStamp(std::vector<MYINT> &          stampColumn,
                                std::vector<MYINT> &          stampRow,
                                std::vector<MatrixEntryType> &entryType,
                                std::vector<MYREAL> &         potentialEntryValues,
                                MYINT &                       nrStampsElements,
                                const AN_Analysis *           analysis) const;

  /** see super class */
  virtual void getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                   std::vector<MatrixEntryType> &entryType,
                                   std::vector<MYREAL> &         potentialEntryValues,
                                   MYINT &                       nrRHSStampsElements,
                                   const AN_Analysis *           analysis) const;

  /** see super class */
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
  MYREAL alpha_f_;
  MYREAL alpha_r_;
  MYREAL I_es_;
  MYREAL I_cs_;
  MYREAL v_t_;
  MYREAL v_t_inv_;
};
#endif
