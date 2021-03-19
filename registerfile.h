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
#ifndef registerfile_h
#define registerfile_h
#include <string>
#include <vector>
#include "hex.h"
class registerfile : public hex
{
public:
    registerfile();
    void reset();
    void set(uint32_t, int32_t);
    int32_t get(uint32_t) const;
    void dump(const std::string &hdr) const;
private:
    std::vector<uint32_t> reg;
};
#endif