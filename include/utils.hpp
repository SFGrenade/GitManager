#pragma once

// C++ headers
#include <string>

template < class T, size_t N >
constexpr size_t arraySize( T ( & )[N] ) {
  return N;
}

void toUpper( std::string& str );
void toLower( std::string& str );
