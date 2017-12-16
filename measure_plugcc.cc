// file measure_plugcc.cc
//  Copyright (C) 2017 Free Software Foundation, Inc.

/*
This measure_plugcc.cc file is a GCC plugin doing simple measures.
measure_plugcc.cc is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

measure_plugcc.cc is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include <iostream>
#include <string>
#include "gcc-plugin.h"
#include "plugin-version.h"
extern "C" int plugin_is_GPL_compatible;
int plugin_is_GPL_compatible=1;

static void
start_unit_cb(void *gcc_data, void *)
{
} // end start_unit_cb

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  plugin_info->version = "measure_plugcc " __DATE__;
  plugin_info->help = "measure_plugcc is measuring";
  if (!plugin_default_version_check (version, &gcc_version))
    return 1;
  register_callback(plugin_info->base_name, PLUGIN_START_UNIT, start_unit_cb, nullptr);
  return  0;
}
