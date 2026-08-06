#include "methods.h"
#include "meta.h"
#include <sstream>
#include <string>

std::string getVerString() {
  std::ostringstream ver_string;
  ver_string << c_assemble_appver.maj << "." << c_assemble_appver.min << "."
             << c_assemble_appver.fix;

  return ver_string.str();
}
