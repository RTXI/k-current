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

#include <default_gui_model.h>
#include <math.h>
#include <string>

class gK : public DefaultGUIModel
{

public:

  gK(void);
  virtual
  ~gK(void);

  virtual void
  execute(void);

protected:

  virtual void
  update(DefaultGUIModel::update_flags_t);

private:

  void
  initParameters();
  double Vm;
  double a;
  double b;
  double da;
  double db;
  double period;
  int steps;
  double G_K_max;
  double E_K;
  double rate;
  double jpot;
  double dt;
  double Iout;
  double systime;
  long long count;
};
