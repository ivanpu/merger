#include "merger.hpp"
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <istream>
#include <ostream>
#include <deque>
#include <cassert>

template <typename Container, typename SepF>
void merger::Merger::split( Container &out, std::string const& line, SepF const& sep )
{
  boost::tokenizer<SepF> tokens{ line, sep };
  out.clear();
  std::copy( tokens.begin(), tokens.end(), std::back_inserter(out) );
}

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
  std::deque<std::string> l_fields, r_fields;
  boost::char_separator<char> sep{ std::string{ separator }.c_str() };
  boost::escaped_list_separator<char> sep_esc{ '\\', separator };

  while (!left.eof() && !right.eof()) {
    // split fields
    if (ignore_quotes) {
      split( l_fields, l_line, sep );
      split( r_fields, r_line, sep );

    } else {
      split( l_fields, l_line, sep_esc );
      split( r_fields, r_line, sep_esc );
    }

    // find lines order
    auto skip_left  = l_fields.size() < key;
    auto skip_right = r_fields.size() < key;

    if (!skip_left && !skip_right) {
      auto cmp = compare( l_fields[key], r_fields[key] );

      skip_left  = cmp == Cmp::greater;
      skip_right = cmp == Cmp::less;
    }

    // print relevant lines
    if (skip_left == skip_right) {
      if (!skip_left) out << l_line << separator << r_line << '\n';
      getline( left, l_line );
      getline( right, r_line );

    } else if (skip_right) {
      assert( !skip_left );
      if (keep_empty) out << l_line << r_seps << '\n';
      getline( left, l_line );

    } else {
      assert( skip_left && !skip_right );
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

template <typename T>
auto merger::Merger::check( T const& l, T const& r ) -> Cmp
{
  if (l == r) return Cmp::equal;
  if (l < r) return Cmp::less;
  return Cmp::greater;
}

auto merger::Merger::compare( std::string const& l_field, std::string const& r_field ) const -> Cmp
{
  if (by_time) {
    namespace pt = boost::posix_time;
    return check(
      pt::duration_from_string( l_field ),
      pt::duration_from_string( r_field )
    );

  } else {
    return check(
      boost::lexical_cast<long>( l_field ),
      boost::lexical_cast<long>( r_field )
    );
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
