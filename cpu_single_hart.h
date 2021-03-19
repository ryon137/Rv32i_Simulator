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
#ifndef cpu_single_hart_h
#define cpu_single_hart_h
#include <string>
#include "rv32i_hart.h"
class cpu_single_hart : public rv32i_hart
{
public:
    cpu_single_hart(memory &mem) : rv32i_hart(mem){}
    void run(uint64_t exec_limit);
};
#endif