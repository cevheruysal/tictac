#pragma once
#include "OUT_WaveFormPlotter.hpp"

class OUT_GnuplotPlotter : public OUT_WaveFormPlotter
{
 public:
  OUT_GnuplotPlotter();
  virtual ~OUT_GnuplotPlotter() { ; }
  virtual void startRecording();
  virtual void flushActualLine();
  virtual void endPlotting();
};
