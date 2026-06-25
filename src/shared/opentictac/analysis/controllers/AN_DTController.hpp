/*
 * AN_DTController.hpp
 *
 *  Created on: Oct 22, 2013
 *      Author: benk
 */

#ifndef AN_DTCONTROLLER_HPP_
#define AN_DTCONTROLLER_HPP_

#include "sim_utils.hpp"
#include "Netlist.hpp"
#include "IN_SimOptions.hpp"

/** enum types for integration schemes */
typedef enum {
  IE_SCHEME = 0,
  TR_SCHEME = 1
} IntegrationSchemeType;

/** */
typedef enum {
  NORMAL_STEP = 0,
  BREAKPOINT_STEP = 1
} TimeStepType;


/** class to control the time derivatives (DT) in the transient simulation.
 * All the time integration should be dealt with here !
 *
 * For each DT this controller needs to reserve two doubles,
 * the actual value of DDT(X) and X.
 *
 * The usage of this class within a transient analysis should be
 *  prepateFirstTranStep()
 *   - solve Newton -> first fake time-step, this should converge in 1 iteration
 *  prepateSecondTranStep() -> prepate the comming time steps
 *  while (not finished) {
 *    - solve Newton -> solve one time step
 *    - compute next time step length
 *    nextTimeStamp( ... )
 *
 *  The fake time step in the beginning is needed only for initialization of the
 *  DDTs and other operators.
 *  }*/
class AN_DTController : public UT_SIMVerboseClass
{
public:

  /** Ctor */
  AN_DTController(const Netlist *netlist, const IN_SimOptions *simoptions);

  /** Dtor */
  virtual ~AN_DTController();


// ==================== METHODS FOR THE ANALYSIS CONTROLLER =======================
  /** In case of failed time step one needs to reset
   * @param timeStepIndex index of the actual reseted time step
   * @param resetTimeStamp next time value
   * @param resetDeltaT new time step
   * @param stepType infor for the next time step (if it is after breakpoint)*/
  void resetNextTimeStamp(
      [[maybe_unused]] MYINT timeStepIndex,
      MYREAL resetTimeStamp,
      MYREAL resetDeltaT,
      TimeStepType stepType)
  {
    SIM_PRINT_L4( verb(), "resetNextTimeStamp: actualTime_=" << actualTime_ << " resetTimeStamp=" << resetTimeStamp
        << " actualDeltaTime_=" << actualDeltaTime_ << " resetDeltaT=" << resetDeltaT);
    actualDeltaTime_ = resetDeltaT;
    actualTime_ = resetTimeStamp;
    SIM_ERROR_COND_STOP( actualDeltaTime_ < 0.0 , " DDT resetNextTimeStamp DT became negative");
    SIM_ERROR_COND_STOP( SIMABS(actualDeltaTime_) <= 1E-30 , " DDT resetNextTimeStamp DT became zero");
    SIM_ERROR_COND_STOP( timeStamps_[swapMap_[1]] > actualTime_ , " DDT resetNextTimeStamp we can not go backward in time");
    // set the actual time point in the vector and the time step
    timeStamps_[swapMap_[0]] = actualTime_;
    timeDeltas_[swapMap_[0]] = actualDeltaTime_;

    // set the integration scheme
    if (stepType == BREAKPOINT_STEP) { // for breakpoints use IE method
        integrationScheme_ = IE_SCHEME;
        actualAlpha_ = 1.0/actualDeltaTime_;
      }
    else { // if there is no breakpoint then
        integrationScheme_ = IE_SCHEME;
        actualAlpha_ = 1.0/actualDeltaTime_;
      }
  }


  /** use this function to move to the next time point.
   * It is important for the integration method to know if this is a breakpoint
   * in order to start with first order method
   * @param timeStepIndex index of the actual time step
   * @param netxTimeStamp next time value
   * @param stepType infor for the next time step (if it is after breakpoint)*/
  void nextTimeStamp(
      [[maybe_unused]] MYINT timeStepIndex, MYREAL netxTimeStamp,
      TimeStepType stepType)
  {

    SIM_PRINT_L4( verb(), "nextTimeStamp: actualTime_=" << actualTime_ << " netxTimeStamp=" << netxTimeStamp);
    actualDeltaTime_ = netxTimeStamp - actualTime_;
    actualTime_ = netxTimeStamp;

    // what if actualDeltaTime_<0 becomes negative
    // Test possible errors or just trust the API user ?
    SIM_ERROR_COND_STOP( actualDeltaTime_ <= 0.0 , " DDT nextTimeStamp DT became negative");
    SIM_ERROR_COND_STOP( SIMABS(actualDeltaTime_) <= 1E-30 , " DDT nextTimeStamp DT became zero");

    if (stepType == BREAKPOINT_STEP) { // for breakpoints use IE method
        integrationScheme_ = IE_SCHEME;
        actualAlpha_ = 1.0/actualDeltaTime_;
      }
    else { // if there is no breakpoint then
        integrationScheme_ = IE_SCHEME;
        actualAlpha_ = 1.0/actualDeltaTime_;
      }

    // swap the history vector, and other bookkeeping
    // here make the array roll over
    SIM_PRINT_L4( verb(), "nextTimeStamp nrHistoryStamp_=" << nrHistoryStamp_ );
    MYINT tmp = swapMap_[nrHistoryStamp_-1];
    for (MYINT i = nrHistoryStamp_-1; i > 0; i--) {
        SIM_PRINT_L4( verb(), "i=" << i << " swapMap_[i]=" << swapMap_[i] << " swapMap_[i-1]=" << swapMap_[i-1] );
        swapMap_[i] = swapMap_[i-1];
    }
    swapMap_[0] = tmp;

    // set the actual time point in the vector and the time step
    timeStamps_[swapMap_[0]] = actualTime_;
    timeDeltas_[swapMap_[0]] = actualDeltaTime_;
    nrValidHistoryStamps_ = ((nrValidHistoryStamps_ >= nrHistoryStamp_) ? nrHistoryStamp_ : (nrValidHistoryStamps_+1));
    SIM_PRINT_L4( verb(), "nextTimeStamp: actualDeltaTime_=" << actualDeltaTime_ << " nrValidHistoryStamps_=" << nrValidHistoryStamps_ << " nrHistoryStamp_=" << nrHistoryStamp_);
  }


  /** function which should be called before the first tran step.
   * The first time step is always a fake one which only initializes the various
   * fields, where the DDT should be 0 */
  void prepateFirstTranStep() {
    // set all history to zero
    for (MYINT j = 0; j < nrHistoryStamp_; j++) {
        timeStamps_[j] = 0.0;
        timeDeltas_[j] = 0.0;
        for (MYINT i = 0; i < 2*nrDDTs_; i++)
          ctrVectors_[j][i] = 0.0;
    }
    // here is the trick to set all indices to "1" for this fake time step
    // (the fake time step is needed only for initialization purposes)
    // for this step all the DDTs will be zero
    for (MYINT j = 0; j < nrHistoryStamp_; j++)
      swapMap_[j] = 1;
    integrationScheme_ = IE_SCHEME;
    nrValidHistoryStamps_ = 0;
    //
    actualTime_ = 0.0;
    // this is set to a maximum value but it should not matter (could also set to "0.0")
    actualDeltaTime_ = 1E+29;
    actualAlpha_ = 0.0;
  }

  /** function to be called after the converged first tran step. This first time step is a
   * "fake" once which purely initializes the method.
   * @param firstTimeStamp this is the "real" first time step size (the previous is a fake one)*/
  void prepateSecondTranStep(MYREAL firstTimeStamp) {
    for (MYINT j = 0; j < nrHistoryStamp_; j++)
      swapMap_[j] = j;
    // we can already switch to TR scheme?  no and rather IE
    nrValidHistoryStamps_ = 1;
    integrationScheme_ = IE_SCHEME;
    actualTime_ = firstTimeStamp;
    actualDeltaTime_ = firstTimeStamp;
    timeStamps_[0] = actualTime_;
    timeDeltas_[0] = actualTime_;
    actualAlpha_ = 1.0/actualDeltaTime_;
  }



// =============================== METHODS FOR THE ELEMENTS/DEVICES ========================

  /** @returns the actual time step */
  inline MYREAL getDeltaT() const { return actualDeltaTime_; }

  /** retunrs the last value set in the actual time stamp
   * @param ddtIndex the DDT index */
  inline MYREAL getDDTX(MYINT ddtIndex) const { return ctrVectors_[swapMap_[0]][2*ddtIndex]; }

  /** returns the actual derivative
   * @param ddtIndex the DDT index */
  inline MYREAL getDDT(MYINT ddtIndex) const { return ctrVectors_[swapMap_[0]][2*ddtIndex+1]; }

  /** sets the value of the DDT argument
   * @param ddtIndex the index of the DDT
   * @param X the argument of the DDT*/
  inline void setDDTX(MYINT ddtIndex, MYREAL X) {
    SIM_PRINT_L5(verb(),"ddtIndex=" << ddtIndex << " x=" << X << " swapMap:" << swapMap_.size() << " ctrVectors:" << ctrVectors_.size());
    SIM_PRINT_L5(verb(),"swapMap_[0]=" << swapMap_[0] << " s=" << ctrVectors_[swapMap_[0]].size());

    ctrVectors_[swapMap_[0]][2*ddtIndex] = X;
    if (integrationScheme_ == IE_SCHEME) {
        // for IE: DDT(x)_{i} = (1/dt)*(x_{i} - x_{i-1})
        ctrVectors_[swapMap_[0]][2*ddtIndex + 1] =
            actualAlpha_*(ctrVectors_[swapMap_[0]][2*ddtIndex] - ctrVectors_[swapMap_[1]][2*ddtIndex]);
    }
    else
      // for TR: DDT(x)_{i} = 0.5*(1/dt)*(x_{i} - x_{i-1}) + 0.5*DDT(x)_{i-1}
      if (integrationScheme_ == TR_SCHEME) {
          ctrVectors_[swapMap_[0]][2*ddtIndex + 1] =
              actualAlpha_*(ctrVectors_[swapMap_[0]][2*ddtIndex] - ctrVectors_[swapMap_[1]][2*ddtIndex]);
          ctrVectors_[swapMap_[0]][2*ddtIndex + 1] =
              ctrVectors_[swapMap_[0]][2*ddtIndex + 1]
               + 0.5*ctrVectors_[swapMap_[1]][2*ddtIndex + 1];
      }
    SIM_PRINT_L5(verb(), "DDT=" << ctrVectors_[swapMap_[0]][2*ddtIndex + 1] << " actualAlpha=" << actualAlpha_
        << " diff=" << (ctrVectors_[swapMap_[0]][2*ddtIndex] - ctrVectors_[swapMap_[1]][2*ddtIndex]) );
  }

  /** returns the alpha coefficient for the implicit integration
   * this is relevant for the Jacobian matrix computation
   * @param ddtIndex the DDT index */
  inline MYREAL getAlpha([[maybe_unused]] MYINT ddtIndex) const { return actualAlpha_; }

  /** returns the alpha coefficient for the implicit integration
   * ddt(X) = \alpha * x_i + beta -> beta = ddt(X) - \alpha * x_i.
   * This might be used for the RHS setup, but there "getDDT" should be used
   * for structural reasons
   * @param ddtIndex the DDT index */
   inline MYREAL getBeta(MYINT ddtIndex) const {
     return ctrVectors_[swapMap_[0]][2*ddtIndex+1] - actualAlpha_ * ctrVectors_[swapMap_[0]][2*ddtIndex];
   }

   /** returns the number of DDT operators */
   MYINT getNrDDTs() const { return nrDDTs_; }

// ======================== STUFF REGARDING FINITE DIFFERENCES OF CHARGES ================

   /** returns the first derivative as finite diference*/
   MYREAL getDD1(MYINT ddtIndex, MYINT ind = 0) const {
     if (nrValidHistoryStamps_ > 1) {
       // only if we have enough data points
       return getDD1_priv(ddtIndex, ind);
     }
     else
       // otherwise return zero
       return 0.0;
   }

   /** returns the second derivative as finite diference*/
   MYREAL getDD2(MYINT ddtIndex, MYINT ind = 0) const {
     if (nrValidHistoryStamps_ > 2) {
         MYREAL v1 = getDD1_priv(ddtIndex, ind);
         MYREAL v2 = getDD1_priv(ddtIndex, ind+1);
         //std::cout << " ind=" << ind << " getDD1_priv(ddtIndex, ind)=" << getDD1_priv(ddtIndex, ind) << " getDD1_priv(ddtIndex, ind+1)=" << getDD1_priv(ddtIndex, ind+1) << "\n";
         // only if we have enough data points
         return (v1 - v2) / ( timeDeltas_[swapMap_[ind]] + timeDeltas_[swapMap_[ind+1]]);
     }
     else {
         //std::cout << " nrValidHistoryStamps_=" << nrValidHistoryStamps_ << "\n";
         // otherwise return zero
         return 0.0;
     }
   }

   /** returns the third derivative as finite diference*/
   MYREAL getDD3(MYINT ddtIndex) const {
     if (nrValidHistoryStamps_ > 3)
       // only if we have enough data points
       return (getDD2(ddtIndex, 0) - getDD2(ddtIndex, 1)) / ( timeDeltas_[swapMap_[0]] + timeDeltas_[swapMap_[1]]);
     else
       // otherwise return zero
       return 0.0;
   }

   /** returns the LTE of the x^dot, where x is the argument of the DDT operators
    * x is the charge and x^dot are the dynamic currents*/
  MYREAL getLTE_DDTX(MYINT ddtIndex) const {
  /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 
    if (integrationScheme_ == IE_SCHEME) {
      return getDeltaT() * SIMABS(getDD2(ddtIndex, 0));
    }
    return 0.0;
  /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
  }

   /** returns the actual integration scheme */
   IntegrationSchemeType getIntegScheme() const { return integrationScheme_; }

   /** returns the actual integration order (we will only have first and second order) */
   MYINT getIntegrationOrder() const {
     if (integrationScheme_ == IE_SCHEME)
       return 1;
     else
       return 2;
   }

private:
   /** private DD operator as finite difference*/
   inline MYREAL getDD1_priv(MYINT ddtIndex, MYINT shift) const {
     //std::cout << " shift=" << shift << " timeDeltas_[shift]=" << timeDeltas_[swapMap_[shift]] << "\n";
     return (ctrVectors_[swapMap_[shift]][2*ddtIndex] - ctrVectors_[swapMap_[shift+1]][2*ddtIndex]) / timeDeltas_[swapMap_[shift]];
   }


  /** vector of vectors to store the time derivative informations.
   * first index is the time stamp index [0] -> actual [1] -> time step before
   * ...
   * the second index is the DDT's access [2*ddt_index] -> X-value
   * [2*ddt_index + 1] -> DDT(X).
   * The easy swap is done my the map vector swapMap_*/
  std::vector< std::vector<MYREAL> > ctrVectors_;

  /** use this map vector to make an easy swap between the time stamp
   * (instead of pointers to vectors)
   * TODO: later for performance this could be replaced by simple INTEGER[4] */
  std::vector<MYINT> swapMap_;

  /** number of stored time step as history */
  MYINT nrHistoryStamp_;

  /** out of nrHistoryStamp_ how many time stamps hold valid informations */
  MYINT nrValidHistoryStamps_;

  /** number of DDTs required for one specified netlist */
  MYINT nrDDTs_;

  /** time stamps for the vector "ctrVectors_" */
  std::vector<MYREAL> timeStamps_;
  /** store the historic time deltas for nrTimeStapHistory-1 */
  std::vector<MYREAL> timeDeltas_;

  /** the actual time delta.
   * Later this might be device specific (local time steps)*/
  MYREAL actualDeltaTime_;

  /** this should used only internally, represents the actual simulation time */
  MYREAL actualTime_;

  /** at the time we have a global alpha coefficient (see technical docu of DDT)*/
  MYREAL actualAlpha_;

  /** the actual integration scheme */
  IntegrationSchemeType integrationScheme_;

};

#endif /* AN_DTCONTROLLER_HPP_ */
