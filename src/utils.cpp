#include "utils.hpp"

// C++ headers
#include <algorithm>

void toUpper( std::string& str ) {
  std::transform( str.begin(), str.end(), str.begin(), []( std::string::value_type c ) { return std::toupper( c ); } );
}

void toLower( std::string& str ) {
  std::transform( str.begin(), str.end(), str.begin(), []( std::string::value_type c ) { return std::tolower( c ); } );
}
