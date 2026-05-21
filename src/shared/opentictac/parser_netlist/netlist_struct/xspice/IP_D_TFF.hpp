//
// Created by adrian on 7/18/19.
//

#ifndef TICTAC_IP_D_TFF_HPP
#define TICTAC_IP_D_TFF_HPP

#include "IP_D_BaseElement.hpp"
#include "IP_NetworkNode.hpp"
#include "sim_utils.hpp"

class IP_D_TFF : public IP_D_BaseElement
{
 public:
  IP_D_TFF(std::string name_space, std::string model_name)
      : IP_D_BaseElement(name_space, model_name)
  {
    ;
  }
  /// empty Dtor
  virtual ~IP_D_TFF() { ; }

  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  TICTAC_SPTR<IP_NetworkNode> t() { return t_; };
  IP_D_TFF& t(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    t_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> clk() { return clk_; };
  IP_D_TFF& clk(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    clk_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> set() { return set_; };
  IP_D_TFF& set(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    set_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> reset() { return reset_; };
  IP_D_TFF& reset(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    reset_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> out() { return out_; };
  IP_D_TFF& out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    out_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> nout() { return nout_; };
  IP_D_TFF& nout(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    nout_ = network_node;
    return *this;
  };

  virtual void unify_nodes(IP_NetlistStructSequence& seq)
  {
    SIM_PRINT_L5(verb(), "We are in the d_tff!!!");
    t_ = seq.add_flattened_network_node(t_);
    clk_ = seq.add_flattened_network_node(clk_);
    set_ = seq.add_flattened_network_node(set_);
    reset_ = seq.add_flattened_network_node(reset_);
    out_ = seq.add_flattened_network_node(out_);
    nout_ = seq.add_flattened_network_node(nout_);
  }

  virtual void exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    bool could_not_exchange_any_node = true;

    if (t_.get() == old_node)
      {
        t_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (clk_.get() == old_node)
      {
        clk_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (set_.get() == old_node)
      {
        set_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (reset_.get() == old_node)
      {
        reset_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (could_not_exchange_any_node) SIM_ERROR_STOP("Cannot exchange any input node!")
  }

  virtual void exchange_digital_out(const IP_NetworkNode* old_node,
                                    TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    bool could_not_exchange_any_node = true;

    if (out_.get() == old_node)
      {
        out_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (nout_.get() == old_node)
      {
        nout_ = new_node;
        could_not_exchange_any_node = false;
      }

    if (could_not_exchange_any_node) SIM_ERROR_STOP("Cannot exchange any output node!")
  }

  virtual TICTAC_SPTR<IP_D_BaseElement> promote_clone(std::string prefix);

  virtual std::tuple<std::string, std::string, std::string> get_channel_info(IP_NetworkNode* nn);

 private:
  TICTAC_SPTR<IP_NetworkNode> t_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> clk_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> set_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> reset_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> out_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> nout_ = nullptr;
};

TICTAC_SPTR<IP_D_TFF>
    create_d_tff(std::string name,
                 std::string model_name,
                 std::string t,
                 std::string clkd,
                 std::string set,
                 std::string reset,
                 std::string out,
                 std::string nout);


#endif // TICTAC_IP_D_TFF_HPP