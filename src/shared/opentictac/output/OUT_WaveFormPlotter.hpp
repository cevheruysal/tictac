/*
 * OUT_WaveFormPlotter.hpp
 *
 *  Created on: Nov 8, 2013
 *      Author: benk
 */

#ifndef OUT_WAVEFORMPLOTTER_HPP_
#define OUT_WAVEFORMPLOTTER_HPP_

#include "sim_utils.hpp"

typedef enum {
  NODE_VOLTAGE_PLOT = 0,
  INTERNAL_VOLTAGE_PLOT = 1,
  CURRENT_PLOT = 2
} PlotVarType;

class Netlist;
class LA_MatrixNetlistMap;
class AN_SimulationControllers;

/** class representing the waveform plotter functionality.
 *
 * This might be mainly used in the transient analysis but it also meant to be used
 * in other context as well where the XARG is not just the time */
class OUT_WaveFormPlotter
{
public:

  /** emty Ctor*/
  OUT_WaveFormPlotter();

  /** empty Dtor */
  virtual  ~OUT_WaveFormPlotter();

  /** sets the */
  void setFileName(std::string fileName) { fileName_ = fileName;}

  /** add a new scalar plot to the plot file */
  MYINT addScalarPlot(
      std::string scalarPlotName,
      std::string scalarPlotUnit,
      PlotVarType plotType,
      MYINT scalarIndex,
      MYINT internalIndex = 0)
  {
    scalarPlotNames_.push_back(scalarPlotName);
    scalarPlotUnits_.push_back(scalarPlotUnit);
    scalarPlotTypes_.push_back(plotType);
    scalarPlotIndex_.push_back(scalarIndex);
    scalarPlotInternalIndex_.push_back(internalIndex);
    nrScalarPlots_++;
    //std::cout << " ADDSCALAR " << nrScalarPlots_ << "\n";
    return nrScalarPlots_-1;
  }

  /** returns the number of scalar plots */
  MYINT getNrScalarPlots() const { return nrScalarPlots_;}

  /** updates/gets all the ploting values from the actual state and netlist */
  void actualizeValues(
      const Netlist* netlist,
      const std::vector<MYREAL>& actState,
      const LA_MatrixNetlistMap* map,
      const AN_SimulationControllers* simCtr,
      MYREAL actualScalarXArg);

  /// important method for all wavefiles before start recording
  void startRecordingInit();

  /** methods that initializes the recording/plotting*/
  virtual void startRecording();

  /** prints actual line into the file*/
  virtual void flushActualLine();

  /** finishes the plotting and closes the file */
  virtual void endPlotting();

protected:

  /** save file name */
  std::string fileName_;

  /** the actual data file handler */
  std::ofstream myfileData_;
  /** the actual header file handler */
  std::ofstream myfileHeader_;

  /** number of scalar plots*/
  MYINT nrScalarPlots_;

  /** names of the scalar plots*/
  std::vector< std::string > scalarPlotNames_;
  /** the units of each plot */
  std::vector< std::string > scalarPlotUnits_;
  /** types of the scalar plots */
  std::vector< PlotVarType > scalarPlotTypes_;
  /** Indices used for the plotting, this is either node index or device index */
  std::vector< MYINT > scalarPlotIndex_;
  /** this is internal index for e.g., current variable of one device*/
  std::vector< MYINT > scalarPlotInternalIndex_;

  /** the actual values of the scalar plots */
  std::vector<MYREAL> actualScalarValues_;
  /** the actual xarg value of all scalar plots */
  MYREAL actualScalarXArg_;

  std::string xargUnits_;
  std::string xargName_;

  /** number of recorded points */
  MYINT nrRecordedPoints_;
};

#endif /* OUT_WAVEFORMPLOTTER_HPP_ */
