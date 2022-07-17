#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(BUILDING_SOURCERER)
#define SOURCERER_API __declspec(dllexport)
#else
#define SOURCERER_API __declspec(dllimport)
#endif
#else
#ifdef BUILDING_SOURCERER
#define SOURCERER_API __attribute__((visibility("default")))
#else
#define SOURCERER_API
#endif
#endif
