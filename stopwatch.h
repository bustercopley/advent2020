// -*- C++ -*-
#ifndef stopwatch_h
#define stopwatch_h

#include <cstdio>
#include <string>

#if _WIN32

// Under MinGW64 / libstdc++, high_resolution_clock is not well implemented.

struct stopwatch {
  stopwatch() { ::QueryPerformanceCounter(&start); }

  double stop() {
    LARGE_INTEGER counter, frequency;
    ::QueryPerformanceCounter(&counter);
    ::QueryPerformanceFrequency(&frequency);

    return (double)(counter.QuadPart - start.QuadPart) /
           (double)frequency.QuadPart;
  }

  const std::string stamp() {
    double t = stop();
    std::string s(64, '\0');
    s.resize(std::snprintf(std::data(s), std::size(s), "%.7f seconds", t));
    return s;
  }

private:
  LARGE_INTEGER start;
};

#else

struct stopwatch {
  double stop() {
    return 0.0; // TO DO
  }

  const std::string stamp() {
    double t = stop();
    std::string s(64, '\0');
    s.resize(std::snprintf(std::data(s), std::size(s), "%.7f seconds", t));
    return s;
  }
};

#endif

#endif
