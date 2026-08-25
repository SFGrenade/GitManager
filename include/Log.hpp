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
  static void DeInit();

  static void LogLine( std::string const& level, std::string const& outputString );
  template < class... T >
  static void LogLine( std::string const& level, std::string const& formatString, T... args );

  static void Trace( std::string const& outputString );
  template < class... T >
  static void Trace( std::string const& formatString, T... args );
  static void Debug( std::string const& outputString );
  template < class... T >
  static void Debug( std::string const& formatString, T... args );
  static void Warning( std::string const& outputString );
  template < class... T >
  static void Warning( std::string const& formatString, T... args );
  static void Error( std::string const& outputString );
  template < class... T >
  static void Error( std::string const& formatString, T... args );

  private:
  static void StartLogLine( std::string const& level );
  static void EndLogLine();

  private:
  static FILE* file_;
};

template < class... T >
void Log::LogLine( std::string const& level, std::string const& formatString, T... args ) {
  StartLogLine( level );
  std::fprintf( file_, formatString.c_str(), args... );
  EndLogLine();
}

template < class... T >
void Log::Trace( std::string const& formatString, T... args ) {
  return LogLine< T... >( "trace", formatString, args... );
}

template < class... T >
void Log::Debug( std::string const& formatString, T... args ) {
  return LogLine< T... >( "debug", formatString, args... );
}

template < class... T >
void Log::Warning( std::string const& formatString, T... args ) {
  return LogLine< T... >( "warning", formatString, args... );
}

template < class... T >
void Log::Error( std::string const& formatString, T... args ) {
  return LogLine< T... >( "error", formatString, args... );
}
