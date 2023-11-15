
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
#include <cstdint>
#include <string>

///
///@file unicode.hpp
///@brief Unicode encoder/decoder.
///@warning Internal use only.
///

namespace Term
{
namespace Private
{

#if defined(_WIN32)
///
///@brief Convert a UTF-16 encoded \b std::wstring to a UTF-8 encoded \b std::string .
///
///@param wstr The UTF-16 \b std::wstring to convert to UTF-18 encoded \b std::string .
///@return std::string the UTF-8 encoded \b std::string .
///@warning Internal use only. And \b Windows specific.
///
std::string to_narrow(const std::wstring& wstr);

///
///@brief Convert a UTF-8 encoded \b std::string to a UTF-16 encoded \b std::wstring .
///
///@param str the UTF-8 string to convert to UTF-16 encoded \b std::wstring .
///@return std::wstring the UTF-16 encoded \b std::wstring .
///@warning Internal use only. And \b Windows specific.
///
std::wstring to_wide(const std::string& str);
#endif

///
///@brief Encode a codepoint using UTF-8 \b std::string .
///
///@param codepoint The codepoint ( \b char32_t ) on range [0,0x10FFFF] to convert.
///@param exception If \b true throw exception on error, otherwise change the out of range \b codepoint to "replacement character" \b � .
///@return \b std::string encoded in UTF-8.
///@warning Internal use only.
///
std::string utf32_to_utf8(const char32_t& codepoint, const bool& exception = false);

///
///@brief Encode a \b std::u32string into UTF-8 \b std::string .
///
///@param str The \b std::u32string to convert.
///@param exception If \b true throw exception on error, otherwise change the out of range \b codepoint to "replacement character" \b � .
///@return \b std::string encoded in UTF-8.
///@warning Internal use only.
///
std::string utf32_to_utf8(const std::u32string& str, const bool& exception = false);

///
///@brief Encode a UTF-8 \b char to \b std::u32string .
///
///@param character The UTF-8 char on range [0,127] to convert.
///@param exception If \b true throw exception on error, otherwise change the out of range \b codepoint to "replacement character" \b � .
///@return \b std::u32string the UTF-32 string.
///@warning Internal use only.
///
std::u32string utf8_to_utf32(const char& character, const bool& exception = false);

///
///@brief Encode a UTF-8 \b std::string to \b std::u32string .
///
///@param str The UTF-8 \b std::string to convert.
///@param exception If \b true throw exception on error, otherwise change the \b char not into UTF-8 sequence to "replacement character" \b � .
///@return \b std::u32string the UTF-32 string.
///@warning Internal use only.
///
std::u32string utf8_to_utf32(const std::string& str, const bool& exception = false);

///
///@brief check if the \b std::string is a good UTF-8 encoded code-unit.
///
///@param str The \b std::string to check. (if the string is >=5 char it is not a code-unit).
///@return true The string is a code-unit encoded in UTF-8.
///@return false The string is \b NOT a code-unit encoded in UTF-8.
///@warning Internal use only.
///
bool is_valid_utf8_code_unit(const std::string& str);

///
///@brief Check if it is UTF-8 encoded.
///
///@param begin The start of the sequence to test.
///@param end The en of the sequence to test.
///@return true The sequence is UTF-8 encoded.
///@return false The sequence is ill-UTF-8 encoded.
///@warning Internal use only.
///
bool is_valid_utf8(const std::string::const_iterator& begin, const std::string::const_iterator& end);

///
///@brief The size of the UTF-8 codepoint.
///
///@param begin The first value of the UTF-8 encoded code-point.
///@return \b std::size_t The size of the UTF-8 encoded code-point.
///@warning Internal use only.
///@warning The function assume the sequence is a valid UTF-8.
///
std::size_t utf8_codepoint_size(const std::uint8_t& begin) noexcept;

}  // namespace Private
}  // namespace Term
