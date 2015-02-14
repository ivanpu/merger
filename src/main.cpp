#include "merger.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>

// program version
const auto version = "0.1";

// general information for users
void usage( std::string const& name )
{
  std::cerr << name << " - version " << version << "\n"
	    << "Usage: " << name
	    << " <left-file> <right-file> [<output-file>]"
	    << std::endl;
  exit(0);
}

int main( int argc, char* argv[] )
{
  // executable name
  auto name = boost::filesystem::path{ argv[0] }.filename().string();

  // check for correct number of parameters
  if (argc < 3 || argc > 4) usage( name );

  std::ifstream left, right;
  // we need to check for failbit in case that opening failed...
  left.exceptions( std::ifstream::failbit | std::ifstream::badbit );
  right.exceptions( std::ifstream::failbit | std::ifstream::badbit );

  try {
    left.open( argv[1] );
    right.open( argv[2] );

    // ... but failbit is set in some EOF conditions by getline,
    // so disable its checking after file is open
    left.exceptions( std::ifstream::badbit );
    right.exceptions( std::ifstream::badbit );

    merger::Merger m{ ',' };
    
    if (argc >= 4) {
      // optional output file specified - using it as output
      std::ofstream out;
      out.exceptions( std::ofstream::failbit | std::ofstream::badbit );
      out.open( argv[3] );
      out.exceptions( std::ofstream::badbit );

      m.merge( left, right, out );

    } else {
      // no output specified - using standart output
      m.merge( left, right, std::cout );
    }

  } catch (std::ios_base::failure const& e) {
    // I/O error occured - terminating
    // TODO: more informative message
    std::cerr << name << ": file I/O error: " << e.what() << std::endl;
    return 1;
  }
}
