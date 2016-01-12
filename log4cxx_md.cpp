#include "log4cxx_md.hpp"

log4cxx_md::log4cxx_md() {

}

log4cxx_md::~log4cxx_md() {

}

void log4cxx_md::init() {
  DOMConfigurator::configure("../config/log_config.xml");
}
