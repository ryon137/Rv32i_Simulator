//******************************************************************
// Ryon Cook
// z1863954
// CSCI 463 Section 1 
// Assignment 5
//
// I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment
//
//******************************************************************
#include <iostream>
#include <iomanip>
#include <sstream>
#include "hex.h"

/**
 * Convert string to 8-bit hex.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return String with exactly 2 hex digits representing the 8 bits of the i argument.
 *
 ********************************************************************************/
std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;//declare string stream
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);//format number
    return os.str();//return string
}

/**
 * Convert string to 32-bit hex.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return String with exactly 8 hex digits representing the 32 bits of the i argument.
 *
 ********************************************************************************/
std::string hex::to_hex32(uint32_t i)
{
    std::ostringstream os;//declare string stream
    os << std::hex << std::setfill('0') << std::setw(8) << i;//format number 
    return os.str();//return string
}

/**
 * Convert string to 0x32-bit hex.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return String beginning with 0x, followed by the 8 hex digits representing the 32 bits of the i argument.
 *
 ********************************************************************************/
std::string hex::to_hex0x32(uint32_t i)
{
    return std::string("0x")+to_hex32(i);//concatenate 0x to the output of previous string
}

/**
 * Convert string to 0x20-bit hex to be used with lui and auipc instructions.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return String beginning with 0x, followed by the 5 hex digits representing the 20 bits of the i argument.
 *
 ********************************************************************************/
std::string hex::to_hex0x20(uint32_t i)
{
    i = (i & 0x000fffff);
    std::ostringstream os;//declare out stringstream
    os << std::hex << std::setfill('0') << std::setw(5) << i;//last 5 hext digits of i argument
    return std::string("0x") + os.str();//concatenate 0x to string and return
}

/**
 * Convert string to 0x20-bit hex to be used with csrrX() instructions.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return String beginning with 0x, followed by the 3 hex digits representing the 12 bits of the i argument.
 *
 ********************************************************************************/
std::string hex::to_hex0x12(uint32_t i)
{
    i = (i & 0x00000fff);
    std::ostringstream os;//declare out stringstream
    os << std::hex << std::setfill('0') << std::setw(3) << i;//last 3 hext digits of i argument
    return std::string("0x") + os.str();//concatenate 0x to string and return
}
