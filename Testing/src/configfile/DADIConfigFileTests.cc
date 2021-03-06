/**
 * @file DADIConfigFileTests.cc
 * @brief This file implements the libdadi tests for configFile
 * @author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
 * @section License
 *  |LICENSE|
 *
 */

// C++ Headers
#include <iostream>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/test/unit_test.hpp>
#include "dadi/Config.hh"
#include "dadi/Options.hh"
#include "testconfig.h"

namespace bfs = boost::filesystem;  // an alias for boost filesystem namespace

BOOST_AUTO_TEST_SUITE(ConfigFileTests)

BOOST_AUTO_TEST_CASE(read_config_file_normal_call) {
  BOOST_TEST_MESSAGE("#Read config file test normal call#");

  int argc = 3;
  char *argv[argc];
  dadi::Options opt;
  dadi::Config& config = dadi::Config::instance();
  boost::filesystem::path configFilepath(TESTFILESOUTPUTPATH);
  configFilepath /= "cfg";
  configFilepath /= "configfile.cfg";

  opt.setName("simple-configfile");
  opt.setUsage("-c <config-file>");

  // we prefix command line and file options respectively with command and file
  /* command line options */
  boost::function0<void> fHelp(boost::bind(dadi::help, boost::cref(opt)));
  boost::function1<void, std::string> fFile(
    boost::bind(dadi::setPropertyString, "command.file", _1));
  /* configuration file options */
  boost::function1<void, std::string> fName(
    boost::bind(dadi::setPropertyString, "file.name", _1));
  dadi::OptionsGroup fileGroup("file configuration", true,
                               dadi::OptionsGroup::CFG);
  fileGroup.addOption("file.name", "name", fName);

  opt.addSwitch("help,h", "display help message", fHelp);
  // configuration file is a MANDATORY option
  opt.addOption("config-file,c", "configuration file", fFile, true);
  opt.addGroup(fileGroup);

  argv[0] = const_cast<char *>("./simple-configfile");
  argv[1] = const_cast<char *>("-c");
  argv[2] = const_cast<char *>(configFilepath.c_str());

  opt.parseCommandLine(argc, argv);
  opt.notify();

  // then get configuration file path
  std::string cfile = config.get<std::string>("command.file");
  // finally parse configuration file
  // if you want to allow unregistered options, set second arg to true
  opt.parseConfigFile(cfile, false);
  opt.notify();

  BOOST_REQUIRE_EQUAL(config.get<std::string>("file.name"), "<testConfigFile>");
}

BOOST_AUTO_TEST_SUITE_END()
