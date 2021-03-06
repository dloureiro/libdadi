/**
 * @file DADIConfigTests.cc
 * @brief This file implements the libdadi tests for configFile
 * @author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
 * @author Ibrahima Cissé (ibrahima.cisse@sysfera.com)
 * @author Benjamin Depardon <benjamin.depardon@sysfera.com>
 * @section License
 *  |LICENSE|
 *
 */

#include "dadi/Exception/Parameters.hh"
#include "dadi/Config.hh"
#include "DADIConfigTestsFixtures.hh"

namespace bfs = boost::filesystem;  // an alias for boost filesystem namespace

BOOST_AUTO_TEST_SUITE(ConfigTests)

BOOST_AUTO_TEST_CASE(read_config_file_normal_call) {
  BOOST_TEST_MESSAGE("#Read config file test normal call#");

  int argc = 3;
  char *argv[argc];
  dadi::Options opt;
  dadi::Config& config = dadi::Config::instance();
  boost::filesystem::path configFilePath(TESTFILESOUTPUTPATH);
  configFilePath /= "cfg";
  configFilePath /= "configfile.cfg";

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
  argv[2] = const_cast<char *>(configFilePath.c_str());

  opt.parseCommandLine(argc, argv);
  opt.notify();
  // check  if there is file.name key in the config
  BOOST_REQUIRE(config.get_optional<std::string>("command.file"));
  // then get configuration file path
  std::string cfile = config.get<std::string>("command.file");
  // finally parse configuration file
  // if you want to allow unregistered options, set second arg to true
  opt.parseConfigFile(cfile, false);
  opt.notify();

  BOOST_REQUIRE(config.get_optional<std::string>("file.name"));
  BOOST_REQUIRE_EQUAL(config.get<std::string>("file.name"),
                      "<testConfigFile>");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

/**************************************************************/

// test load member function
BOOST_AUTO_TEST_CASE(load_config_file_test) {
  BOOST_TEST_MESSAGE("[config Loader] LOAD CONFIG BEGIN");
  dadi::Config& config = dadi::Config::instance();
  // build the input file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path inputFilePath = configFileDir / "inputFile.cfg";
  // ensure that inputFile does not exist
  if (bfs::exists(inputFilePath)) {
    bfs::remove(bfs::path(inputFilePath));
  }
  // write some setting in the inputFile
  {
    std::ofstream ofs(inputFilePath.native().c_str());
    ofs << "diet\n {\n version 3.0 \n }";
  }
  // load this file
  {
    std::ifstream ifs(inputFilePath.native().c_str());
    config.load(ifs);
  }
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string> ("diet.version"), "3.0");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

BOOST_AUTO_TEST_CASE(load_bad_config_file_test) {
  BOOST_TEST_MESSAGE("[config Loader] LOAD BAD CONFIG BEGIN");
  dadi::Config& config = dadi::Config::instance();
  // build the input file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path inputFilePath = configFileDir / "inputFile.cfg";
  // ensure that inputFile does not exist
  if (bfs::exists(inputFilePath)) {
    bfs::remove(bfs::path(inputFilePath));
  }
  // write some setting in the inputFile
  {
    std::ofstream ofs(inputFilePath.native().c_str());
    ofs << "diet= version sdfsf3.1 \n }";
  }
  // load this file
  {
    std::ifstream ifs(inputFilePath.native().c_str());
    BOOST_REQUIRE_THROW(config.load(ifs), dadi::ParsingAttributeError);
  }

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}



// test  save member function
BOOST_AUTO_TEST_CASE(save_config_file_test_INFO) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "3.0");
  // build the output file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path outputFilePath = configFileDir/"outputFile.cfg";
  // ensure that outputFile does not exist
  if (bfs::exists(outputFilePath)) {
    bfs::remove(bfs::path(outputFilePath));
  }

  // write some setting in the inputFile
  {
    std::ofstream ofs(outputFilePath.native().c_str());
    // save the config
    config.save(ofs, dadi::FORMAT_INFO);
    bfs::ifstream ifs(outputFilePath);
  }
  config.clear();
  // check the config again
  BOOST_CHECK_THROW(config.get<std::string> ("diet.version"),
                    dadi::UnknownParameterError);
  // load this file
  {
    std::ifstream ifs(outputFilePath.native().c_str());
    config.load(ifs, dadi::FORMAT_INFO);
  }
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "3.0");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

BOOST_AUTO_TEST_CASE(save_config_file_test_XML) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "3.0");
  // build the output file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path outputFilePath = configFileDir/"outputFile.xml";
  // ensure that outputFile does not exist
  if (bfs::exists(outputFilePath)) {
    bfs::remove(bfs::path(outputFilePath));
  }

  // write some setting in the inputFile
  {
    std::ofstream ofs(outputFilePath.native().c_str());
    // save the config
    config.save(ofs, dadi::FORMAT_XML);
  }
  config.clear();
  // check the config again
  BOOST_CHECK_THROW(config.get<std::string> ("diet.version"),
                    dadi::UnknownParameterError);
  // load this file
  {
    std::ifstream ifs(outputFilePath.native().c_str());
    config.load(ifs, dadi::FORMAT_XML);
  }
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "3.0");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

BOOST_AUTO_TEST_CASE(save_config_file_test_JSON) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "3.0");
  // build the output file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path outputFilePath = configFileDir/"outputFile.json";
  // ensure that outputFile does not exist
  if (bfs::exists(outputFilePath)) {
    bfs::remove(bfs::path(outputFilePath));
  }

  // write some setting in the inputFile
  {
    std::ofstream ofs(outputFilePath.native().c_str());
    // save the config
    config.save(ofs, dadi::FORMAT_JSON);
  }
  config.clear();
  // check the config again
  BOOST_CHECK_THROW(config.get<std::string> ("diet.version"),
                    dadi::UnknownParameterError);
  // load this file
  {
    std::ifstream ifs(outputFilePath.native().c_str());
    config.load(ifs, dadi::FORMAT_JSON);
  }
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "3.0");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}


BOOST_AUTO_TEST_CASE(save_config_file_test_INI) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "3.0");
  // build the output file
  boost::filesystem::path configFileDir(TESTFILESOUTPUTPATH);
  configFileDir /= "cfg";
  bfs::path outputFilePath = configFileDir/"outputFile.ini";
  // ensure that outputFile does not exist
  if (bfs::exists(outputFilePath)) {
    bfs::remove(bfs::path(outputFilePath));
  }

  // write some setting in the inputFile
  {
    std::ofstream ofs(outputFilePath.native().c_str());
    // save the config
    config.save(ofs, dadi::FORMAT_INI);
  }
  config.clear();
  // check the config again
  BOOST_CHECK_THROW(config.get<std::string> ("diet.version"),
                    dadi::UnknownParameterError);
  // load this file
  {
    std::ifstream ifs(outputFilePath.native().c_str());
    config.load(ifs, dadi::FORMAT_INI);
  }
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "3.0");

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}



// test the clear member function
BOOST_AUTO_TEST_CASE(clear_config_file_test) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "3.0");
  // check the setting
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "3.0");
  // clear the config completely
  config.clear();
  // check the config again
  BOOST_CHECK_THROW(config.get<std::string> ("diet.version"),
                    dadi::UnknownParameterError);
  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

// test the get member function
BOOST_AUTO_TEST_CASE(get_config_file_test) {
  dadi::Config& config = dadi::Config::instance();
  // put some settings into the config store
  config.put("diet.version", "Cool version");
  // check the setting
  BOOST_CHECK_NO_THROW(config.get<std::string>("diet.version"));
  BOOST_CHECK_EQUAL(config.get<std::string>("diet.version"), "Cool version");
  // check the config again
  BOOST_CHECK_THROW(config.get<int> ("diet.version"),
                    dadi::InvalidParameterError);

  /* clear the config completely at the end of each test in order not
   * to have side effects
   */
  config.clear();
}

BOOST_AUTO_TEST_SUITE_END()
