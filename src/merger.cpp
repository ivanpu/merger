#include "merger.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <istream>
#include <ostream>

void merger::Merger::merge( std::istream &left, std::istream &right, std::ostream &out )
{
  std::string l_line, r_line;
  getline( left, l_line );
  getline( right, r_line );

  // skip && copy header
  for (std::size_t i = 0; i < header; ++i) {
    out << l_line << "\n";
    getline( left, l_line );
    getline( right, r_line );
  }

  // using extra one comma as separator
  auto l_seps = separators( count_separators( l_line ) + 1 );
  auto r_seps = separators( count_separators( r_line ) + 1 );

  // merge from both streams, till we reach end of one of them
  Cmp cmp;
  while (!left.eof() && !right.eof()) {
    if (l_line[0] == '"' || (cmp = compare( l_line, r_line )) == Cmp::equal) {
      // lines "equal" - merge them into one
      out << l_line << separator << r_line << "\n";
      getline( left, l_line );
      getline( right, r_line );

    } else if (cmp == Cmp::less) {
      // left is previous in time - output only it
      out << l_line << r_seps << "\n";
      getline( left, l_line );

    } else {
      // right is previous in time - output only it
      out << l_seps << r_line << "\n";
      getline( right, r_line );
    }
  }

  // dump remains of left file
  while (!left.eof()) {
    out << l_line << r_seps << "\n";
    getline( left, l_line );
  }

  // dump remains of right file
  while (!right.eof()) {
    out << l_seps << r_line << "\n";
    getline( right, r_line );
  }
}

auto merger::Merger::compare( std::string const& left, std::string const& right ) -> Cmp
{
  using boost::posix_time::duration_from_string;

  auto l_time = duration_from_string( left.substr( 0, left.find( separator ) ) );
  auto r_time = duration_from_string( right.substr( 0, right.find( separator ) ) );

  if (l_time == r_time) return Cmp::equal;
  if (l_time < r_time) return Cmp::less;
  return Cmp::greater;
}

int merger::Merger::count_separators( std::string const& str )
{
  auto count = 0;
  auto in_string = false;

  for (auto const& ch : str) {
    if (in_string) {
      if (ch == '"') in_string = false;

    } else if (ch == '"' && !ignore_quotes) {
      in_string = true;

    } else if (ch == separator) {
      ++count;
    }
  }

  return count;
}
