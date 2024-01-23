#pragma once
#include "string";

typedef const unsigned int CU_INT32;
typedef unsigned int U_INT32;
typedef const short unsigned int CU_INT16;
typedef short unsigned int U_INT16;
typedef const unsigned char CU_CHAR;
typedef unsigned char U_CHAR;
typedef std::basic_string<unsigned char> U_STRING;

static CU_INT16 BIT32_BITS = 0xffffffff;
static CU_INT16 BIT32_SIZE = 32;

static CU_INT16 INT_SIZE_BYTES = 4;
static CU_INT16 BYTE_SIZE_BITS = 8;
static CU_INT16 CHARACTER_SIZE_BYTES = 1;
static CU_INT16 INT_SIZE_BITS = INT_SIZE_BYTES * BYTE_SIZE_BITS;


CU_INT32 ExtractBits(CU_INT32 number, CU_INT16 field, CU_INT32 width) {
  //offset to field
  //make the field bit the first
  //number = 101110 
  //field = 1 -> 01110
  //field = 0 -> 101110
  // >> lshift
  // << rshift
  CU_INT32 field_offsetted_number = number >> field;
  CU_INT32 bit_mask = (1 << width) - 1;
  return field_offsetted_number & bit_mask;
}

CU_INT32 ReplaceBits(CU_INT32 target_number, CU_INT32 replacement, CU_INT32 field, CU_INT32 width) {
  //there was a weird glitch
  //if i did BIT32_BITS >> BIT32_SIZE it gave me 0
  //but if i did BIT32_BITS >> BIT32_SIZE - field  field = 0 it gave me 4,294,967,295
  CU_INT32 start_bits = field == 0 ? 0 : ExtractBits(target_number, 0, field);
  CU_INT32 end_bits_full = BIT32_BITS << width + field;
  CU_INT32 end_bits = target_number & end_bits_full;
  CU_INT32 offseted_replacement = ExtractBits(replacement, 0, width) << field;

  return (offseted_replacement | start_bits) | end_bits;
}

CU_INT32 AddEndBits(CU_INT32 target_number, CU_INT32 replacement, CU_INT32 field) {
  CU_INT32 offseted_replacement = replacement << field;
  return target_number | offseted_replacement;
}

std::string Int32ToString(CU_INT32 number) {
  std::string return_string = "";
  for (int i = 0; i < INT_SIZE_BYTES; i++) {
    CU_INT32 character = ExtractBits(number, i * CHARACTER_SIZE_BYTES * BYTE_SIZE_BITS, CHARACTER_SIZE_BYTES * BYTE_SIZE_BITS);
    return_string += character;
  }
  return return_string;
}