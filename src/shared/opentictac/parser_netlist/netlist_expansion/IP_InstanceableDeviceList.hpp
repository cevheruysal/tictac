/*
 * IP_InstanceableDeviceList.hpp
 *
 *  Created on: Feb 23, 2015
 *      Author: benk
 */

#ifndef IP_INSTANCEABLEDEVICELIST_HPP_
#define IP_INSTANCEABLEDEVICELIST_HPP_

#include "IP_DeviceMatchinRule.hpp"
#include "IP_Element.hpp"
#include "sim_utils.hpp"


/** class which just provides the list of instanceable devices.
 * Each device which can be instanciated (device not X-call)
 * must register itself here in a list such that it will be considered
 * in the device matching process. */
class IP_InstanceableDeviceList
{
public:

  IP_InstanceableDeviceList();

  /** empty Dtor */
  virtual ~IP_InstanceableDeviceList() {;}

  /** @returns an array of device mathcings (array is empty when there is none )*/
  const std::vector< TICTAC_SPTR<IP_DeviceMatchinRule> >&  getListOfDeviceMatchings(IP_ElemPreType elemType);

private:

  /** device type belonging to */
  std::vector< IP_ElemPreType > devType_;

  /** for each dev type we have */
  std::vector< std::vector< TICTAC_SPTR<IP_DeviceMatchinRule> > > devMatchRule_;

  /** this list will be always empty and will be returned only when there is no match in the
   * getListOfDeviceMatchings" function */
  std::vector< TICTAC_SPTR<IP_DeviceMatchinRule> > emptyList_;
};

#endif /* IP_INSTANCEABLEDEVICELIST_HPP_ */
