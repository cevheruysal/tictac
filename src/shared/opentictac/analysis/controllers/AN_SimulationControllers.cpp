/*
 * AN_SimulationControllers.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: benk
 */

#include "AN_SimulationControllers.hpp"
#include "Netlist.hpp"
#include "AN_Analysis.hpp"
#include "IN_SimOptions.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "AN_DTController.hpp"
#include "AN_Tran_TimeStepController.hpp"

AN_SimulationControllers::AN_SimulationControllers(
    [[maybe_unused]] const Netlist *netlist,
    [[maybe_unused]] const AN_Analysis *analysis,
    [[maybe_unused]] const IN_SimOptions* simulationOptions) : UT_SIMVerboseClass("SimulationControllers")
{
  // TODO set the default parameters correctly from the simulation option
  temperature_ = 25;
  actTime_ = 0.0;
  dtController_ = NULL;
  timeStepSizeController_ = NULL;
}

AN_SimulationControllers::~AN_SimulationControllers()
{
  // delete stuff
  if (dtController_) delete dtController_;
  if (timeStepSizeController_) delete timeStepSizeController_;
}

void AN_SimulationControllers::updateControllers(
    const Netlist *netlist,
    [[maybe_unused]] const AN_Analysis *analysis,
    const IN_SimOptions* simulationOptions,
    const LA_MatrixNetlistMap *map)
{
  // --- delete stuff before ---
  if (dtController_) delete dtController_;
  if (timeStepSizeController_) delete timeStepSizeController_;

  // NOW RE-CREATE the new controllers
  dtController_ = new AN_DTController(netlist, simulationOptions);
  // create the time step size controller
  timeStepSizeController_ =
      new AN_Tran_TimeStepController(map->getNrColAndRow(), simulationOptions,
          netlist, map, dtController_);
}
