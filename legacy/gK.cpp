/*
 Copyright (C) 2011 Georgia Institute of Technology

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <gK.h>
#include <math.h>
#include <qwhatsthis.h>

static inline double
a_inf(double V)
{
  return pow(
      0.0761 * exp(0.0314 * (V + 94.22)) / (1 + exp(0.0346 * (V + 1.17))),
      (1 / 3.0));
}

static inline double
tau_a(double V)
{
  return 0.3632e-3 + 1.158e-3 / (1 + exp(0.0497 * (V + 55.96)));
}

static inline double
b_inf(double V)
{
  return pow(1 / (1 + exp(0.0688 * (V + 53.3))), 4.0);
}

static inline double
tau_b(double V)
{
  return 1.24e-3 + 2.678e-3 / (1 + exp(0.0624 * (V + 50.0)));
}

extern "C" Plugin::Object *
createRTXIPlugin(void)
{
  return new gK();
}

static DefaultGUIModel::variable_t vars[] =
  {
    { "Vm", "Membrane potential (V)", DefaultGUIModel::INPUT, },
    { "Iout", "Output current (A)", DefaultGUIModel::OUTPUT, },
    { "G_K_max", "mS/cm^2", DefaultGUIModel::PARAMETER
        | DefaultGUIModel::DOUBLE, },
    { "E_K", "mV", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
    { "Junc. Pot.", "Junction potential (mV)", DefaultGUIModel::PARAMETER
        | DefaultGUIModel::DOUBLE, },
    { "rate", "Number of integrations/time step", DefaultGUIModel::PARAMETER
        | DefaultGUIModel::UINTEGER, },
    { "Vm2", "Membrane potential (mV)", DefaultGUIModel::STATE, },
    { "a", "A-type Potassium Activation", DefaultGUIModel::STATE, },
    { "b", "A-type Potassium Inactivation", DefaultGUIModel::STATE, },
    { "Time (s)", "Time (s)", DefaultGUIModel::STATE, }, };

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

gK::gK(void) :
  DefaultGUIModel("Gk", ::vars, ::num_vars)
{
  QWhatsThis::add(
      this,
      "<p><b>Gk:</b><br>This module presents an example of a potassium ion channel where"
        "I = G_K_max*a^3*b*(Vm-E_K). </p>");
  createGUI(vars, num_vars);
  initParameters();
  update( INIT);
  refresh();
}

gK::~gK(void)
{
}

void
gK::execute(void)
{
  systime = count * period; // time in seconds
  Vm = input(0) * 1e3 - jpot; // convert to mV

  for (int i = 0; i < rate; i++)
    {
      da = (a_inf(Vm) - a) / tau_a(Vm);
      db = (b_inf(Vm) - b) / tau_b(Vm);
      a += period / rate * da;
      b += period / rate * db;
    }

  // nS * mV = pA (output amps)
  Iout = G_K_max * a * a * a * b * (Vm - E_K);
  output(0) = Iout * 1e-12; // convert to amps
  count++;
}

void
gK::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag)
    {
  case INIT:
    setParameter("G_K_max", G_K_max * 100);
    setParameter("E_K", E_K);
    setParameter("Junc. Pot.", jpot);
    setParameter("rate", rate);
    setState("a", a);
    setState("b", b);
    setState("Vm2", Vm);
    setState("Time (s)", systime);
    break;
  case MODIFY:
    G_K_max = getParameter("G_K_max").toDouble() / 100;
    E_K = getParameter("E_K").toDouble();
    jpot = getParameter("Junc. Pot.").toDouble();
    rate = getParameter("rate").toDouble();
    a = a_inf(-65);
    b = b_inf(-65);
    break;
  case PERIOD:
    period = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
  case PAUSE:
    output(0) = 0.0;
    break;
  case UNPAUSE:
    systime = 0;
    count = 0;
    break;
  default:
    break;
    }
}

void
gK::initParameters()
{
  Vm = -65;
  G_K_max = .477;
  E_K = -75.0;
  jpot = 10; // mV
  rate = 4;
  a = a_inf(Vm);
  b = b_inf(Vm);
  Iout = 0;
  output(0) = Iout;
  period = RT::System::getInstance()->getPeriod() * 1e-9; // s
  systime = 0;
  count = 0;
}
