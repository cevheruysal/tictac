#include "IP_D_ADC.hpp"

void
    IP_D_ADC::add_param(TICTAC_SPTR<IP_Param> p)
{
  if (x_adc_ == nullptr) SIM_ERROR_STOP("D_ADC: no pointer to x_adc provided.")

  std::string parameter_name = p->getName();
  if (parameter_name == "out_low" || parameter_name == "out_high" || parameter_name == "clamp_low"
      || parameter_name == "clamp_high" || parameter_name == "clamp_res"
      || parameter_name == "clamp_bias" || parameter_name == "time_tol")
    {
      x_adc_->add_param(p);
    }
  else if (parameter_name == "in_low" || parameter_name == "in_high")
    {
      x_adc_->add_param(p);
      IP_D_BaseElement::add_param(p);
    }
  else
    IP_D_BaseElement::add_param(p);
}

void
    IP_D_ADC::copy_params_adc(const IP_D_ADC& from)
{
  for (const auto& param : from.x_adc()->getCallParams())
    this->x_adc_->add_param(std::make_shared<IP_Param>(IP_Param(*(param))));

  IP_D_BaseElement::copy_params(from);
}

TICTAC_SPTR<IP_D_BaseElement>
    IP_D_ADC::promote_clone(std::string prefix)
{
  auto [x_adc, d_adc] = promote_clone_adc(prefix);
  return d_adc;
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>>
    IP_D_ADC::promote_clone_adc(std::string prefix)
{
  std::string in = promote_node_name(prefix, this->in()->node_name());
  std::string out = promote_node_name(prefix, this->out()->node_name());
  std::vector<IP_Token> tokens = this->x_adc()->getTokens();

  return create_adc_bridge(tokens, prefix + this->name_space(), this->model_name(), in, out);
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>>
    create_adc_bridge(std::vector<IP_Token>& tokens,
                      std::string model_name,
                      TICTAC_SPTR<IP_NetworkNode> in,
                      TICTAC_SPTR<IP_NetworkNode> out)
{
  if (out->is_analog()) SIM_ERROR_STOP("Output node must not be analog! Cannot generate adc.");

  // The analog part of the adc bridge only has one node, so this node is passed on
  // to systemc
  in->is_sc_in(true);

  auto d_adc = std::make_shared<IP_D_ADC>(IP_D_ADC(tokens[0].getValue(), model_name));

  std::vector<std::string> pinNameBinding(0); // name binding is empty for TITAN
  std::vector<TICTAC_SPTR<IP_Param>> xparams(0);
  tokens[0].setValue("X" + tokens[0].getValue());
  auto x_adc =
      TICTAC_SPTR<IP_XCall>(new IP_XCall(tokens, 1, "adc_bridge", xparams, pinNameBinding));
  x_adc->is_resolvable(false);

  // Link the XCall to the digital model
  d_adc->x_adc(x_adc);

  // Wire the analog part of the adc bridge (only one node!!)
  x_adc->add_network_node(in);

  // Wire the SystemC part of the adc bridge (input node is the same as for the analog part!!)
  d_adc->in(in);
  d_adc->out(out);

  out->digital_outs(d_adc); // Add the new bridge as a digital out

  return {x_adc, d_adc};
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>>
    create_adc_bridge(std::vector<IP_Token>& tokens,
                      std::string name_space,
                      std::string model_name,
                      std::string in,
                      std::string out)
{
  std::vector<std::string> pinNameBinding(0); // name binding is empty for TITAN
  std::vector<TICTAC_SPTR<IP_Param>> xparams(0);
  tokens[0].setValue("X" + name_space);
  tokens[1].setValue(in);
  auto x_adc =
      TICTAC_SPTR<IP_XCall>(new IP_XCall(tokens, 1, "adc_bridge", xparams, pinNameBinding));
  x_adc->is_resolvable(false);

  auto d_adc = create_d_adc_bridge(name_space, model_name, x_adc, in, out);

  // Wire the analog part of the adc bridge (only one node!!)
  x_adc->add_network_node(d_adc->in());

  return {x_adc, d_adc};
}

TICTAC_SPTR<IP_D_ADC>
    create_d_adc_bridge(std::string name,
                        std::string model_name,
                        TICTAC_SPTR<IP_XCall> x_adc,
                        std::string in,
                        std::string out)
{
  auto port_in = std::make_shared<IP_NetworkNode>(IP_NetworkNode(in));
  port_in->is_sc_in(true);

  auto port_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(out));
  port_out->is_analog(false);

  auto d_adc = std::make_shared<IP_D_ADC>(IP_D_ADC(name, model_name));
  d_adc->x_adc(x_adc);

  // Wire the SystemC part of the dac bridge (output node is the aux node)
  d_adc->in(port_in);
  port_out->digital_outs(d_adc); // Add the new bridge as a digital out
  d_adc->out(port_out);

  return d_adc;
}
