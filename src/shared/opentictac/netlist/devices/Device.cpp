/*
 * Element.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#include "Device.hpp"

MYINT Device::deviceGIDCount_ = 0;

Device::Device(MYINT numPin, std::string name, MYINT numParti)
: instanceName_(name)
{
  nrParti_ = numParti;
  deviceLIDs_.resize(nrParti_,-1);
  numPins_ = numPin;
  internalPinsIndex_ = -1;
  pins_.resize(numPins_*nrParti_,-1);
  deviceLIDs_[0] = deviceGID_ = deviceGIDCount_++; // at the beginning LID = GID
  mFactor_ = 1;
  nrDDTs_ = 0;
  temp_ = 27.0;  dtemp_ = 0.0; // TODO: set the temp differently ?
}

/** Ctor with two pins */
Device::Device(MYINT p1, MYINT p2, std::string name,
    MYINT numParti)
: instanceName_(name)
{
  nrParti_ = numParti;
  deviceLIDs_.resize(nrParti_,-1);
  numPins_ = 2;
  internalPinsIndex_ = -1;
  pins_.resize(numPins_*nrParti_,-1);
  pins_[0] = p1;  pins_[1] = p2;
  deviceLIDs_[0] = deviceGID_ = deviceGIDCount_++; // at the beginning LID = GID
  mFactor_ = 1;
  nrDDTs_ = 0;
  temp_ = 27.0;  dtemp_ = 0.0; // TODO: set the temp differently ?
}

/** Ctor with four pins */
Device::Device(std::vector<MYINT> &pins,
    std::string name, MYINT numParti) : instanceName_(name)
{
  nrParti_ = numParti;
  deviceLIDs_.resize(nrParti_,-1);
  numPins_ = (MYINT)pins.size();
  internalPinsIndex_ = -1;
  pins_.resize(numPins_*nrParti_,-1);
  for (MYINT i = 0; i < numPins_; i++)
   pins_[i] = pins[i];
  deviceLIDs_[0] = deviceGID_ = deviceGIDCount_++; // at the beginning LID = GID
  mFactor_ = 1;
  nrDDTs_ = 0;
  temp_ = 27.0;  dtemp_ = 0.0; // TODO: set the temp differently ?s
}
