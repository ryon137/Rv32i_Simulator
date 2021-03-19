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
#include "cpu_single_hart.h"
#include "rv32i_hart.h"
#include "memory.h"
#include "registerfile.h"

/**
 * Controls execution of the cpu simulator.  
 * 
 * @param exec_limit Number of maximum executions 
 *
 ********************************************************************************/
void cpu_single_hart::run(uint64_t exec_limit)
{
    //set register x2 to the memory size
    regs.set(2,mem.get_size());

    if(exec_limit == 0)
    {
        while(is_halted() == false)
        {
            tick();//call tick while is_halted is false
        }
    }

    if(exec_limit != 0)
    {
        while(is_halted() == false && get_insn_counter() != exec_limit)
        {
            tick();
        }
    }

    if(is_halted() == true)
    {
        std::cout <<"Execution terminated. Reason: " << get_halt_reason() << std::endl;
    }
    std::cout << get_insn_counter() << " instructions executed" << std::endl;
}