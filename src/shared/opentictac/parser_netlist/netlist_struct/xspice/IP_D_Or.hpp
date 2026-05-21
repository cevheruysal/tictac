//
// Created by adrian on 10/21/19.
//

#ifndef TICTAC_IP_D_OR_HPP
#define TICTAC_IP_D_OR_HPP

#include "IP_D_BaseElement.hpp"
#include "IP_NetworkNode.hpp"
#include "sim_utils.hpp"

class IP_D_Or : public IP_D_BaseElement
{
 public:
  IP_D_Or(std::string name_space, std::string model_name)
      : IP_D_BaseElement(name_space, model_name)
  {
    ;
  }
  /// empty Dtor
  virtual ~IP_D_Or() { ; }

  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  TICTAC_SPTR<IP_NetworkNode> in(int i) { return ins_[i]; };

  const std::vector<TICTAC_SPTR<IP_NetworkNode>>& ins() { return ins_; };
  IP_D_Or& ins(const std::vector<TICTAC_SPTR<IP_NetworkNode>>& ins)
  {
    ins_ = ins;
    return *this;
  };

  IP_D_Or& in(int i, TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    ins_[i] = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> out() { return out_; };
  IP_D_Or& out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    out_ = network_node;
    return *this;
  };

  virtual void unify_nodes(IP_NetlistStructSequence& seq)
  {
    SIM_PRINT_L5(verb(), "We are in the D_Or!!!");
    for (auto& in : ins_) in = seq.add_flattened_network_node(in);
    out_ = seq.add_flattened_network_node(out_);
  }

  virtual void exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node);

  virtual void exchange_digital_out(const IP_NetworkNode* old_node,
                                    TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    if (out_.get() == old_node)
      out_ = new_node;
    else
      SIM_ERROR_STOP("Cannot exchange nodes!")
  }
  virtual std::tuple<std::string, std::string, std::string> get_channel_info(IP_NetworkNode* nn);

  virtual TICTAC_SPTR<IP_D_BaseElement> promote_clone(std::string prefix);

 private:
  std::vector<TICTAC_SPTR<IP_NetworkNode>> ins_;
  TICTAC_SPTR<IP_NetworkNode> out_ = nullptr;
};

TICTAC_SPTR<IP_D_Or>
    create_d_or(std::string name,
                std::string model_name,
                const std::vector<std::string>& ins,
                std::string out);

#endif // TICTAC_IP_D_OR_HPP