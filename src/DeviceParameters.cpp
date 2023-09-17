#include "DeviceParameters.hpp"

APICLG::DeviceParameters::DeviceParameters()
{
  role = RoleType::middle;
  typeGate = TypeGate::rect;
  state = StateType::off;
  programType = ProgramType::solid;
  speed = 127;
  colorHSV = {0, 255, 255};
}
