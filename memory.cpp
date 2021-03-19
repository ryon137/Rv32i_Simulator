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
#include "memory.h"
#include "hex.h"

/**
 * Constructor.
 * Allocate siz byes in the mem vector and initalize every bye/element to 0xa5.
 *
 * @param siz Size to allocate to the vector.
 *
 ********************************************************************************/
memory::memory(uint32_t siz)
{
    siz = (siz+15)&0xfffffff0;//round the length up, mod-16

    mem.reserve(siz);//set vector size to siz

    for(unsigned i = 0; i < siz; ++i)//loop through mem vector
    {
        mem.push_back(0xa5);//insert 0xa5 into end of vector
    }
}

/**
 * Destructor.
 ********************************************************************************/
memory::~memory()
{
    mem.clear();
}

/**
 * Return true if given address is not in simulated memory.  If not in memory, print warning message.
 *
 * @param i Index of memory vector to be referenced.
 *
 * @return True if i is in simulated memory, false if not.
 *
 ********************************************************************************/
bool memory::check_illegal(uint32_t i) const
{
    try//address of data in memory
    {
        mem.at(i);
    }
    catch(...)//print error
    {
        cout << "WARNING: Address out of range: " << hex::to_hex0x32(i) << endl;
        return true;
    }

    return false;

}

/**
 * Return the number of bytes in simulated memory.
 *
 * @return uint32_t containing size of 
 ********************************************************************************/
uint32_t memory::get_size() const
{
    return mem.size();
}

/**
 * Check to see if given addr is in mem by calling check_illigal().  If addr is in valid range then return value of byte of simulated memory at given address.  If not, return 0.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return Return value of byte in the simulated memory, or 0.
 *
 ********************************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
    if(check_illegal(addr) == false)
    {
        return mem[addr];
    }
    else
    {
        return 0;
    }
}

/**
 * Call get8 function twice to get two bytes and then combine them in little-endian order to create a 16-bit return value.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return 16-bit number from memory.
 *
 ********************************************************************************/
uint16_t memory::get16(uint32_t addr) const
{
    uint8_t byte1 = get8(addr);//fetch byte 1
    uint8_t byte2 = get8(addr+1);//fetch byte 2

    uint16_t combined = (byte2 << 8) | (byte1 &0xff);//combine numbers together
    return combined;
}

/**
 * Call get16() twice and combine results in little endian order.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return 32-bit number from memory.
 *
 ********************************************************************************/
uint32_t memory::get32(uint32_t addr) const
{
    uint16_t byte1 = get16(addr);//fetch byte 1
    uint16_t byte2 = get16(addr+2);//fetch byte 2

    uint32_t combined = (byte2 << 16) | (byte1 &0xffff);

    return combined;
}

/**
 * Call get8 and return sign-extended value of the byte as a 32-bit signed integer.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return 32-bit sign-extended number from memory.
 *
 ********************************************************************************/
int32_t memory::get8_sx(uint32_t addr) const
{
    uint8_t temp = get8(addr);//create temp to only call get8 once
    uint8_t num = temp;
    uint32_t num2 = (num & 0x000000ff); //copy numbers into 32-bit value

    num2 >>= 7;

    if(num2 == 0x00000001)//if MSB is 1, put 1's in front
    {
        num2 = (temp | 0xffffff00);
    }

    else
    {
        num2 = temp;
    }

    return num2;
}

/**
 * Call get16 and return sign-extended value of the byte as a 32-bit signed integer.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return 32-bit sign-extended number from memory.
 *
 ********************************************************************************/
int32_t memory::get16_sx(uint32_t addr) const
{
    uint16_t temp = get16(addr);//create temp to only call get16 once
    uint16_t num = temp;
    uint32_t num2 = (num& 0x0000ffff); //copy numbers into 32-bit value

    num2 >>= 15;

    if(num2 == 0x00000001)//if MSB is 1, put 1's in front
    {
        num2 = (temp | 0xffff0000);
    }

    else
    {
        num2 = temp;
    }

    return num2;
}

/**
 * Call get32 and return sign-extended value of the byte as a 32-bit signed integer.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @return 32-bit sign-extended number from memory.
 *
 ********************************************************************************/
int32_t memory::get32_sx(uint32_t addr) const
{
    /*uint32_t num = get32(addr);
    uint32_t num2 = (num& 0x00ffffff); //copy numbers into 32-bit value

    if(num2 & 0x00000000)//if MSB is 1, put 1's in front
    {
        num2 = (num2 | 0xff000000);
    }*/

    return get32(addr);
}

/**
 * Call check_illegal to verify if the addr is valid.  If valid, set byte in simulated memory at that address to the given val.  If not, discard data and return to caller
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @param val Value to change address to.
 *
 ********************************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
    if(check_illegal(addr) == false)//if valid
    {
        mem[addr] = val;//set value at addr to given val
    }
}

/**
 * Call set8 twice to store given val in little-endian order into simulated memory starting at address given in addr.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @param val Value to change address to.
 *
 ********************************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
    set8(addr,val & 0x000000ff);
    set8(addr+1,(val & 0x0000ff00) >> 8);
}

/**
 * Call set16 twice to store given val in little-endian order into simulated memory starting at address given in addr.
 *
 * @param addr Address of memory vector to be referenced.
 *
 * @param val Value to change address to.
 *
 ********************************************************************************/
void memory::set32(uint32_t addr, uint32_t val)
{
    //split val in half, then send them
    set16(addr,val & 0x0000ffff);
    set16(addr+2,(val & 0xffff0000) >> 16);
}

/**
 * Dump entire contents of sumulated memory in hex with corresponding ASCII characters on right
 *
 ********************************************************************************/
void memory::dump() const
{
    int header = 0;//keep track of header
    int newLine = 0;//keep track of when to newline
    uint8_t ch;

    std::cout << std::setfill('0');

    for(auto i = std::begin(mem); i != std::end(mem); i = i + 16)//use iterator to loop through vector
    {
        std::cout << std::setw(8) << hex::to_hex32(header) << ": ";//print out number
        header = header + 16;//add 16 to header

        auto a = i;//store hex characters
        auto b = i;//store ascii characters

        for(int k = 0; k != 16; ++k)//while not a newline
        {
            newLine++;//increment newLine
            std::cout << hex::to_hex8(*a) << " ";//pint out hex characters
            std::advance(a, 1);//advance vector by 1

            if(newLine == 8)//break up dump
            {
                std::cout << " ";
            }
        }

        newLine = 0;//zero out newline

        std::cout << "*";

        for(int j = 0; j != 16; ++j)//print ascii characters
        {
            ch = isprint(*b) ? *b : '.';
            std::cout << ch;
            std::advance(b,1);
        }
        
        std::cout << "*" << std::endl;
    }

    
}

/**
 * Open file in binary mode and read contents into simulated memory.
 *
 * @param fname Name of file to be read.
 *
 * @return True if file can be opened and read, false if not.
 *
 ********************************************************************************/
bool memory::load_file(const std::string &fname)
{
    ifstream infile(fname, std::ios::in|std::ios::binary);

    uint8_t i;//index variable
        
    if(!infile)
    {
        cout << "Can't open file '" << fname << "' for reading." << endl;
        return false;
    }

    //check to see if file is too big
    infile.seekg(0,std::ios::end);
    size_t fileSize = infile.tellg();
    if(fileSize > get_size())
    {
        check_illegal(get_size());
        std::cerr << "Program too big." << endl;
        infile.close();
        return false;
    }

    //reopen file, because seekg 
    infile.close();
    infile.open(fname);

    infile >> std::noskipws;
 
    //loop through file and add elements to vector
    for (uint32_t addr = 0; infile >> i; ++addr)
    {
        mem[addr] = i;//add element to memory vector
    }

    infile.close();//close file
    return true;

}