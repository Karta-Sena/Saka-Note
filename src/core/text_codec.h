#pragma once

#include <vector>
#include <string>
#include <utility>

#include "types.h"

std::pair<Encoding, LineEnding> DetectEncoding(const std::vector<BYTE> &data);
std::wstring DecodeText(const std::vector<BYTE> &data, Encoding enc);
std::vector<BYTE> EncodeText(const std::wstring &text, Encoding enc, LineEnding le);
