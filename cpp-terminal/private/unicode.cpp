/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/unicode.hpp"

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/private/exception.hpp"

#include <array>
#include <limits>
#include <string>

#if defined(_WIN32)
  #define NOMINMAX
  #include <windows.h>
#endif

#if defined(_WIN32)
std::string Term::Private::to_narrow(const std::wstring& in)
{
  if(in.empty()) return std::string();
  static constexpr DWORD flag{WC_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0, nullptr, nullptr)};
  if(ret_size == 0) throw Term::Private::WindowsException(::GetLastError());
  std::string ret(static_cast<std::size_t>(ret_size), '\0');
  int         ret_error{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size, nullptr, nullptr)};
  if(ret_error == 0) throw Term::Private::WindowsException(::GetLastError());
  return ret;
}

std::wstring Term::Private::to_wide(const std::string& in)
{
  if(in.empty()) return std::wstring();
  static constexpr DWORD flag{MB_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0)};
  if(ret_size == 0) throw Term::Private::WindowsException(::GetLastError());
  std::wstring ret(static_cast<std::size_t>(ret_size), '\0');
  int          ret_error{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size)};
  if(ret_error == 0) throw Term::Private::WindowsException(::GetLastError());
  return ret;
}
#endif

std::string Term::Private::utf32_to_utf8(const char32_t& codepoint, const bool& exception)
{
  static const constexpr std::array<std::uint32_t, 4> size{0x7F, 0x07FF, 0xFFFF, 0x10FFFF};
  static const constexpr std::uint8_t                 mask{0x80};
  static const constexpr std::uint8_t                 add{0x3F};
  static const constexpr std::array<std::uint8_t, 3>  mask_first{0x1F, 0x0F, 0x07};
  static const constexpr std::array<std::uint8_t, 3>  add_first{0xC0, 0xE0, 0xF0};
  static const constexpr std::array<std::uint8_t, 4>  shift{0, 6, 12, 18};
  static const constexpr std::uint8_t                 max_size{4};
  std::string                                         ret;
  ret.reserve(max_size);
  if(codepoint <= size[0]) { ret = {static_cast<char>(codepoint)}; }  // Plain ASCII
  else if(codepoint <= size[1]) { ret = {static_cast<char>(((codepoint >> shift[1]) & mask_first[0]) | add_first[0]), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(codepoint <= size[2]) { ret = {static_cast<char>(((codepoint >> shift[2]) & mask_first[1]) | add_first[1]), static_cast<char>(((codepoint >> shift[1]) & add) | mask), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(codepoint <= size[3]) { ret = {static_cast<char>(((codepoint >> shift[3]) & mask_first[2]) | add_first[2]), static_cast<char>(((codepoint >> shift[2]) & add) | mask), static_cast<char>(((codepoint >> shift[1]) & add) | mask), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(exception) { throw Term::Exception("Invalid UTF32 codepoint."); }
  else { ret = "\xEF\xBF\xBD"; }
  return ret;
}

std::string Term::Private::utf32_to_utf8(const std::u32string& str, const bool& exception)
{
  std::string ret;
  for(const char32_t codepoint: str) { ret.append(utf32_to_utf8(codepoint, exception)); }
  return ret;
}

std::u32string Term::Private::utf8_to_utf32(const char& character, const bool& exception)
{
  static const constexpr std::uint8_t max_ascii_value{127};
  if('\0' == character) { return {}; }
  if((std::numeric_limits<char>::max() > max_ascii_value) && (character > max_ascii_value))
  {
    if(exception) { throw Term::Exception("the char is not in UTF-8 range."); }
    return U"\U0000FFFD";
  }
  if((std::numeric_limits<char>::lowest() < 0) && (character < 0))
  {
    if(exception) { throw Term::Exception("the char is not in UTF-8 range."); }
    return U"\U0000FFFD";
  }
  return {static_cast<char32_t>(character)};
}

std::size_t Term::Private::utf8_codepoint_size(const std::uint8_t& str) noexcept
{
  static const constexpr std::array<std::uint8_t, 4> mask{128, 224, 240, 248};
  static const constexpr std::array<std::uint8_t, 4> compare{0, 192, 224, 240};
  if((str & mask[0]) == compare[0]) { return 1; }
  if((str & mask[1]) == compare[1]) { return 2; }
  if((str & mask[2]) == compare[2]) { return 3; }
  if((str & mask[3]) == compare[3]) { return 4; }
  return 0;
}

std::u32string Term::Private::utf8_to_utf32(const std::string& str, const bool& exception)
{
  static const constexpr std::array<std::uint8_t, 5> mask{63, 127, 31, 15, 7};
  static const constexpr std::array<std::uint8_t, 4> shift{0U, 6U, 12U, 18U};
  if(str.empty()) { return {}; }
  std::u32string ret;
  for(std::size_t byte = 0; byte < str.size();)
  {
    bool        error{false};
    std::size_t byte_count{utf8_codepoint_size(static_cast<std::uint8_t>(str[byte]))};
    switch(byte_count)
    {
      case 1:
      {
        if(is_valid_utf8(str.begin() + byte, str.begin() + byte + 1)) { ret.push_back(static_cast<std::uint8_t>(str[byte] & mask[1]) << shift[0]); }
        else { error = true; }
        break;
      }
      case 2:
      {
        if(is_valid_utf8(str.begin() + byte, str.begin() + byte + 2)) { ret.push_back((static_cast<std::uint8_t>(str[byte] & mask[2]) << shift[1]) | (static_cast<std::uint8_t>(str[byte + 1] & mask[0]) << shift[0])); }
        else { error = true; }
        break;
      }
      case 3:
      {
        if(is_valid_utf8(str.begin() + byte, str.begin() + byte + 3)) { ret.push_back((static_cast<std::uint8_t>(str[byte] & mask[3]) << shift[2]) | (static_cast<std::uint8_t>(str[byte + 1] & mask[0]) << shift[1]) | (static_cast<std::uint8_t>(str[byte + 2] & mask[0]) << shift[0])); }
        else { error = true; }
        break;
      }
      case 4:
      {
        if(is_valid_utf8(str.begin() + byte, str.begin() + byte + 4)) { ret.push_back((static_cast<std::uint8_t>(str[byte] & mask[4]) << shift[3]) | (static_cast<std::uint8_t>(str[byte + 1] & mask[0]) << shift[2]) | (static_cast<std::uint8_t>(str[byte + 2] & mask[0]) << shift[1]) | (static_cast<std::uint8_t>(str[byte + 3] & mask[0]) << shift[0])); }
        else { error = true; }
        break;
      }
      default:
      {
        ++byte;
        error = true;
      }
    }
    if(error)
    {
      if(exception) { throw Term::Exception("Bad UTF-8 sequence."); }
      ret.push_back(U'\U0000FFFD');
    }
    byte += byte_count;
  }
  return ret;
}

bool Term::Private::is_valid_utf8_code_unit(const std::string& str) { return utf8_codepoint_size(str[0]) == str.size() && is_valid_utf8(str.begin(), str.end()); }

bool Term::Private::is_valid_utf8(const std::string::const_iterator& begin, const std::string::const_iterator& end)
{
  static const constexpr std::uint8_t b1OOOOOOO{128};
  static const constexpr std::uint8_t b11OOOOOO{192};
  static const constexpr std::uint8_t b111OOOOO{224};
  static const constexpr std::uint8_t b1111OOOO{240};
  static const constexpr std::uint8_t b11111OOO{248};
  if(begin > end) { return false; }
  for(std::string::const_iterator it = begin; it != end;)
  {
    std::size_t byte_count{utf8_codepoint_size(*it)};
    if(it + byte_count > end) { return false; }
    switch(byte_count)
    {
      case 1:
      {
        if((static_cast<std::uint8_t>(*it) & b1OOOOOOO) != 0) return false;
        break;
      }
      case 2:
      {
        if(((static_cast<std::uint8_t>(*it) & b111OOOOO) != b11OOOOOO) || ((static_cast<std::uint8_t>(*(it + 1)) & b11OOOOOO) != b1OOOOOOO)) return false;
        break;
      }
      case 3:
      {
        if(((static_cast<std::uint8_t>(*it) & b1111OOOO) != b111OOOOO) || ((static_cast<std::uint8_t>(*(it + 1)) & b11OOOOOO) != b1OOOOOOO) || ((static_cast<std::uint8_t>(*(it + 2)) & b11OOOOOO) != b1OOOOOOO)) return false;
        break;
      }
      case 4:
      {
        if(((static_cast<std::uint8_t>(*it) & b11111OOO) != b1111OOOO) || ((static_cast<std::uint8_t>(*(it + 1)) & b11OOOOOO) != b1OOOOOOO) || ((static_cast<std::uint8_t>(*(it + 2)) & b11OOOOOO) != b1OOOOOOO) || ((static_cast<std::uint8_t>(*(it + 3)) & b11OOOOOO) != b1OOOOOOO)) return false;
        break;
      }
      default: return false;
    }
    it += byte_count;
  }
  return true;
}
