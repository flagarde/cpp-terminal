/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/args.hpp"

namespace Term
{

Term::Arguments::Arguments() {}  //NOLINT(hicpp-use-equals-default,modernize-use-equals-default)

Term::Argc::Argc() {}  //NOLINT(hicpp-use-equals-default,modernize-use-equals-default)

Term::Argc::operator unsigned int() { return static_cast<unsigned int>(Term::Arguments::argc()); }

Term::Argc::operator unsigned int() const { return static_cast<unsigned int>(Term::Arguments::argc()); }

std::string Term::Arguments::operator[](const std::size_t& arg) const { return m_args[arg]; }

}  // namespace Term
