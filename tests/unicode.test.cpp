/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/private/unicode.hpp"

#include "cpp-terminal/exception.hpp"
#include "doctest/doctest.h"

#include <string>

#if defined(_WIN32)
TEST_CASE("to_narrow")
{
  std::wstring in(L"Hello World; 你好; おはようございます; בוקר טוב");
  std::string  out{Term::Private::to_narrow(in)};
  CHECK(out == (const char*)(u8"Hello World; 你好; おはようございます; בוקר טוב"));
}
#endif

#if defined(_WIN32)
TEST_CASE("to_wide")
{
  std::string in((const char*)(u8"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n"));  // Some multi-language charabia
  std::wstring out{Term::Private::to_wide(in)};
  CHECK(out == L"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n");
}
#endif

TEST_CASE("utf32 to utf8")
{
  char32_t    codepoint = U'\u0001';
  std::string s{Term::Private::utf32_to_utf8(codepoint)};
  CHECK(s.size() == 1);
  CHECK(s[0] == '\x01');
  CHECK(s == (const char*)u8"\u0001");

  codepoint = U'\u0080';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 2);
  CHECK(s[0] == '\xC2');
  CHECK(s[1] == '\x80');
  CHECK(s == (const char*)u8"\u0080");

  codepoint = U'\u0800';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 3);
  CHECK(s[0] == '\xE0');
  CHECK(s[1] == '\xA0');
  CHECK(s[2] == '\x80');
  CHECK(s == (const char*)u8"\u0800");

  codepoint = U'\U0010FFFF';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 4);
  CHECK(s[0] == '\xF4');
  CHECK(s[1] == '\x8F');
  CHECK(s[2] == '\xBF');
  CHECK(s[3] == '\xBF');
  CHECK(s == (const char*)u8"\U0010FFFF");

  std::uint32_t out_of_range{0x110000};
  s = Term::Private::utf32_to_utf8(out_of_range, false);
  CHECK(s.size() == 3);
  CHECK(s[0] == '\xEF');
  CHECK(s[1] == '\xBF');
  CHECK(s[2] == '\xBD');
  //CHECK(s == (const char*)u8"\xEF\xBF\xBD"); // FIXME windows

  try
  {
    s = Term::Private::utf32_to_utf8(out_of_range, true);
  }
  catch(const Term::Exception& ex)
  {
    CHECK(ex.code() == 0);
    CHECK(std::string(ex.what()) == "Invalid UTF32 codepoint.");
  }
}

TEST_CASE("utf8 to utf32")
{
  // Some charabia UTF8
  str = (const char*)(u8"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n");
  s   = {Term::Private::utf8_to_utf32(str)};
  ref = U"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n";
  CHECK(s == ref);
}

TEST_CASE("is_valid_utf8")
{
  std::string bad{"\xc3\x28"};
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xa0\xa1";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xe2\x28\xa1";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xe2\x82\x28";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xf0\x28\x8c\xbc";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xf0\x90\x28\xbc";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  bad = "\xf0\x28\x8c\x28";
  CHECK(Term::Private::is_valid_utf8(bad.begin(), bad.end()) == false);
  std::string good{"0"};
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
  good = "a";
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
  good = "\xc3\xb1";
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
  good = "\xe2\x82\xa1";
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
  good = "\xf0\x90\x8c\xbc";
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
  good = {"n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑"};
  CHECK(Term::Private::is_valid_utf8(good.begin(), good.end()) == true);
}

TEST_CASE("utf8_codepoint_size")
{
  std::string one = "a";
  CHECK(Term::Private::utf8_codepoint_size(one[0]) == 1);
  std::string two = "\xc3\xb1";
  CHECK(Term::Private::utf8_codepoint_size(two[0]) == 2);
  std::string three = "\xe2\x82\xa1";
  CHECK(Term::Private::utf8_codepoint_size(three[0]) == 3);
  std::string four = "\xf0\x90\x8c\xbc";
  CHECK(Term::Private::utf8_codepoint_size(four[0]) == 4);
}
