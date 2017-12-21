#ifndef MERGER_HPP
#define MERGER_HPP

#include <iosfwd>
#include <string>

namespace merger {

  class Merger
  {
  public:
    Merger( char sep, bool no_quotes, bool sort_by_time, bool drop_empty, std::size_t header_length ) :
      separator{ sep },
      ignore_quotes{ (sep == '"') ? true : no_quotes },
      by_time{ sort_by_time },
      keep_empty{ !drop_empty },
      header{ header_length }
    {}

    // merging of streams, using modified "merge-sort" algorithm:
    // "equal" lines will be merged into one
    void merge( std::istream &left, std::istream &right, std::ostream &out ) const;

  private:
    const char separator;
    const bool ignore_quotes, by_time, keep_empty;
    const std::size_t header;

    // compare the lines by first field
    enum class Cmp { less, equal, greater };
    Cmp compare( std::string const&, std::string const& ) const;

    // counts number of separators, optionally excluding the quoted ones
    std::string::size_type count_separators( std::string const& ) const;

    // generates string full of separators
    std::string separators( std::string::size_type n ) const
    { return std::string( n, separator ); }
  };

} // namespace merger

#endif // MERGER_HPP
