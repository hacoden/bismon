// file BM_makeconst.cc

/***
    BM_makeconst: a program to scan Bm_Const macros
    Copyright (C) 2017 Basile Starynkevitch

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

***/
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>

int main(int argc, char**argv) {
  if (argc < 3) {
    fprintf(stderr, "%s expects at least three arguments:\n -H <generated-header> <C-file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, __FILE__ " @@running: ");
  for (int ix=0; ix<argc; ix++) fprintf(stderr, " %s", argv[ix]);
  fputc('\n', stderr);
  fflush(stderr);
  if (!strcmp(argv[1], "-H")) {
    auto hpath = argv[2];
    std::ofstream outh(hpath);
    outh << "#warning " __FILE__ << " should generate " << hpath << std::endl;
  }
  return 0;
}
     
  
