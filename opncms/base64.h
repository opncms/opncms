#pragma once
#ifndef OPNCMS_BASE64_H
#define OPNCMS_BASE64_H

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(const std::string& s);

#endif