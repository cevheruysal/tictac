#include "IP_D_DFF.hpp"

TICTAC_SPTR<IP_D_BaseElement>
    IP_D_DFF::promote_clone(std::string prefix)
{
  std::string data = promote_node_name(prefix, this->data()->node_name());
  std::string clk = promote_node_name(prefix, this->clk()->node_name());
  std::string set = promote_node_name(prefix, this->set()->node_name());
  std::string reset = promote_node_name(prefix, this->reset()->node_name());

  std::string out = promote_node_name(prefix, this->out()->node_name());
  std::string nout = promote_node_name(prefix, this->nout()->node_name());

  return create_d_dff(
      prefix + this->name_space(), this->model_name(), data, clk, set, reset, out, nout);
}

std::tuple<std::string, std::string, std::string>
    IP_D_DFF::get_channel_info(IP_NetworkNode* nn)
{
  std::string data_load = "1p";
  std::string clk_load = "1p";
  std::string set_load = "1p";
  std::string reset_load = "1p";
  std::string sink_current = "0";
  std::string source_current = "0";

  const IP_Param* param = NULL;
  if (get_nr_params() > 0)
    {
      param = get_param("data_load");
      if (param != NULL) data_load = get_param("data_load")->getInitExprToken().getValue();

      param = get_param("clk_load");
      if (param != NULL) clk_load = get_param("clk_load")->getInitExprToken().getValue();

      param = get_param("set_load");
      if (param != NULL) set_load = get_param("set_load")->getInitExprToken().getValue();

      param = get_param("reset_load");
      if (param != NULL) reset_load = get_param("reset_load")->getInitExprToken().getValue();

      param = get_param("sink_current");
      if (param != NULL) sink_current = get_param("sink_current")->getInitExprToken().getValue();

      param = get_param("source_current");
      if (param != NULL)
        source_current = get_param("source_current")->getInitExprToken().getValue();

      param_multiply(data_load, 1);
      param_multiply(clk_load, 1);
      param_multiply(set_load, 1);
      param_multiply(reset_load, 1);

      param_multiply(sink_current, 1);
      param_multiply(source_current, 1);
    }

  bool not_a_single_connection = true;

  std::string total_load;
  std::string total_sink_current;
  std::string total_source_current;

  if (data_.get() == nn)
    {
      total_load = data_load;
      total_sink_current = sink_current;
      total_source_current = source_current;

      not_a_single_connection = false;
    }

  if (clk_.get() == nn)
    {
      param_addition(total_load, clk_load);
      param_addition(total_sink_current, sink_current);
      param_addition(total_source_current, source_current);

      not_a_single_connection = false;
    }

  if (set_.get() == nn)
    {
      param_addition(total_load, set_load);
      param_addition(total_sink_current, sink_current);
      param_addition(total_source_current, source_current);

      not_a_single_connection = false;
    }

  if (reset_.get() == nn)
    {
      param_addition(total_load, reset_load);
      param_addition(total_sink_current, sink_current);
      param_addition(total_source_current, source_current);

      not_a_single_connection = false;
    }

  if (not_a_single_connection)
    {
      SIM_ERROR_STOP("Node is not connected to this element!");
    }
  else
    {
      return std::make_tuple(total_load, total_sink_current, total_source_current);
    }
}


TICTAC_SPTR<IP_D_DFF>
    create_d_dff(std::string name,
                 std::string model_name,
                 std::string data,
                 std::string clk,
                 std::string set,
                 std::string reset,
                 std::string out,
                 std::string nout)
{
  auto port_data = std::make_shared<IP_NetworkNode>(IP_NetworkNode(data));
  port_data->is_analog(false);

  auto port_clk = std::make_shared<IP_NetworkNode>(IP_NetworkNode(clk));
  port_clk->is_analog(false);

  auto port_set = std::make_shared<IP_NetworkNode>(IP_NetworkNode(set));
  port_set->is_analog(false);

  auto port_reset = std::make_shared<IP_NetworkNode>(IP_NetworkNode(reset));
  port_reset->is_analog(false);

  auto port_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(out));
  port_out->is_analog(false);

  auto port_nout = std::make_shared<IP_NetworkNode>(IP_NetworkNode(nout));
  port_nout->is_analog(false);

  auto d_dff = std::make_shared<IP_D_DFF>(IP_D_DFF(name, model_name));

  d_dff->data(port_data);
  port_data->digital_ins(d_dff);
  d_dff->clk(port_clk);
  port_clk->digital_ins(d_dff);
  d_dff->set(port_set);
  port_set->digital_ins(d_dff);
  d_dff->reset(port_reset);
  port_reset->digital_ins(d_dff);

  d_dff->out(port_out);
  port_out->digital_outs(d_dff);
  d_dff->nout(port_nout);
  port_nout->digital_outs(d_dff);
  return d_dff;
}
