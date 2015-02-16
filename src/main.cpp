#include "merger.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>

// program version
const auto version = "0.2alpha";

void merge( boost::program_options::variables_map const& vm )
{
  std::ifstream left, right;
  // we need to check for failbit in case that opening failed...
  left.exceptions( std::ifstream::failbit | std::ifstream::badbit );
  right.exceptions( std::ifstream::failbit | std::ifstream::badbit );

  left.open( vm["left"].as<std::string>() );
  right.open( vm["right"].as<std::string>() );

  // ... but failbit is set in some EOF conditions by getline,
  // so disable its checking after file is open
  left.exceptions( std::ifstream::badbit );
  right.exceptions( std::ifstream::badbit );

  merger::Merger m{
    vm["separator"].as<char>(),
    vm["ignore-quotes"].as<bool>()
  };

  if (vm.count( "out" )) {
    // optional output file specified - using it as output
    std::ofstream out;
    out.exceptions( std::ofstream::failbit | std::ofstream::badbit );
    out.open( vm["out"].as<std::string>() );
    out.exceptions( std::ofstream::badbit );

    m.merge( left, right, out );

  } else {
    // no output specified - using standart output
    m.merge( left, right, std::cout );
  }
}

int main( int argc, char* argv[] )
{
  // executable name
  auto name = boost::filesystem::path{ argv[0] }.filename().string();

  try {
    // parse parameters
    namespace po = boost::program_options;

    po::options_description desc{ "Allowed options" };
    desc.add_options()
      ( "help", "print this help message" )
      ( "version", "print program version" )
      ( "separator,s", po::value<char>()->default_value(','), "set separator" )
      ( "ignore-quotes", po::value<bool>()->default_value(false), "no special treatment for quoted text" )
      ;
    po::options_description hidden{ "Hidden options" };
    hidden.add_options()
      ( "left", po::value<std::string>(), "left file" )
      ( "right", po::value<std::string>(), "right file" )
      ( "out", po::value<std::string>(), "output file" )
      ;

    po::options_description cmdline;
    cmdline.add( desc ).add( hidden );

    po::positional_options_description p;
    p.add( "left", 1 ).add( "right", 1 ).add( "out", 1 );

    po::variables_map vm;
    po::store( po::command_line_parser{ argc, argv }.options( cmdline ).positional( p ).run(), vm );
    po::notify( vm );

    // check parameters
    if (vm.count( "help" )) {
      std::cout << "Usage:\n  " << name << " [OPTIONS] LEFT-FILE RIGHT-FILE [OUTPUT]\n\n"
                << desc << std::endl;
      return 0;
    }

    if (vm.count( "version" )) {
      std::cout << name << " version: " << version << std::endl;
      return 0;
    }

    if (!vm.count( "left" ) || !vm.count( "right" )) {
      std::cerr << name << ": at least 2 files have to be provided" << std::endl;
      return 1;
    }

    // the actual program
    merge( vm );

  } catch (std::ios_base::failure const& e) {
    // I/O error occured - terminating
    // TODO: more informative message
    std::cerr << name << ": file I/O error: " << e.what() << std::endl;
    return 1;

  } catch (std::exception const& e) {
    // some other error occured
    std::cerr << name << ": " << e.what() << std::endl;
    return 1;
  }
}
