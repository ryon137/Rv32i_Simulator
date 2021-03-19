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
#include <unistd.h>
#include <sstream>  
#include <iostream>
#include <iomanip>
#include <fstream>
#include "hex.h"
#include "memory.h"
#include "rv32i_decode.h"
#include "cpu_single_hart.h"
#include "rv32i_hart.h"
#include "registerfile.h"

/**
 * Prints an appropriate “Usage” error message and “Pattern” to stderr and terminates the program.
 *
 ********************************************************************************/
 static void usage()
 {
    cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit] [-m hex-mem-size] infile" << endl;
    cerr << "    -d show disassembly before program execution" << endl;
    cerr << "    -i show instruction printing during execution" << endl;
    cerr << "    -l maximum number of instructions to exec" << endl;
    cerr << "    -m specify memory size (default = 0x100)" << endl;
    cerr << "    -r show register printing during execution" << endl;
    cerr << "    -z show a dump of the regs & memory after simulation" << endl;
    exit(1);
 }
/**
 * Loops through memory vector, and prints the disassembled information
 * 
 * @param mem Simulated memory vector
 *  
 ********************************************************************************/
 static void disassemble(const memory &mem)
 {
    int addr = 0;

    for(uint32_t i = 0; i < mem.get_size(); i+=4)
    {
        uint32_t opcode = mem.get32(addr);

        std::cout << hex::to_hex32(addr) << ": " << hex::to_hex32(opcode) << "  " << rv32i_decode::decode(addr,opcode) <<endl;
        
        addr += 4;
    }
 }

/**
 * Calls the various methods of Memory.h, Hex.h, and rv32i_decode in order to test.
 *
 * @return Determines whether or not function executed successfully.
 *
 *
 ********************************************************************************/
int main (int argc,char ** argv)
{
    uint32_t memory_limit = 0x100 ; // default memory size is 0 x100
    uint32_t execution_limit = 0;

    int opt;
    
    bool dflag = false;
    bool zflag = false;
    bool iflag = false;
    bool rflag = false;

    while ((opt = getopt(argc, argv, "dil:m:rz")) != -1)
    {
        switch(opt)
        {
            case 'm':
            {
                std::istringstream iss(optarg);
                iss >> std::hex >> memory_limit;
                break;
            }
            case 'd':
            {
                //disassemble program
                dflag = true;
                break;
            }
            case 'i':
            {
                //show instruction printing during execution
                iflag = true;
                break;
            }
            case 'l':
            {
                //specifie maximum limit of instructions to execute
                //lflag = true;
                std::istringstream ist(optarg);
                ist >> std::hex >> execution_limit;
                break;
            }
            case 'r':
            {
                //show dump of hart status before each instruction is simulated
                rflag = true;
                break;
            }
            case 'z':
            {
                zflag = true;
                //show a dump of the hart status and memory after the simulation has halted
                break;
            }
            default: /* '?' */
                usage();
        }
    }


    if (optind >= argc)
    {
        usage(); // missing filename
    }

    memory mem(memory_limit);

    
    if (!mem.load_file(argv[optind]))
        usage();

    if(dflag == true)
    {
        disassemble(mem); 
    }

    //construct and reset cpu
    cpu_single_hart cpu(mem);
    cpu.reset();

    if(iflag == true)
    {
        cpu.set_show_instructions(true);
    }
    
    if(rflag == true)
    {
        cpu.set_show_registers(true);
    }


    cpu.run(execution_limit);

    if(zflag == true)
    {
        cpu.dump();   
        mem.dump();
    }

    

    return 0;
 }

