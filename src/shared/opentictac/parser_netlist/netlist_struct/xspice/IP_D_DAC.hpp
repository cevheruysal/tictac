//
// Created by adrian on 7/18/19.
//

#ifndef TICTAC_IP_D_DAC_HPP
#define TICTAC_IP_D_DAC_HPP

#include "IP_D_BaseElement.hpp"
#include "IP_NetworkNode.hpp"
#include "IP_XCall.hpp"
#include "sim_utils.hpp"

class IP_D_DAC : public IP_D_BaseElement
{
 public:
  IP_D_DAC(std::string name_space, std::string model_name)
      : IP_D_BaseElement(name_space, model_name)
  {
    ;
  }
  /// empty Dtor
  virtual ~IP_D_DAC() { ; }
  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  virtual void add_param(TICTAC_SPTR<IP_Param> p);
  virtual void add_param(std::string param_name, std::string param_value)
  {
    IP_D_BaseElement::add_param(param_name, param_value);
  };
  virtual void add_param(const std::vector<TICTAC_SPTR<IP_Param>>& params)
  {
    IP_D_BaseElement::add_param(params);
  };

  void copy_params_dac(const IP_D_DAC& from);

  TICTAC_SPTR<IP_NetworkNode> in() const { return in_; };
  IP_D_DAC& in(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    in_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> out() const { return out_; };
  IP_D_DAC& out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    out_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> analog_out() { return analog_out_; };
  IP_D_DAC& analog_out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    analog_out_ = network_node;
    return *this;
  };

  virtual void unify_nodes(IP_NetlistStructSequence& seq)
  {
    SIM_PRINT_L5(verb(), "We are in the d_dac!!!");
    in_ = seq.add_flattened_network_node(in_);
    out_ = seq.add_flattened_network_node(out_);
    analog_out_ = seq.add_flattened_network_node(analog_out_);
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

  virtual std::tuple<std::string, std::string, std::string> get_channel_info(IP_NetworkNode* nn)
  {
    if (in_.get() == nn)
      {
        std::string input_load = "1p";
        std::string sink_current = "0";
        std::string source_current = "0";

        const IP_Param* param = NULL;
        if (get_nr_params() > 0)
          {
            param = get_param("input_load");
            if (param != NULL) input_load = get_param("input_load")->getInitExprToken().getValue();

            param = get_param("sink_current");
            if (param != NULL)
              sink_current = get_param("sink_current")->getInitExprToken().getValue();

            param = get_param("source_current");
            if (param != NULL)
              source_current = get_param("source_current")->getInitExprToken().getValue();
          }
        return std::make_tuple(input_load, sink_current, source_current);
      }
    else
      SIM_ERROR_STOP("Node is not connected to this element!");
  }

  virtual TICTAC_SPTR<IP_D_BaseElement>
      promote_clone(std::string prefix);
  std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
      promote_clone_dac(std::string prefix);


  void x_dac(TICTAC_SPTR<IP_XCall> x_dac) { x_dac_ = x_dac; }
  TICTAC_SPTR<IP_XCall> x_dac() const { return x_dac_; }

 private:
  TICTAC_SPTR<IP_NetworkNode> in_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> out_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> analog_out_ = nullptr;
  TICTAC_SPTR<IP_XCall> x_dac_ = nullptr;
};

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
    create_dac_bridge(std::vector<IP_Token>& tokens,
                      std::string model_name,
                      TICTAC_SPTR<IP_NetworkNode> in,
                      TICTAC_SPTR<IP_NetworkNode> out);


std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
    create_dac_bridge(std::vector<IP_Token>& tokens,
                      std::string name_space,
                      std::string model_name,
                      std::string in,
                      std::string out,
                      std::string analog_out);


#endif // TICTAC_IP_D_DAC_HPP