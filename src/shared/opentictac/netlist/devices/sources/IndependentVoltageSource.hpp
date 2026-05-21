/*
 * IndependentVoltageSource.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#ifndef INDEPENDENTVOLTAGESOURCE_HPP_
#define INDEPENDENTVOLTAGESOURCE_HPP_

#include "Device.hpp"

/** class that contains the PWL pattern and useful evaluation routines */
class PWLSource {
public:
  /// the time values for each PWL points
  std::vector<MYREAL> timeStamps_;
  /// the values (e.g. voltage) of each PWL points
  std::vector<MYREAL> voltageValues_;
  /// number of PWL points
  MYINT nrPoints_;
  /// index of the repeat PWL point
  MYINT repeatIndex_;
  /// start time of the PWL
  MYREAL startTime_;
  /// repreat time of the PWL points
  MYREAL repeatTime_;
  /// delay time of the PWL points
  MYREAL delayTime_;

  /** */
  void reserve(MYINT i){
    nrPoints_ = i;
    timeStamps_.resize(i,0.0);
    voltageValues_.resize(i,0.0);
  }

  /** sets the record to default values */
  void getDefaultValues(){
    startTime_ = 0.0; repeatTime_ = -1.0; nrPoints_ = 0; delayTime_ = 0.0;
    repeatIndex_ = -1;
  }

  /** this should be called to create a consistent state of the PWL*/
  void updateConstruct(){
    repeatIndex_ = -1;
    MYINT i;
    // the time stamps must be increasing
    for (i = 0; i < nrPoints_ -1 ; i++) {
      SIM_ERROR_COND_STOP(timeStamps_[i] >= timeStamps_[i+1], " PWL source error "
          "time stamps must keep increasing i=" << i << " t1="
          << timeStamps_[i] << " t2=" << timeStamps_[i+1]);
    }

    if (repeatTime_ >= 0.0){
        for (i = 0; i < nrPoints_; i++)
          if (repeatTime_ == timeStamps_[i]) repeatIndex_ = i;
        // error if this is not found
        if (repeatIndex_<0) {
            std::string tmp = "";
            for (i = 0; i < nrPoints_; i++)
              tmp += sim_doubleToString(timeStamps_[i]) + ", ";
            SIM_ERROR_COND_STOP(repeatIndex_<0," Could not find PWL repeat time=" <<
                repeatTime_ << " time vector = {" << tmp << "}");
          }
    }
  }

  /** gets the interpolated voltage
   * @param timeIn   [IN]  the PWL time (and NOT the simulation time)
   * @param actPoint [OUT] right PWL point of the interpolation */
  MYREAL getInterpolatedVoltage(MYREAL timeIn, MYINT &actPoint)
  {
    MYINT i;
    MYREAL ret = 0.0;
    actPoint = -1;
    for (i = 0; i < nrPoints_ - 1; i++) {
      if ( (timeStamps_[i] <= timeIn) && ((timeStamps_[i+1] >= timeIn))) {
        ret = ((timeStamps_[i+1]-timeIn)*voltageValues_[i] + (timeIn-timeStamps_[i])*voltageValues_[i+1])/(timeStamps_[i+1] -timeStamps_[i]);
        //std::cout << "getInterpolatedVoltage  timeStamps_[i]=" << timeStamps_[i] << " , voltageValues_[i+1]=" << voltageValues_[i+1] << "\n";
        actPoint = i+1;
      }
    }
    SIM_ERROR_COND_STOP(actPoint < 0," PWL getInterpolatedVoltage failed for t=" << timeIn);
    return ret;
  }

  /** evaluates for a time point the voltage value (that is returned )
   * @param timeIn      [IN] the simulation time
   * @param actPoint    [OUT] right PWL point
   * @param PWLLeftTime [OUT] time until the next PWL point (in simulation or PWL time does not matter) */
  MYREAL evalPWLSource(MYREAL timeIn, MYINT &actPoint, MYREAL &PWLLeftTime)
   {
     MYREAL ret = voltageValues_[0];
     PWLLeftTime = 0.0;
     // formula according to TITAN manual
     if (timeIn <= delayTime_) {
       ret = voltageValues_[0];
       PWLLeftTime = delayTime_ - timeIn;
       actPoint = 0;
     }
     else // not in the delay regime
     { // substract the delay time
         PWLLeftTime = delayTime_;
         timeIn = timeIn - delayTime_;
         MYREAL periodT = 0.0;
         if (repeatIndex_ >= 0) { // this is a periodic PWL
             if (timeIn > timeStamps_[repeatIndex_]) {
                 // we are in the repeat loop
                 MYREAL fact = 0.0;
                 timeIn = timeIn - timeStamps_[repeatIndex_];
                 periodT = timeStamps_[nrPoints_-1] - timeStamps_[repeatIndex_];
                 // timeIn are now only in the periodical part
                 fact = floor((MYREAL)timeIn/periodT);
                 timeIn = timeIn - periodT*fact;
                 ret = getInterpolatedVoltage(timeIn + timeStamps_[repeatIndex_], actPoint);
                 PWLLeftTime = timeStamps_[actPoint] - (timeIn + timeStamps_[repeatIndex_]);
             }
             else {
                 // we are not in the repeat loop
                 ret = getInterpolatedVoltage(timeIn, actPoint);
                 PWLLeftTime = timeStamps_[actPoint] - timeIn;
             }
         }
         else { // no repeat PWL
             if (timeIn >= timeStamps_[nrPoints_-1]) {
               ret = voltageValues_[nrPoints_-1]; // we have constant DC after last point
               actPoint = nrPoints_;
               PWLLeftTime = 1E+0; //1sec ... no further pwl Points
             }
             else { // call the interpolation function
               ret = getInterpolatedVoltage(timeIn, actPoint);
               PWLLeftTime = timeStamps_[actPoint] - timeIn;
             }
         }
     }
     //std::cout << " evalPWLSource timeIn=" << timeIn << " , ret=" << ret << " , actPoint=" << actPoint
     //    << " t=" << timeStamps_[actPoint] << " delayTime=" << delayTime_ << "\n";
     return ret;
   }
};

/** class to model sinus sources */
class SINSource {
public:
  MYREAL sinvo_;      ///<   offset V or A
  MYREAL sinva_;      ///<   amplitude  V or A
  MYREAL sinfreq_;    ///<   frequency       > 0     1/tstop    Hz
  MYREAL sintd_;      ///<   delay   $ \ge$ 0        0.0     s
  MYREAL sintheta_;   ///<   damping factor          0.0     1/s
  MYREAL sinphase_;   ///<   phase shift

   /** sets the record to default values */
   void getDefaultValues() {
     sinvo_ = 0.0;
     sinva_ = 1.0;
     sinfreq_ = 1.0;
     sintd_ = 0.0; sintheta_ = 0.0; sinphase_ = 0.0;
   }

   /** evaluates for a time point the voltage value (that is returned )
    * @param timeIn the actual simulation time */
   MYREAL evalSINSource(MYREAL timeIn) {
     MYREAL ret = 0.0;
     // formula according to TITAN manual
     if (timeIn < sintd_)
         ret = sinvo_ + sinva_ * sin(sinphase_ * 2.0 * (SIM_PI/ 360.0) );
     else
         ret = sinvo_ + sinva_
             * sin(2.0 * SIM_PI * sinfreq_ * (timeIn - sintd_) + sinphase_ * 2.0 * (SIM_PI/ 360.0))
             * exp(-(timeIn - sintd_)*sintheta_);
     return ret;
   }
};

/** enum type internally used for the independent voltage sources */
typedef enum {
  IndepVolt_PWL_Type = 0,
  IndepVolt_SIN_Type = 1,
  IndepVolt_DC_Type  = 2,
  IndepVolt_PULSE_Type  = 3
} IndepVoltageType;



/** class to model the independent voltage sources.
 * Actual we only support Pulse, PWL and sinus sources */
class IndependentVoltageSource : public Device {
public:
  /** Ctor with SIN source */
  IndependentVoltageSource(MYINT p1, MYINT p2, SINSource* sinsource,
      std::string name, MYREAL mFact = 1);

  /** Ctor with PWL source */
  IndependentVoltageSource(MYINT p1, MYINT p2, PWLSource* pwlSource,
      std::string name, MYREAL mFact = 1);

  /** Ctor with DC value source */
  IndependentVoltageSource(MYINT p1, MYINT p2, MYREAL dcValue,
      std::string name, MYREAL mFact = 1);

  /** empty Dtor */
  virtual ~IndependentVoltageSource() {;}

  /** type is independent voltage source */
  DeviceType getDeviceType() const { return VELEM_TYPE; }

  /** voltage sources have one current variables */
  virtual MYINT getNrCurrentVariable([[maybe_unused]] const AN_Analysis *analysis) const { return 1; }

  virtual MYINT getCurrentID(MYINT i, MYINT parti = 0) const {
    SIM_ERROR_COND_STOP(i != 0, "getCurrentID: Voltage source has only one current variable i=" << i);
    return currentID_[parti];
  }

  virtual void setCurrentID(MYINT i, MYINT curID, MYINT parti = 0) {
    SIM_ERROR_COND_STOP(i != 0, "setCurrentID: Voltage source has only one current variable i=" << i);
    currentID_[parti] = curID;
  }

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

  /** see base class */
  virtual bool needsTranPostProcesseing() const {
    // is true only for the PWL source, other sources should
    // be dealt with by the time step size control
    return ((sourceType_ == IndepVolt_PWL_Type) || ((sourceType_ == IndepVolt_SIN_Type) && (this->sinsource_.sintd_ > 0.0)));
  }

  /** see base class */
  virtual void tranPostProcess(
      AN_SimulationControllers* simulationController,
      MYREAL actualTime, MYREAL &proposedDT,
      MYINT &flags, MYREAL &startDT);

private:

  /** pwl pattern */
  PWLSource pwlsource_;

  /** variable to help for breakpoint detection.
   * This always represents the PWL point right to the actual evaluation time point
   * this might also become nrPWLPoints, which means now we are in a constant domain */
  MYINT pwlIndex_;

  /** Sinusoidal source */
  SINSource sinsource_;

  /** DC source, constant voltage source */
  MYREAL dcValue_;

  /** type of the voltage source */
  const IndepVoltageType sourceType_;

  /** the current IDs of the voltage source */
  std::vector<MYINT> currentID_;
};

#endif /* INDEPENDENTVOLTAGESOURCE_HPP_ */
