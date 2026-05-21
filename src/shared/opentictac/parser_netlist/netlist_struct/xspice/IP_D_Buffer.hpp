//
// Created by adrian on 7/18/19.
//

#ifndef TICTAC_IP_D_BUFFER_HPP
#define TICTAC_IP_D_BUFFER_HPP

#include "IP_D_BaseElement.hpp"
#include "IP_NetworkNode.hpp"
#include "sim_utils.hpp"

class IP_D_Buffer : public IP_D_BaseElement
{
 public:
  IP_D_Buffer(std::string name_space, std::string model_name)
      : IP_D_BaseElement(name_space, model_name)
  {
    ;
  }
  /// empty Dtor
  virtual ~IP_D_Buffer() { ; }

  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  TICTAC_SPTR<IP_NetworkNode> in() { return in_; };
  IP_D_Buffer& in(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    in_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> out() { return out_; };
  IP_D_Buffer& out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    out_ = network_node;
    return *this;
  };

  virtual void unify_nodes(IP_NetlistStructSequence& seq)
  {
    SIM_PRINT_L5(verb(), "We are in the d_inverter!!!");
    in_ = seq.add_flattened_network_node(in_);
    out_ = seq.add_flattened_network_node(out_);
  }

  virtual void exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    if (in_.get() == old_node)
      in_ = new_node;
    else
    SIM_ERROR_STOP("Cannot exchange nodes!")
  }
  virtual void exchange_digital_out(const IP_NetworkNode* old_node,
                                    TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    if (out_.get() == old_node)
      out_ = new_node;
    else
    SIM_ERROR_STOP("Cannot exchange nodes!")
  }

  virtual TICTAC_SPTR<IP_D_BaseElement>
  promote_clone(std::string prefix);

  virtual std::tuple<std::string, std::string, std::string> get_channel_info(IP_NetworkNode* nn);

 private:
  TICTAC_SPTR<IP_NetworkNode> in_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> out_ = nullptr;
};

TICTAC_SPTR<IP_D_Buffer>
    create_d_buffer(std::string name, std::string model_name, std::string in, std::string out);


#endif // TICTAC_IP_D_BUFFER_HPP