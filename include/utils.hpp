#pragma once

// C++ headers
#include <memory>
#include <string>

template < class T, size_t N >
constexpr size_t arraySize( T ( & )[N] ) {
  return N;
}

void toUpper( std::string& str );
void toLower( std::string& str );

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
