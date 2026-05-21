/*
 * AN_SimulationControllers.hpp
 *
 *  Created on: Oct 22, 2013
 *      Author: benk
 */

#ifndef AN_SIMULATIONCONTROLLERS_HPP_
#define AN_SIMULATIONCONTROLLERS_HPP_

class Netlist;
class AN_Analysis;
class LA_MatrixNetlistMap;
class AN_DTController;
class AN_Tran_TimeStepController;

#include "sim_utils.hpp"
#include "IN_SimOptions.hpp"


/** class to represent the general functionalities (and other controllers),
 * which are required by a simulation
 * */
class AN_SimulationControllers : public UT_SIMVerboseClass
{

public:

  /** Ctor */
  AN_SimulationControllers(
      const Netlist *netlist,
      const AN_Analysis *analysis,
      const IN_SimOptions* simulationOptions);

  /** Dtor */
  virtual ~AN_SimulationControllers();

  /** @return returns the actual temperature */
  MYREAL getTemperature() const { return temperature_; }
  /** sets the temperature
   * @param newTemp new temperature */
  void setTemperature(MYREAL newTemp) { temperature_ = newTemp;}

  /** @return actual time */
  MYREAL getTime() const { return actTime_; }
  /** sets the actual time, which might be necessary by refused time steps or similar */
  void setActTime(MYREAL newTime) { actTime_ = newTime; }

  /** returns the DT controller */
  AN_DTController* getDDTController() { return dtController_; }
  const AN_DTController* getDDTController() const { return dtController_; }

  /** returns the time step size controller */
  AN_Tran_TimeStepController* getTimeStepController() { return timeStepSizeController_; }
  const AN_Tran_TimeStepController* getTimeStepController() const { return timeStepSizeController_; }

  /** creates the time step size controller */
  void updateControllers(
      const Netlist *netlist,
      const AN_Analysis *analysis,
      const IN_SimOptions* simulationOptions,
      const LA_MatrixNetlistMap *map);

private:

  /** the DDT controller for the time integration (derivation) */
  AN_DTController *dtController_;

  /** the time step size controller for transient analysis */
  AN_Tran_TimeStepController *timeStepSizeController_;

  /** the actual temperature */
  MYREAL temperature_;

  /** actual simulation time, for DC is always zero */
  MYREAL actTime_;

};

#endif /* AN_SIMULATIONCONTROLLERS_HPP_ */
