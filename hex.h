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
#ifndef hex_h
#define hex_h
#include <string>
class hex
{
    public:
        static std::string to_hex8(uint8_t i);
        static std::string to_hex32(uint32_t i);
        static std::string to_hex0x32(uint32_t i);
        static std::string to_hex0x20(uint32_t i);
        static std::string to_hex0x12(uint32_t i);
};
#endif