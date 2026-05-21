/*
 * OUT_MatlabPlotter.hpp
 *
 *  Created on: May 13, 2015
 *      Author: benk
 */

#ifndef OUT_MATLABPLOTTER_HPP_
#define OUT_MATLABPLOTTER_HPP_

#include "OUT_WaveFormPlotter.hpp"

/** */
class OUT_MatlabPlotter : public OUT_WaveFormPlotter
{

public:

  /** empty Ctor */
  OUT_MatlabPlotter();

  /** empty Dtor */
  virtual ~OUT_MatlabPlotter() {;}

  /** methods that initializes the recording/plotting*/
  virtual void startRecording();

  /** prints actual line into the file*/
  virtual void flushActualLine();

  /** finishes the plotting and closes the file */
  virtual void endPlotting();

private:

};

#endif /* OUT_MATLABPLOTTER_HPP_ */
