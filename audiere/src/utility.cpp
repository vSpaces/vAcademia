#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "stdafx.h"
#include <ctype.h>
#include "utility.h"
#include "internal.h"


namespace audiere {

  ParameterList::ParameterList(const char* parameters):MP_MAP_INIT(m_values) {
    std::string key;
    std::string value;

    std::string* current_string = &key;

    // walk the string and generate the parameter list
    const char* p = parameters;
    while (*p) {

      if (*p == '=') {

        current_string = &value;

      } else if (*p == ',') {

        if (key.length() && value.length()) {
          //m_values[ MAKE_MP_STRING(key)] = MAKE_MP_STRING( value);
			m_values.insert( std::map<MP_STRING, MP_STRING>::value_type( MAKE_MP_STRING(key), MAKE_MP_STRING(value)));
        }
        key   = "";
        value = "";
        current_string = &key;

      } else {
        *current_string += *p;
      }

      ++p;
    }

    // is there one more parameter without a trailing comma?
    if (key.length() && value.length()) {
      //m_values[ MAKE_MP_STRING(key)] = MAKE_MP_STRING( value);
		m_values.insert( std::map<MP_STRING, MP_STRING>::value_type( MAKE_MP_STRING(key), MAKE_MP_STRING(value)));
    }
  }

  std::string
  ParameterList::getValue(
    const std::string& key,
    const std::string& defaultValue) const
  {
    m_valuesIter i = m_values.find(MAKE_MP_STRING(key));
    return (i == m_values.end() ? defaultValue : i->second);
  }

  bool
  ParameterList::getBoolean(const std::string& key, bool def) const {
    std::string value = getValue(key, (def ? "true" : "false"));
    return (value == "true" || atoi(value.c_str()) != 0);
  }

  int
  ParameterList::getInt(const std::string& key, int def) const {
    char str[20];
    sprintf(str, "%d", def);
    return atoi(getValue(key, str).c_str());
  }


  int strcmp_case(const char* a, const char* b) {
    while (*a && *b) {

      char c = tolower(*a++);
      char d = tolower(*b++);

      if (c != d) {
        return c - d;
      }
    }
  
    char c = tolower(*a);
    char d = tolower(*b);
    return (c - d);
  }

  int wcscmp_case(const wchar_t* a, const wchar_t* b) {
	  while (*a && *b) {

		  wchar_t c = towlower(*a++);
		  wchar_t d = towlower(*b++);

		  if (c != d) {
			  return c - d;
		  }
	  }

	  wchar_t c = towlower(*a);
	  wchar_t d = towlower(*b);
	  return (c - d);
  }


  ADR_EXPORT(int) AdrGetSampleSize(SampleFormat format) {
    switch (format) {
      case SF_U8:  return 1;
      case SF_S16: return 2;
      default:     return 0;
    }
  }

}
