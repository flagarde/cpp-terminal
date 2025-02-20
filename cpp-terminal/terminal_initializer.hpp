/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstddef>

namespace Term
{

class TerminalInitializer
{
public:
  ~TerminalInitializer();
  TerminalInitializer();
  TerminalInitializer(const TerminalInitializer&)            = delete;
  TerminalInitializer(TerminalInitializer&&)                 = delete;
  TerminalInitializer& operator=(TerminalInitializer&&)      = delete;
  TerminalInitializer& operator=(const TerminalInitializer&) = delete;

private:
  static std::size_t m_counter;
};

}  // namespace Term
