#pragma once

// C++ headers
#include <cstdio>
#include <string>

class Log {
  public:
  Log() = delete;
  Log( Log const& ) = delete;
  Log( Log&& ) = delete;
  Log& operator=( Log const& ) = delete;
  Log& operator=( Log&& ) = delete;

  static void Init();

  static void LogLine( std::string const& level, std::string const& formatString );
  template < class... T >
  static void LogLine( std::string const& level, std::string const& formatString, T... args );

  static void Trace( std::string const& formatString );
  template < class... T >
  static void Trace( std::string const& formatString, T... args );
  static void Debug( std::string const& formatString );
  template < class... T >
  static void Debug( std::string const& formatString, T... args );
  static void Warning( std::string const& formatString );
  template < class... T >
  static void Warning( std::string const& formatString, T... args );
  static void Error( std::string const& formatString );
  template < class... T >
  static void Error( std::string const& formatString, T... args );

  private:
  static FILE* file_;
};

void Log::LogLine( std::string const& level, std::string const& formatString ) {
  std::fprintf( file_, "[%s] %s\n", level.c_str(), formatString.c_str() );
  std::fflush( file_ );
}

template < class... T >
void Log::LogLine( std::string const& level, std::string const& formatString, T... args ) {
  std::fprintf( file_, "[%s] ", level.c_str() );
  std::fprintf( file_, formatString.c_str(), args... );
  std::fprintf( file_, "\n" );
  std::fflush( file_ );
}

void Log::Trace( std::string const& formatString ) {
  return LogLine( "trace", formatString );
}

template < class... T >
void Log::Trace( std::string const& formatString, T... args ) {
  return LogLine< T... >( "trace", formatString, args... );
}

void Log::Debug( std::string const& formatString ) {
  return LogLine( "debug", formatString );
}

template < class... T >
void Log::Debug( std::string const& formatString, T... args ) {
  return LogLine< T... >( "debug", formatString, args... );
}

void Log::Warning( std::string const& formatString ) {
  return LogLine( "warning", formatString );
}

template < class... T >
void Log::Warning( std::string const& formatString, T... args ) {
  return LogLine< T... >( "warning", formatString, args... );
}

void Log::Error( std::string const& formatString ) {
  return LogLine( "error", formatString );
}

template < class... T >
void Log::Error( std::string const& formatString, T... args ) {
  return LogLine< T... >( "error", formatString, args... );
}
