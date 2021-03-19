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
#include <cassert>
#include <sstream>
#include <string>
#include "rv32i_decode.h"
#include "hex.h"
#include "memory.h"

/**
 * Decodes instructions 
 *
 * @param addr Address in memory
 * 
 * @param insn 32-bit instruction
 *
 * @return String containing the 32-bit address in hex, hex fullword represenation of the instruction, instruction mnemonic, and the register number/literal/decimal
 *
 ********************************************************************************/
std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
    
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);

    
    if (insn == insn_ecall)
    {
        return render_ecall(insn);
    }

    if (insn == insn_ebreak)
    {
        return render_ebreak(insn);
    }
    //switch statement for opcode
    switch(get_opcode(insn))
    {
        default:    
            return render_illegal_insn(insn);
        case opcode_lui :
            return render_lui(insn);
        case opcode_auipc :
            return render_auipc(insn);
        case opcode_jal :
            return render_jal(addr,insn);
        case opcode_jalr :
            return render_jalr(insn);
        case opcode_btype :
            switch(funct3)
            {
            default:            return render_illegal_insn(insn);
            case funct3_beq:    return render_btype(addr,insn,"beq");
            case funct3_bne:    return render_btype(addr,insn,"bne");
            case funct3_blt:    return render_btype(addr,insn,"blt");
            case funct3_bge:    return render_btype(addr,insn,"bge");
            case funct3_bltu:   return render_btype(addr,insn,"bltu");
            case funct3_bgeu:   return render_btype(addr,insn,"bgeu");
            }
            
        case opcode_load_imm :
            switch (funct3)
            {
            default:            return render_illegal_insn(insn);
            case funct3_lb:     return render_itype_load(insn,"lb");
            case funct3_lh:     return render_itype_load(insn,"lh");
            case funct3_lw:     return render_itype_load(insn,"lw");
            case funct3_lbu:    return render_itype_load(insn,"lbu");
            case funct3_lhu:    return render_itype_load(insn,"lhu");
            }
        case opcode_stype :
            switch(funct3)
            {
            default:            return render_illegal_insn(insn);
            case funct3_sb:     return render_stype(insn,"sb");
            case funct3_sh:     return render_stype(insn,"sh");
            case funct3_sw:     return render_stype(insn,"sw");
            }
        case opcode_alu_imm :
            switch(funct3)
            {
            default:            return render_illegal_insn(insn);
            case funct3_add:    return render_itype_alu(insn,"addi",get_imm_i(insn));
            case funct3_sll:    return render_itype_alu(insn,"slli",get_imm_i(insn));
            case funct3_slt:    return render_itype_alu(insn,"slti",get_imm_i(insn));
            case funct3_sltu:   return render_itype_alu(insn,"sltiu",get_imm_i(insn));
            case funct3_xor:    return render_itype_alu(insn,"xori",get_imm_i(insn));
            case funct3_srx:    
                switch(funct7)
                {
                default:            return render_illegal_insn(insn);
                case funct7_sra:    return render_itype_alu(insn,"srai",get_imm_i(insn)%XLEN);
                case funct7_srl:    return render_itype_alu(insn,"srli",get_imm_i(insn)%XLEN);
                }
            case funct3_or:    return render_itype_alu(insn,"ori",get_imm_i(insn));
            case funct3_and:   return render_itype_alu(insn,"andi",get_imm_i(insn));
            
            }
        case opcode_rtype :
            switch(funct3)
            {
            default:            return render_illegal_insn(insn);
            case funct3_add:
                switch(funct7)
                {
                default:            return render_illegal_insn(insn);
                case funct7_add:    return render_rtype(insn,"add");
                case funct7_sub:    return render_rtype(insn,"sub");
                }    
            case funct3_sll:    return render_rtype(insn,"sll");
            case funct3_slt:    return render_rtype(insn,"slt");
            case funct3_sltu:   return render_rtype(insn,"sltu");
            case funct3_xor:    return render_rtype(insn,"xor");
            case funct3_srx:    
                switch(funct7)
                {
                default:            return render_illegal_insn(insn);
                case funct7_sra:    return render_rtype(insn,"sra");
                case funct7_srl:    return render_rtype(insn,"srl");
                }
            case funct3_or:         return render_rtype(insn,"or");
            case funct3_and:        return render_rtype(insn,"and");
            }
        case opcode_system :
            switch(funct3)
            {
                default:            return render_illegal_insn(insn);
                case funct3_csrrw:  return render_csrrx(insn,"csrrw");
                case funct3_csrrs:  return render_csrrx(insn,"csrrs");
                case funct3_csrrc:  return render_csrrx(insn,"csrrc");
                case funct3_csrrwi: return render_csrrxi(insn,"csrrwi");
                case funct3_csrrsi: return render_csrrxi(insn,"csrrsi");
                case funct3_csrrci: return render_csrrxi(insn,"csrrci");
            }
        
    }

    assert(0 && "unrecognized opcode");

}
/**
 * Renders an error if instruction is illegal
 *
 * @param insn 32-bit instruction
 *
 * @return String containing error message
 *
 ********************************************************************************/
std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}
/**
 * Gets the opcode of the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing opcode
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return (insn & 0x0000007f);
}
/**
 * Gets the rd of the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing rd
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    uint32_t rd = (insn & 0x00000f80) >> 7;
    return rd;
}
/**
 * Gets rs1 of the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing rs1
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    return (insn & 0x000f8000) >> 15;
}
/**
 * Gets rs2 of the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing rs2
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    return (insn & 0x01f00000) >> 20;
}
/**
 * Gets the funct3 value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing funct3
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    return (insn & 0x00007000) >> 12;
}
/**
 * Gets the funct7 value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing funct7
 *
 ********************************************************************************/
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    return (insn & 0xfe000000) >> 25;
}
/**
 * Gets the imm_u value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing imm_u
 *
 ********************************************************************************/
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    int32_t imm_u = (insn & 0xfffff000);//I CHANGED THIS TO REMOVE AN F DO NOT FORGET IF ERRORS PERSIST
    return imm_u;
}
/**
 * Gets the imm_j value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing imm_j
 *
 ********************************************************************************/
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    int32_t imm_j = (insn & 0x7fe00000) >> (21 - 1);//b-k
    imm_j |= (insn & 0x00100000) >> (20 - 11);//for l
    imm_j |= (insn & 0x80000000) >> (31-20);//for a
    imm_j |= (insn & 0x000ff000);//m-t

    if (insn & 0x80000000) //extend sign
    {
        imm_j |= 0xfff00000;//fill rest of number
    }

    return imm_j;
}
/**
 * Gets the imm_i value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing imm_i
 *
 ********************************************************************************/
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    int32_t imm_i = (insn & 0xfff00000) >> (20 - 0);//a-l
    
    if (insn & 0x80000000)//extend sign
    {
        imm_i |= 0xfffff000;//fill number
    }

    return imm_i;
}
/**
 * Gets the imm_b value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing imm_b
 *
 ********************************************************************************/
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    int32_t imm_b = (insn & 0x7e000000) >> (25-5);//move b-g to 5-10
    imm_b |= (insn & 0x80000000) >> (31 - 12);//move a to spot 12
    imm_b |= (insn & 0x00000f00) >> (7-0);//move u-x to spot 4-1
    imm_b |= (insn & 0x00000080) << (11 - 7);//move y to spot 11

    if (insn & 0x80000000) 
        imm_b |= 0xfffff000; //sign-extend the left

    return imm_b;
}
/**
 * Gets the imm_s value in the instruction
 *
 * @param insn 32-bit instruction
 *
 * @return String containing imm_s
 *
 ********************************************************************************/
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    int32_t imm_s = (insn & 0xfe000000) >> (25-5);//move a-g to 5-11
    imm_s |= (insn & 0x00000f80) >> (7-0);//move u-y to 0-4

    if (insn & 0x80000000) 
        imm_s |= 0xfffff000; //sign-extend the left

    return imm_s;
}
/**
 * Formats the register into a printable format
 *
 * @param r register to be formatted
 *
 * @return String containing formatted register
 *
 ********************************************************************************/
std::string rv32i_decode::render_reg(int r)
{
    std::string s = std::to_string(r);
    return std::string("x") + s;
}
/**
 * Formats the base displacement
 *
 * @param base Number to go into the base
 * 
 * @param disp Number to go to displacement
 *
 * @return String containing formatted displacement
 *
 ********************************************************************************/
std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    std::ostringstream os;
    os << disp << "(" << render_reg(base) << ")";
    return os.str();
}
/**
 * Renders instruction mnemonic
 *
 * @param mnemonic Mnemonic of the instruction
 *
 * @return String containing formatted mnemonic
 *
 ********************************************************************************/
std::string rv32i_decode::render_mnemonic(const std::string &mnemonic)
{
    std::ostringstream os;
    os << std::left << std::setw(mnemonic_width) << mnemonic;
    return os.str();
}
/**
 * Renders the lui instruction
 *
 * @param insn Instuction to be rendered
 *
 * @return String containing formatted lui instruction
 ********************************************************************************/
std::string rv32i_decode::render_lui(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << render_mnemonic("lui") << render_reg(rd) << "," << hex::to_hex0x20((imm_u >> 12) & 0x0fffff);
    return os.str();
}
/**
 * Renders the aui_pc instruction
 *
 * @param insn Instuction to be rendered
 *
 * @return String containing formatted aui_pc instruction
 ********************************************************************************/
std::string rv32i_decode::render_auipc(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << render_mnemonic("auipc") << render_reg(rd) << "," << hex::to_hex0x20(((imm_u >> 12) & 0x0fffff) + ((opcode_auipc >> 12) & 0x0fffff));
    return os.str();
}
/**
 * Renders the jal instruction
 *
 * @param addr Address instruction is located in
 * 
 * @param insn Instuction to be rendered
 *
 * @return String containing formatted jal instruction
 ********************************************************************************/
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);

    std::ostringstream os;
    os << render_mnemonic("jal") << render_reg(rd) << "," << hex::to_hex0x32(addr + imm_j);
    return os.str();
}

/**
 * Renders the jalr instruction
 *
 * @param insn Instuction to be rendered
 *
 * @return String containing formatted jalr instruction
 ********************************************************************************/
std::string rv32i_decode::render_jalr(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic("jalr") << render_reg(rd) << "," << render_base_disp(rs1,imm_i);
    return os.str();
}
/**
 * Renders btype instructions
 *
 * @param addr Address instruction is located in
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted btype instructions
 ********************************************************************************/
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    int32_t imm_b = get_imm_b(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rs1) << "," << render_reg(rs2) << "," << hex::to_hex0x32(imm_b + (addr));
  
    return os.str();

}
/**
 * Renders itype instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted itype instructions 
 ********************************************************************************/
std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_base_disp(rs1,imm_i);
  
    return os.str();
}
/**
 * Renders stype instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted stype instructions 
 ********************************************************************************/
std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    int32_t imm_s = get_imm_s(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rs2) << "," << render_base_disp(rs1,imm_s);
  
    return os.str();
}
/**
 * Renders itype_alu instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 * 
 * @param imm_i imm_i instruction that goes with the alu
 *
 * @return String containing formatted itype_alu instructions 
 ********************************************************************************/
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << "," << imm_i;
  
    return os.str();
}
/**
 * Renders rtype instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted rtype instructions 
 ********************************************************************************/
std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << "," << render_reg(rs2);
  
    return os.str();
}
/**
 * Renders ecall
 * 
 * @param insn Instuction to be rendered
 * 
 * @return String containing ecall
 ********************************************************************************/
std::string rv32i_decode::render_ecall(uint32_t insn)
{
    return std::string("ecall");
}
/**
 * Renders ebreak
 * 
 * @param insn Instuction to be rendered
 * 
 * @return String containing ebreak
 ********************************************************************************/
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
    return std::string("ebreak");
}
/**
 * Renders csrrx type instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted csrrx instructions 
 ********************************************************************************/
std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t csr = get_imm_i(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << hex::to_hex0x12(csr) << "," << render_reg(rs1);
  
    return os.str();
}
/**
 * Renders csrrxi type instructions
 * 
 * @param insn Instuction to be rendered
 * 
 * @param mnemonic Mnemonic of instruction
 *
 * @return String containing formatted csrrxi instructions 
 ********************************************************************************/
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t csr = get_imm_i(insn);

    std::ostringstream os;

    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << hex::to_hex0x12(csr) << "," << rs1;
  
    return os.str();
}