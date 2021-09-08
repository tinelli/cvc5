/******************************************************************************
 * Top contributors (to current version):
 *   Gereon Kremer
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Wrappers to handle memory management of streams.
 *
 * This file contains wrappers to handle special cases of managing memory
 * related to streams stored in options.
 */

#include "cvc5_public.h"

#ifndef CVC5__OPTIONS__MANAGED_STREAMS_H
#define CVC5__OPTIONS__MANAGED_STREAMS_H

#include <memory>
#include <ostream>

namespace cvc5 {

namespace detail {
/*
 * Open a file as an output stream and return it as a pointer. The caller
 * assumes the ownership of the returned pointer.
 */
std::ostream* openOStream(const std::string& filename);
/*
 * Open a file as an input stream and return it as a pointer. The caller
 * assumes the ownership of the returned pointer.
 */
std::istream* openIStream(const std::string& filename);
}  // namespace detail

/**
 * Implements memory management for streams, both input and output. It is
 * intended to be subclassed, where a subclass can provide a default value and
 * special cases. Usually, users should use one of these subclasses.
 * The template argument type should be either std::istream or std::ostream,
 * indicating whether the type wraps an input or output stream.
 */
template <typename Stream>
class ManagedStream
{
 public:
  ManagedStream() {}
  virtual ~ManagedStream() {}

  /**
   * Open the stream from the given value. First check the special cases and
   * then fall back to using `std::ofstream` or `std::ifstream`.
   */
  void open(const std::string& value)
  {
    if (specialCases(value)) return;
    if constexpr (std::is_same<Stream, std::ostream>::value)
    {
      d_stream.reset(detail::openOStream(value));
    }
    else if constexpr (std::is_same<Stream, std::istream>::value)
    {
      d_stream.reset(detail::openIStream(value));
    }
  }

  Stream& operator*() const { return *getPtr(); }
  Stream* operator->() const { return getPtr(); }
  operator Stream&() const { return *getPtr(); }
  operator Stream*() const { return getPtr(); }

 protected:
  std::shared_ptr<Stream> d_stream;

 private:
  /** Returns the value to be used if d_stream is not set. */
  virtual Stream* defaultValue() const = 0;
  /**
   * Check if there is a special case for this value. If so, the implementation
   * should set d_stream appropriately and return true to skip the default
   * methods for opening a stream.
   */
  virtual bool specialCases(const std::string& value) = 0;

  /** Return the pointer, either from d_stream of from defaultValue(). */
  Stream* getPtr() const
  {
    if (d_stream) return d_stream.get();
    return defaultValue();
  }
};

template <typename Stream>
std::ostream& operator<<(std::ostream& os, const ManagedStream<Stream>& ms)
{
  return os << "ManagedStream";
}

/**
 * Managed error output. It recognizes "stderr" and "--" as special valued for
 * std::cerr.
 */
class ManagedErr : public ManagedStream<std::ostream>
{
  std::ostream* defaultValue() const override final;
  bool specialCases(const std::string& value) override final;
};

/**
 * Managed standard input. It recognizes "stdin" and "--" as special valued for
 * std::cin.
 */
class ManagedIn : public ManagedStream<std::istream>
{
  std::istream* defaultValue() const override final;
  bool specialCases(const std::string& value) override final;
};

/**
 * Managed standard output. It recognizes "stdout" and "--" as special valued
 * for std::cout.
 */
class ManagedOut : public ManagedStream<std::ostream>
{
  std::ostream* defaultValue() const override final;
  bool specialCases(const std::string& value) override final;
};

}  // namespace cvc5

#endif /* CVC5__OPTIONS__MANAGED_STREAMS_H */
