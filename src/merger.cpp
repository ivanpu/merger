#include "merger.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <istream>
#include <ostream>

void merger::Merger::merge( std::istream &left, std::istream &right, std::ostream &out ) const
{
  std::string l_line, r_line;
  getline( left, l_line );
  getline( right, r_line );

  // skip && copy header
  for (std::size_t i = 0; i < header; ++i) {
    out << l_line << '\n';
    getline( left, l_line );
    getline( right, r_line );
  }

  // using one extra separator
  auto const l_seps = separators( count_separators( l_line ) + 1 );
  auto const r_seps = separators( count_separators( r_line ) + 1 );

  // merge from both streams, till we reach end of one of them
  Cmp cmp;
  while (!left.eof() && !right.eof()) {
    if (l_line[0] == '"' || (cmp = compare( l_line, r_line )) == Cmp::equal) {
      // lines "equal" - merge them into one
      out << l_line << separator << r_line << '\n';
      getline( left, l_line );
      getline( right, r_line );

    } else if (cmp == Cmp::less) {
      // left is previous in time - output only it
      if (keep_empty) out << l_line << r_seps << '\n';
      getline( left, l_line );

    } else {
      // right is previous in time - output only it
      if (keep_empty) out << l_seps << r_line << '\n';
      getline( right, r_line );
    }
  }

  if (keep_empty) {
    // dump remains of left file
    while (!left.eof()) {
      out << l_line << r_seps << '\n';
      getline( left, l_line );
    }

    // dump remains of right file
    while (!right.eof()) {
      out << l_seps << r_line << '\n';
      getline( right, r_line );
    }
  }
}

auto merger::Merger::compare( std::string const& left, std::string const& right ) const -> Cmp
{
  if (by_time) {
    using boost::posix_time::duration_from_string;

    auto l_time = duration_from_string( left.substr( 0, left.find( separator ) ) );
    auto r_time = duration_from_string( right.substr( 0, right.find( separator ) ) );

    if (l_time == r_time) return Cmp::equal;
    if (l_time < r_time) return Cmp::less;
    return Cmp::greater;

  } else {
    auto l_val = boost::lexical_cast<long>(left.substr( 0, left.find( separator ) ));
    auto r_val = boost::lexical_cast<long>(right.substr( 0, right.find( separator ) ));

    if (l_val == r_val) return Cmp::equal;
    if (l_val < r_val) return Cmp::less;
    return Cmp::greater;
  }
}

std::string::size_type merger::Merger::count_separators( std::string const& str ) const
{
  std::string::size_type count = 0;
  auto in_string = false;
  auto escaped = false;

  for (auto const& ch : str) {
    if (ch == '\\') {
      // this deals well with sequence of backslashes:
      escaped = !escaped;

    } else if (!escaped) {
      if (in_string) {
        if (ch == '"') in_string = false;

      } else if (ch == '"' && !ignore_quotes) {
        in_string = true;

      } else if (ch == separator) {
        ++count;
      }

    } else {
      escaped = false;
    }
  }

  return count;
}
