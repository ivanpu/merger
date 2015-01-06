#include <boost/date_time/posix_time/posix_time.hpp>
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

// compare the lines by first field, assuming it as time
enum class Cmp { less, equal, greater };
Cmp compare( std::string const& left, std::string const& right )
{
  using boost::posix_time::duration_from_string;

  auto l_time = duration_from_string( left.substr( 0, left.find( ',' ) ) );
  auto r_time = duration_from_string( right.substr( 0, right.find( ',' ) ) );

  if (l_time == r_time) return ::Cmp::equal;
  if (l_time < r_time) return ::Cmp::less;
  return ::Cmp::greater;
}

// counts number of commas, exulding the quoted ones
int count_commas( std::string const& str)
{
  auto count = 0;
  auto in_string = false;

  for (auto const& ch : str) {
    if (in_string) {
      if (ch == '"') in_string = false;

    } else {
      switch (ch) {
      case '"':
	in_string = true;
	break;

      case ',':
	++count;
	break;
      }
    }
  }

  return count;
}

// generates string full of commas
inline std::string commas( std::size_t n )
{
  return std::string( n, ',' );
}

// merging of streams, using modified "merge-sort" algorithm:
// 'equal' lines will be merged into one
void merge( std::istream &left, std::istream &right, std::ostream &out)
{
  std::string l_line, r_line;
  getline( left, l_line );
  getline( right, r_line );

  // using extra one comma as separator
  auto l_commas = commas( count_commas( l_line ) + 1 );
  auto r_commas = commas( count_commas( r_line ) + 1 );

  // merge from both streams, till we reach end of one of them
  Cmp cmp;
  while (!left.eof() && !right.eof()) {
    if (l_line[0] == '"' || (cmp = compare( l_line, r_line )) == Cmp::equal) {
      // lines "equal" - merge them into one
      out << l_line << ',' << r_line << "\n";
      getline( left, l_line );
      getline( right, r_line );

    } else if (cmp == Cmp::less) {
      // left is previous in time - output only it
      out << l_line << r_commas << "\n";
      getline( left, l_line );

    } else {
      // right is previous in time - output only it
      out << l_commas << r_line << "\n";
      getline( right, r_line );
    }
  }

  // dump remains of left file
  while (!left.eof()) {
    out << l_line << r_commas << "\n";
    getline( left, l_line );
  }

  // dump remains of right file
  while (!right.eof()) {
    out << l_commas << r_line << "\n";
    getline( right, r_line );
  }
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

    if (argc >= 4) {
      // optional output file specified - using it as output
      std::ofstream out;
      out.exceptions( std::ofstream::failbit | std::ofstream::badbit );
      out.open( argv[3] );
      out.exceptions( std::ofstream::badbit );

      merge( left, right, out );

    } else {
      // no output specified - using standart output
      merge( left, right, std::cout );
    }

  } catch (std::ios_base::failure const& e) {
    // I/O error occured - terminating
    // TODO: more informative message
    std::cerr << name << ": file I/O error: " << e.what() << std::endl;
    return 1;
  }
}
