#ifndef MERGER_HPP
#define MERGER_HPP

#include <iosfwd>
#include <string>

namespace merger {

  class Merger
  {
  public:
    Merger(
      char sep,
      bool no_quotes,
      bool sort_by_time,
      bool drop_empty,
      std::size_t header_length,
      std::size_t key_field
    ) :
      separator{ sep },
      ignore_quotes{ (sep == '"') ? true : no_quotes },
      by_time{ sort_by_time },
      keep_empty{ !drop_empty },
      header{ header_length },
      key{ key_field }
    {}

    // merging of streams, using modified "merge-sort" algorithm:
    // "equal" lines will be merged into one
    void merge( std::istream &left, std::istream &right, std::ostream &out ) const;

  private:
    const char separator;
    const bool ignore_quotes, by_time, keep_empty;
    const std::size_t header, key;

    // compare the fields
    auto compare( std::string const&, std::string const& ) const;

    template <typename T>
    static auto check( T const&, T const& );

    template <typename Container, typename SepF>
    static void split( Container&, std::string const&, SepF const& );

    // counts number of separators, optionally excluding the quoted ones
    auto count_separators( std::string const& ) const;

    // generates string full of separators
    auto separators( std::string::size_type n ) const
    { return std::string( n, separator ); }
  };

} // namespace merger

#endif // MERGER_HPP
