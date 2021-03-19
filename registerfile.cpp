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
#include <vector>
#include <fstream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include "hex.h"
#include "registerfile.h"

/**
 * Constructor
 *
 ********************************************************************************/
registerfile::registerfile()
{
    reg.reserve(32);
    reset();
}

/**
 * Resets CPU 
 *
 ********************************************************************************/
void registerfile::reset()
{
    reg[0] = 0x0;

    for (uint8_t i = 1; i <= 32; ++i)
    {
        reg[i] = 0xf0f0f0f0;
    }
}

/**
 * Assign values to CPU registers
 *
 * @param r Register to be altered
 * 
 * @param val Value to put in the register
 *
 ********************************************************************************/
void registerfile::set(uint32_t r, int32_t val)
{
    if (r == 0)
    {
        //do nothing if r = 0
    }

    else
    {
        reg[r] = val;
    }

}

/**
 * Retrieve a value in the given register
 *
 * @param r Register to fetch
 *
 * @return Value of register
 *
 ********************************************************************************/
int32_t registerfile::get(uint32_t r) const
{
    if (r == 0)
    {
        return 0;
    }

    else
    {
        return reg[r];
    }
}

/**
 * Dump registers
 *
 * @param hdr header to be printed for any and every register
 *
 ********************************************************************************/
void registerfile::dump(const std::string &hdr) const
{
    uint8_t newline = 0;
    int regNum = 0;
    int j = 0;
    int k = 0;

    for (uint8_t i = 0; i < 32; i = i + 8)
    {
        std::cout << hdr  << std::right << std::setw(3)<< "x" + std::to_string(regNum) << " ";//print register number

        regNum += 8;

        while(j != 8)
        {
            std::cout << hex::to_hex32(reg[k]);

            
            newline++;

            if(newline != 8)
            {
                std:: cout << " ";
            }

            if(newline == 4)
            {
                std::cout << " ";
            }
            ++j;
            ++k;
        }

        newline = 0;

        j = 0;

        if(j != 33)
        {
            std::cout << std::endl;
        }
        
    }
}