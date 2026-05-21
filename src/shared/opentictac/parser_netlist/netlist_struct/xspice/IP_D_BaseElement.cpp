#include <IP_D_BaseElement.hpp>

const IP_Param*
    IP_D_BaseElement::get_param(std::string param_name, bool case_sens) const
{
  for (const auto& model_param : model_params_)
    {
      // if the parameter match than we delete this
      if (case_sens && model_param->getName() == param_name) return model_param.get();
      if (!case_sens && sim_toLowerString(model_param->getName()) == sim_toLowerString(param_name))
        return model_param.get();
    }
  return NULL;
}

void
    IP_D_BaseElement::add_param(TICTAC_SPTR<IP_Param> p)
{
  if (is_string_param(p))
    string_parameters_[p->getName()] = p->getInitExprToken().getValue();
  else
    model_params_.push_back(std::move(p));
}

void
    IP_D_BaseElement::add_param(std::string param_name, std::string param_value)
{
  this->add_param(std::make_shared<IP_Param>(IP_Param(param_name, param_value)));
}

void
    IP_D_BaseElement::add_param(const std::vector<TICTAC_SPTR<IP_Param>>& params)
{
  for (auto elem : params) this->add_param(elem);
}

bool
    IP_D_BaseElement::is_string_param(TICTAC_SPTR<IP_Param> p)
{
  if (string_parameters_.find(p->getName()) == string_parameters_.end())
    return false;
  else
    return true;
}

void
    IP_D_BaseElement::copy_params(const IP_D_BaseElement& from)
{
  for (auto from_param : from.model_params_)
    {
      model_params_.push_back(std::make_shared<IP_Param>(IP_Param(*from_param)));
    }
  for (auto& elem : from.string_parameters_)
    {
      string_parameters_[elem.first] = elem.second;
    }
  has_resolved_params_ = from.has_resolved_params_;
}

void
    param_multiply(std::string& param, int factor)
{
  if (factor == 1)
    param = "(" + param + ")";
  else if (factor > 1)
    param = "((" + param + ")*" + std::to_string(factor) + ")";
  else
    SIM_ERROR_STOP("Multiplication factor should be larger than zero")
}

void
    param_addition(std::string& param, const std::string& in_param)
{
  if (param == "")
    param = in_param;
  else
    param = param + " + " + in_param;
}