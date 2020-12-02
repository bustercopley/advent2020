#include "precompiled.h"
#include "re.h"

std::string format_error(int error_number) {
  std::string result(256, '\0');
  result.resize(pcre2_get_error_message(
    error_number, (PCRE2_UCHAR8 *)std::data(result), std::size(result)));
  return result;
}

void throw_error(int error_code, const re::source_location &location) {
  std::string throw_error = format_error(error_code);
  std::cerr << location.file_name() << ":" << location.line() << ": "
            << format_error(error_code) << "\n";
  throw std::runtime_error("Regex throw_error");
}

re::code re::regex(const char *pattern, const re::source_location &location) {
  int errorcode;
  PCRE2_SIZE erroroffset;
  if (code p{pcre2_compile_8((PCRE2_SPTR8)pattern, PCRE2_ZERO_TERMINATED, 0,
        &errorcode, &erroroffset, nullptr)};
      p) {
    return p;
  }
  std::cerr << location.file_name() << ":" << location.line() << ": "
            << format_error(errorcode) << "\n"
            << pattern << "\n"
            << std::string(erroroffset, ' ') << "^" << std::endl;
  throw std::runtime_error("Regex compile throw_error");
}

re::match_data re::match(const re::code &re, const std::string &subject,
  const re::source_location &location) {

  match_data p{pcre2_match_data_create_from_pattern(&*re, nullptr)};
  int rc = pcre2_match(&*re, (PCRE2_SPTR8)std::data(subject),
    PCRE2_ZERO_TERMINATED, 0, 0, &*p, nullptr);
  if (rc < 0) {
    if (rc != PCRE2_ERROR_NOMATCH) {
      throw_error(rc, location);
    }
    p.reset();
  }
  return p;
}

std::pair<bool, std::size_t> re::get_matched(
  const re::match_data &data, int n, const re::source_location &location) {
  std::size_t length;
  int rc = pcre2_substring_length_bynumber(&*data, n, &length);
  if (rc < 0) {
    if (rc == PCRE2_ERROR_UNSET) {
      return {false, 0};
    }
    throw_error(rc, location);
  }
  return {true, length};
}

bool re::matched(
  const re::match_data &data, int n, const re::source_location &location) {

  return re::get_matched(data, n, location).first;
}

std::size_t re::matched_length(
  const re::match_data &data, int n, const re::source_location &location) {

  return re::get_matched(data, n, location).second;
}

std::string re::match_string(
  const re::match_data &data, int n, const re::source_location &location) {

  PCRE2_SIZE size;
  if (int rc = pcre2_substring_length_bynumber(&*data, n, &size); rc < 0) {
    throw_error(rc, location);
  }
  std::string result(size, '\0');
  ++size; // writable buffer size including null terminator
  if (int rc = pcre2_substring_copy_bynumber(
        &*data, n, (PCRE2_UCHAR8 *)std::data(result), &size);
      rc < 0) {
    throw_error(rc, location);
  }
  result.resize(size);
  return result;
}
