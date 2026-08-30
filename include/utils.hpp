#pragma once

// C++ headers
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>

namespace std {

std::string to_string( std::filesystem::path const& path );

}

template < class T, size_t N >
constexpr size_t arraySize( T ( & )[N] ) {
  return N;
}

template < class T >
void toUpper( T& str ) {
  std::transform( str.begin(), str.end(), str.begin(), []( T::value_type c ) { return std::toupper( c ); } );
}

template < class T >
void toLower( T& str ) {
  std::transform( str.begin(), str.end(), str.begin(), []( T::value_type c ) { return std::tolower( c ); } );
}

#define RAW_PTR_TO_SHARED_PTR( type, name, deleteMethod, createMethod, ... ) \
  std::shared_ptr< type > name;                                              \
  {                                                                          \
    type* r_##name = nullptr;                                                \
    if( int error = createMethod( __VA_ARGS__ ); error < 0 ) {               \
      PrintGitError( #createMethod, error );                                 \
      return;                                                                \
    }                                                                        \
    name = std::shared_ptr< type >( r_##name, []( type* p ) {                \
      if( p )                                                                \
        deleteMethod( p );                                                   \
    } );                                                                     \
  }
