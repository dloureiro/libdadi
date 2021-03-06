/**
 * @file   Testing/src/plugin/PersonA.hh
 * @author Haïkel Guémar <haikel.guemar@sysfera.com>
 * @brief  IPerson plugin implementation
 * @section License
 *   |LICENSE|
 *
 */
#include <cstring>
#include <iostream>
#include "IPerson.hh"

class PersonA : public IPerson {
protected:
  virtual void
  do_init() {
    std::cout << "initialization of PersonA\n";
  }

  virtual void
  do_say_hello();
};

void
PersonA::do_say_hello() {
  std::cout << "Hello I'm PersonA!\n";
}


int
create_plugin_instance(void **instance) {
  try {
    *instance = new PersonA;
  } catch (const std::bad_alloc& e) {
    return 1;
  }

  return PLUGIN_OK;
}


