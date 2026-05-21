/*
 * IP_NetlistStructSequence.hpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#ifndef IP_NETLISTSTRUCTSEQUENCE_HPP_
#define IP_NETLISTSTRUCTSEQUENCE_HPP_

#include "IP_NetlistStructBase.hpp"
#include "IP_D_BaseElement.hpp"

/** this a construct to represent a sequence of netlist constructs
 * This is just a "helping" construct to represent sequences of constructs */
class IP_NetlistStructSequence : public IP_NetlistStructBase
{
public:

  /** Ctor with tokens,
   * IMPORTANT, the tokens play here only a dummy role */
  IP_NetlistStructSequence(std::vector<IP_Token> &tokens);

  /** emtpy Dtor */
  virtual ~IP_NetlistStructSequence() {;}

  /** adds a new struct to the sequence */
  void addNetlistStruct(IP_NetlistStructBase *newStruct) {
    sequence_.push_back( TICTAC_SPTR<IP_NetlistStructBase>(newStruct) );
  }

  /** adds a new struct to the sequence (adding a smart pointer)*/
  void addNetlistStruct(TICTAC_SPTR<IP_NetlistStructBase> newStruct) {
    sequence_.push_back( newStruct );
  }

  /** adds a new struct to the sequence (adding a smart pointer)*/
  void addBridge(TICTAC_SPTR<IP_NetlistStructBase> newStruct) {
    bridge_sequence_.push_back( newStruct );
  }

  /** adds a new struct to the sequence (adding a smart pointer)*/
  void addNetlistDStruct(TICTAC_SPTR<IP_D_BaseElement> newStruct) {
    sequence_d_.push_back( newStruct );
  }

  void addflattenedNetlistDStruct(TICTAC_SPTR<IP_D_BaseElement> newStruct) {
    flattened_sequence_d_.push_back( newStruct );
  }

  void initialize_d_elements()
  {
    return flattened_sequence_d_.clear();
  }

  /** @returns the length of the sequence */
  MYINT getSequenceLength() const { return (MYINT)sequence_.size(); }
  auto getDSequenceLength() const { return sequence_d_.size(); }
  auto getBSequenceLength() const { return bridge_sequence_.size(); }
  auto getflattenedDSequenceLength() const { return flattened_sequence_d_.size(); }

  /** @returns the smart pointer reference to the struct element  */
  TICTAC_SPTR<IP_NetlistStructBase>& getNetlistStruct(int i) { return sequence_[i]; }
  const std::vector< TICTAC_SPTR<IP_NetlistStructBase> > & getSequence() {return sequence_;};
  const std::vector< TICTAC_SPTR<IP_NetlistStructBase> > & getBSequence() {return bridge_sequence_;};
  const std::vector< TICTAC_SPTR<IP_D_BaseElement> > & getDSequence() {return sequence_d_;};
  const std::vector< TICTAC_SPTR<IP_D_BaseElement> > & getflattenedDSequence() {return flattened_sequence_d_;};

  /// Digital element extension: the analog bridges are stored separately and only in the end of
  /// the digital flattening process become regular analog elements.
  void move_bridges()
  {
    for (const auto elem : bridge_sequence_)
        sequence_.push_back(std::move(elem));
    bridge_sequence_.clear();
  }
//  const TICTAC_SPTR<IP_NetlistStructBase>& getNetlistStruct(int i) const { return sequence_[i]; }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_NetlistStructSequence &)(*this)); }

  /** returns the pointer to the last element (might be useful if we do not
   * want to add the element twice )*/
  const IP_NetlistStructBase* getLastElementPointer() const {
    if ( sequence_.size() > 0)
      return sequence_.back().get();
    else
      return NULL;
  }

  /** returns the pointer to the last element (might be useful if we do not
 * want to add the element twice )*/
  const IP_D_BaseElement* getLastDElementPointer() const {
    if ( sequence_d_.size() > 0)
      return sequence_d_.back().get();
    else
      return nullptr;
  }

  void flatten_out_subckt_network_nodes(
      IP_NetlistStructSequence& seq,
      std::string call_stack,
      TICTAC_SPTR<std::unordered_map<std::string, std::string>> xcall_pin_pairs);

  bool has_digital_part_flattened() {return has_digital_part_flattened_;};
  void has_digital_part_flattened(bool has_digital_part_flattened)
  {
    has_digital_part_flattened_ = has_digital_part_flattened;
  };

 private:

  /** the sequence of constructs */
  std::vector< TICTAC_SPTR<IP_NetlistStructBase> > sequence_;

  /** the sequence of digital constructs */
  std::vector< TICTAC_SPTR<IP_D_BaseElement> > sequence_d_;
  std::vector< TICTAC_SPTR<IP_D_BaseElement> > flattened_sequence_d_;
  std::vector< TICTAC_SPTR<IP_NetlistStructBase> > bridge_sequence_;

  bool has_digital_part_flattened_ = false;

};

#endif /* IP_NETLISTSTRUCTSEQUENCE_HPP_ */
