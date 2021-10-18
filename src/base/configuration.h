/******************************************************************************
 * Top contributors (to current version):
 *   Morgan Deters, Gereon Kremer, Mathias Preiner
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Interface to a public class that provides compile-time information
 * about the cvc5 library.
 */

#include "cvc5_public.h"

#ifndef CVC5__CONFIGURATION_H
#define CVC5__CONFIGURATION_H

#include <string>
#include <vector>

#include "cvc5_export.h"

namespace cvc5 {

/**
 * Represents the (static) configuration of cvc5.
 */
class CVC5_EXPORT Configuration
{
 private:
  /** Private default ctor: Disallow construction of this class */
  Configuration();

  // these constants are filled in by the build system
  static const bool GIT_BUILD;
  static const bool CVC5_IS_RELEASE;
  static const char* const CVC5_VERSION;
  static const char* const CVC5_FULL_VERSION;
  static const char* const CVC5_GIT_INFO;

public:

  static std::string getName();

  static bool isDebugBuild();

  static constexpr bool isStatisticsBuild()
  {
#ifdef CVC5_STATISTICS_ON
    return true;
#else
    return false;
#endif
  }

  static bool isTracingBuild();

  static bool isDumpingBuild();

  static bool isMuzzledBuild();

  static bool isAssertionBuild();

  static bool isCoverageBuild();

  static bool isProfilingBuild();

  static bool isAsanBuild();

  static bool isUbsanBuild();

  static bool isTsanBuild();

  static bool isCompetitionBuild();

  static bool isStaticBuild();

  static std::string getPackageName();

  static std::string getVersionString();

  static std::string copyright();

  static std::string about();

  static bool licenseIsGpl();

  static bool isBuiltWithGmp();

  static bool isBuiltWithCln();

  static bool isBuiltWithGlpk();

  static bool isBuiltWithAbc();

  static bool isBuiltWithCryptominisat();

  static bool isBuiltWithKissat();

  static bool isBuiltWithEditline();

  static bool isBuiltWithPoly();

  /* Return a sorted array of the debug tags name */
  static const std::vector<std::string>& getDebugTags();
  /* Test if the given argument is a known debug tag name */
  static bool isDebugTag(const std::string& tag);

  /* Return a sorted array of the trace tags name */
  static const std::vector<std::string>& getTraceTags();
  /* Test if the given argument is a known trace tag name */
  static bool isTraceTag(const std::string& tag);

  static bool isGitBuild();
  static std::string getGitInfo();

  static std::string getCompiler();
  static std::string getCompiledDateTime();

}; /* class Configuration */

}  // namespace cvc5

#endif /* CVC5__CONFIGURATION_H */
