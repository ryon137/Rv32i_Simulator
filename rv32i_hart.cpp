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
#include <string>
#include "rv32i_hart.h"
#include "memory.h"
#include "hex.h"

/**
 * Reset the rv32i object and the registerfile
 *
 ********************************************************************************/
void rv32i_hart::reset()
{
    pc = 0;//set pc register to zero
    regs.reset();
    insn_counter = 0;
    halt = false;
    halt_reason = { "none" };
}

/**
 * Dump entire state of the hart
 * 
 * @param hdr Header to be inserted into the register dump
 *
 ********************************************************************************/
void rv32i_hart::dump(const std::string &hdr) const
{
    regs.dump(hdr);
    std::cout << " pc " << hex::to_hex32(pc) << std::endl;
}

/**
 * Executes instructions 
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);

    if(insn == insn_ebreak)
    {
        exec_ebreak(insn,pos);
        return;
    }

    switch(get_opcode(insn))
    {
        default:                exec_illegal_insn(insn,pos); return;
        case opcode_lui:        exec_lui(insn,pos); return;
        case opcode_auipc:      exec_auipc(insn,pos); return;
        case opcode_jal:        exec_jal(insn,pos); return;
        case opcode_jalr:       exec_jalr(insn,pos); return;
        case opcode_btype:
            switch(funct3)
            {
            default:            exec_illegal_insn(insn,pos); return;
            case funct3_bne:    exec_bne(insn,pos); return;
            case funct3_blt:    exec_blt(insn,pos); return;
            case funct3_bge:    exec_bge(insn,pos); return;
            case funct3_bltu:   exec_bltu(insn,pos); return;
            case funct3_bgeu:   exec_bgeu(insn,pos); return;
            case funct3_beq:    exec_beq(insn,pos); return;
            }
        case opcode_alu_imm :
            switch(funct3)
            {
            default:            exec_illegal_insn(insn,pos); return;
            case funct3_add:    exec_addi(insn,pos); return;
            case funct3_sll:    exec_slli(insn,pos); return;
            case funct3_slt:    exec_slti(insn,pos); return;
            case funct3_sltu:   exec_sltiu(insn,pos); return;
            case funct3_xor:    exec_xori(insn,pos); return;
            case funct3_srx:
                switch(funct7)
                {
                default:            exec_illegal_insn(insn,pos); return;
                case funct7_srl:    exec_srli(insn,pos); return;
                case funct7_sra:    exec_srai(insn,pos); return;
                }
            case funct3_or:     exec_ori(insn,pos); return;
            case funct3_and:    exec_andi(insn,pos); return;
            }
        case opcode_load_imm:
            switch(funct3)
            {
            default:            exec_illegal_insn(insn,pos); return;
            case funct3_lb:     exec_lb(insn,pos); return;
            case funct3_lh:     exec_lh(insn,pos); return;
            case funct3_lw:     exec_lw(insn,pos); return;
            case funct3_lbu:    exec_lbu(insn,pos); return;
            case funct3_lhu:    exec_lhu(insn,pos); return;
            }
        case opcode_stype:
            switch(funct3)
            {
            default:            exec_illegal_insn(insn,pos); return;
            case funct3_sb:     exec_sb(insn,pos); return;
            case funct3_sh:     exec_sh(insn,pos); return;
            case funct3_sw:     exec_sw(insn,pos); return;
            }
        case opcode_rtype:
            switch(funct3)
            {
            default:            exec_illegal_insn(insn,pos); return;
            case funct3_add:
                switch(funct7)
                {
                default:            exec_illegal_insn(insn,pos); return;
                case funct7_add:    exec_add(insn,pos); return;
                case funct7_sub:    exec_sub(insn,pos); return;
                }
            case funct3_sll:        exec_sll(insn,pos); return;
            case funct3_slt:        exec_slt(insn,pos); return;
            case funct3_sltu:       exec_sltu(insn,pos); return;
            case funct3_xor:        exec_xor(insn,pos); return;
            case funct3_srx:
                switch(funct7)
                {
                default:            exec_illegal_insn(insn,pos); return;
                case funct7_srl:    exec_srl(insn,pos); return;
                case funct7_sra:    exec_sra(insn,pos); return;
                }
            case funct3_or:         exec_or(insn,pos); return;
            case funct3_and:        exec_and(insn,pos); return;
            }
        case opcode_system:
            switch(funct3)
            {
                default:            exec_illegal_insn(insn,pos); return;
                case funct3_csrrs:  exec_csrrs(insn,pos); return;
            }
    }
    
    return;
}

/**
 * Executes lui instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    int32_t imm_u = rv32i_decode::get_imm_u(insn);
    regs.set(rd,imm_u);
    if(pos)
    {
        std::string s = render_lui(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(imm_u) << "\n";
    }
    pc += 4;
}

/**
 * Executes auipc instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    int32_t imm_u = rv32i_decode::get_imm_u(insn);
    if(pos)
    {
        std::string s = render_auipc(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_u) << " = " << hex::to_hex0x32(imm_u + pc) << "\n";
    }
    regs.set(rd,(imm_u + pc));
    pc += 4;
}

/**
 * Executes jal instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    int32_t imm_j = rv32i_decode::get_imm_j(insn);
    if(pos)
    {
        std::string s = render_jal(pc,insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32((pc) + 4) << ",  pc = " << hex::to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_j) << " = " << hex::to_hex0x32((imm_j + pc)) << "\n";
    }
    regs.set(rd,(pc + 4));
    pc += imm_j;
}

/**
 * Executes jalr instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t result = (regs.get(rs1) + imm_i) & 0x0fffffffe;
    if(pos)
    {
        std::string s = render_jalr(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32((pc) + 4) << ",  pc = (" << hex::to_hex0x32(imm_i) << " + " << hex::to_hex0x32(regs.get(rs1)) << ") & 0xfffffffe = " << hex::to_hex0x32(result) << "\n";
    }
    regs.set(rd,(pc + 4));
    pc = result;
}

/**
 * Executes bne instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t result = pc + ((regs.get(rs1) != regs.get(rs2)) ? imm_b : 4);
    
    if(pos)
    {
        std::string s = render_btype(pc,insn,"bne");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " != " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((regs.get(rs1) != regs.get(rs2)) ? imm_b : 4);
    
}

/**
 * Executes blt instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t result = pc + ((regs.get(rs1) < regs.get(rs2)) ? imm_b : 4);
    if(pos)
    {
        std::string s = render_btype(pc,insn,"blt");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " < " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((regs.get(rs1) < regs.get(rs2)) ? imm_b : 4);
}

/**
 * Executes bge instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t result = pc + ((regs.get(rs1) >= regs.get(rs2)) ? imm_b : 4);
    if(pos)
    {
        std::string s = render_btype(pc,insn,"bge");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " >= " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((regs.get(rs1) >= regs.get(rs2)) ? imm_b : 4);
}

/**
 * Executes bltu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t temp2 = regs.get(rs2);
    uint32_t result = pc + ((temp1 < temp2) ? imm_b : 4);
    if(pos)
    {
        std::string s = render_btype(pc,insn,"bltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(temp1) << " <U " << hex::to_hex0x32(temp2) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((temp1 < temp2) ? imm_b : 4);
}

/**
 * Executes bgeu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t temp2 = regs.get(rs2);
    uint32_t result = pc + ((temp1 >= temp2) ? imm_b : 4);
    if(pos)
    {
        std::string s = render_btype(pc,insn,"bgeu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(temp1) << " >=U " << hex::to_hex0x32(temp2) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((temp1 >= temp2) ? imm_b : 4);
}

/**
 * Executes beq instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_b = rv32i_decode::get_imm_b(insn);
    uint32_t result = pc + ((regs.get(rs1) == regs.get(rs2)) ? imm_b : 4);
    if(pos)
    {
        std::string s = render_btype(pc,insn,"beq");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " == " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(result) << "\n";
    }
    pc = pc + ((regs.get(rs1) == regs.get(rs2)) ? imm_b : 4);
}

/**
 * Executes addi instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    if(pos)
    {
        std::string s = render_itype_alu(insn,"addi",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(regs.get(rs1) + imm_i) <<  "\n";
    }
    regs.set(rd,(regs.get(rs1) + imm_i));
    pc = pc + 4;
}

/**
 * Executes sll instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    
    rs2 = (regs.get(rs2) & 0x0000001f);
    uint32_t result = (regs.get(rs1) & 0xffffffff) << rs2 ;
    
    if(pos)
    {
        std::string s = render_rtype(insn,"sll");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " << " << rs2 << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes slli instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t shamt_i = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) & 0xffffffff) << shamt_i;
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"slli",shamt_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " << " << shamt_i << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes srl instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    rs2 = (regs.get(rs2) & 0x0000001f);

    uint32_t result = (regs.get(rs1) & 0xffffffff) >> rs2;
    
    if(pos)
    {
        std::string s = render_rtype(insn,"srl");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << rs2 << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes sra instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    rs2 = (regs.get(rs2) & 0x0000001f);

    uint32_t result = regs.get(rs1) >> rs2;
    
    if(pos)
    {
        std::string s = render_rtype(insn,"sra");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << rs2 << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes srli instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t shamt_i = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) & 0xffffffff) >> shamt_i;
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"srli",shamt_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << shamt_i << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes srai instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t shamt_i = rv32i_decode::get_rs2(insn);
    uint32_t result = regs.get(rs1) >> shamt_i;
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"srai",shamt_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << shamt_i << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes slt instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    int32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t result = (regs.get(rs1) <  regs.get(rs2)) ? 1 : 0;
    
    if(pos)
    {
        std::string s = render_rtype(insn,"slt");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = (" << hex::to_hex0x32(regs.get(rs1)) << " < " << hex::to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes sltu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) <  regs.get(rs2)) ? 1 : 0;
    
    if(pos)
    {
        std::string s = render_rtype(insn,"sltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = (" << hex::to_hex0x32(regs.get(rs1)) << " <U " << hex::to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes xor instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) ^  regs.get(rs2));
    
    if(pos)
    {
        std::string s = render_rtype(insn,"xor");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " ^ " << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes or instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) |  regs.get(rs2));
    
    if(pos)
    {
        std::string s = render_rtype(insn,"or");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " | " << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes slti instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t result = (regs.get(rs1) <  imm_i) ? 1 : 0;
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"slti",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = (" << hex::to_hex0x32(regs.get(rs1)) << " < " << imm_i << ") ? 1 : 0 = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes sltiu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t result = (temp1 < imm_i) ? 1 : 0;
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"sltiu",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = (" << hex::to_hex0x32(temp1) << " <U " << imm_i << ") ? 1 : 0 = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes xori instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t result = (temp1 ^ imm_i);
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"xori",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(temp1) << " ^ " << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes ori instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t result = (temp1 | imm_i);
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"ori",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(temp1) << " | " << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes andi instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t imm_i = rv32i_decode::get_imm_i(insn);
    uint32_t temp1 = regs.get(rs1);
    uint32_t result = (temp1 & imm_i);
    
    if(pos)
    {
        std::string s = render_itype_alu(insn,"andi",imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(temp1) << " & " << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes and instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    uint32_t result = (regs.get(rs1) & regs.get(rs2));
    
    if(pos)
    {
        std::string s = render_rtype(insn,"and");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " & " << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes lbu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    int32_t zero = 0x0;
    uint32_t result = (mem.get8(imm_i + regs.get(rs1)));
    if ((result & 0x00000080) == 0x00000080)
    {
        result |= zero;
    }
    if(pos)
    {
        std::string s = render_itype_load(insn,"lbu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes lhu instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    int32_t zero = 0x0;
    uint32_t result = (mem.get16(imm_i + regs.get(rs1)));
    if ((result & 0x00008000) == 0x00008000)
    {
        result |= zero;
    }
    if(pos)
    {
        std::string s = render_itype_load(insn,"lhu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = zx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes lb instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    int32_t f = 0xffffff00;
    uint32_t result = (mem.get8(imm_i + regs.get(rs1)));
    if ((result & 0x00000080) == 0x00000080)
    {
        result |= f;
    }
    if(pos)
    {
        std::string s = render_itype_load(insn,"lb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes lh instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    int32_t f = 0xffff0000;
    uint32_t result = (mem.get16(imm_i + regs.get(rs1)));
    if ((result & 0x00008000) == 0x00008000)
    {
        result |= f;
    }
    if(pos)
    {
        std::string s = render_itype_load(insn,"lh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes lw instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    int32_t imm_i = rv32i_decode::get_imm_i(insn);
    //int32_t f = 0xf0000000;
    uint32_t result = (mem.get32(imm_i + regs.get(rs1)));
    /*if ((result & 0x80000000) == 0x80000000)
    {
        result |= f;
    }*/
    if(pos)
    {
        std::string s = render_itype_load(insn,"lw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes sb instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_s = rv32i_decode::get_imm_s(insn);
    uint32_t result = (imm_s + regs.get(rs1));
    rs2 = (regs.get(rs2) & 0x000000ff);
    
    if(pos)
    {
        std::string s = render_stype(insn,"sb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// "  << "m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::to_hex0x32(rs2) <<  "\n";
    }
    pc = pc + 4;
    mem.set8(result,rs2);
}

/**
 * Executes sh instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_s = rv32i_decode::get_imm_s(insn);
    uint32_t result = (imm_s + regs.get(rs1));
    rs2 = (regs.get(rs2) & 0x0000ffff);
    
    if(pos)
    {
        std::string s = render_stype(insn,"sh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// "  << "m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::to_hex0x32(rs2) <<  "\n";
    }
    pc = pc + 4;
    mem.set16(result,rs2);
}

/**
 * Executes sw instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);
    int32_t imm_s = rv32i_decode::get_imm_s(insn);
    uint32_t result = (imm_s + regs.get(rs1));
    rs2 = (regs.get(rs2) & 0xffffffff);
    
    if(pos)
    {
        std::string s = render_stype(insn,"sw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// "  << "m32(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::to_hex0x32(rs2) <<  "\n";
    }
    pc = pc + 4;
    mem.set32(result,rs2);
}

/**
 * Executes add instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);;
    uint32_t result = (regs.get(rs2) + regs.get(rs1));
    
    if(pos)
    {
        std::string s = render_rtype(insn,"add");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes sub instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = rv32i_decode::get_rd(insn);
    uint32_t rs1 = rv32i_decode::get_rs1(insn);
    uint32_t rs2 = rv32i_decode::get_rs2(insn);;
    uint32_t result = (regs.get(rs1) - regs.get(rs2));
    
    if(pos)
    {
        std::string s = render_rtype(insn,"sub");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd)  << " = " << hex::to_hex0x32(regs.get(rs1)) << " - " << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(result) <<  "\n";
    }
    pc = pc + 4;
    regs.set(rd,result);
}

/**
 * Executes csrrs instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t csr = get_imm_i(insn) & 0x00000fff;;

    if (csr != 0xf14 || rs1 != 0)
    {
        halt = true;
        halt_reason = "Illegal CSR in CRRSS instruction";
    }

    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrs");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << render_reg(rd) << " = " << std::dec << mhartid << "\n";
    }
    if (!halt)
    {
        regs.set(rd, mhartid);
        pc += 4;
    }
}

/**
 * Executes illegal instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    (void)insn;

    if (pos)
    {
        *pos << render_illegal_insn(insn);
    }

    halt = true;
    halt_reason = "Illegal instruction";
}

/**
 * Executes ebreak instruction
 * 
 * @param insn 32-bit instruction
 * 
 * @param pos Output stream containing exec information
 *
 ********************************************************************************/
void rv32i_hart:: exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if(pos)
    {
        std::string s = render_ebreak(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT\n";
    }
    halt = true;
    halt_reason = "EBREAK instruction";
}

/**
 * Tells the simulator to execute an instruction
 * 
 * @param hdr Header to be printed on left of any and all output 
 *
 ********************************************************************************/
void rv32i_hart::tick(const std::string &hdr)
{
    if(halt == true)
    {
        return;
    }

    insn_counter++;

    if(show_registers == true)
    {
        dump();//dump the hart
    }

    uint32_t insn = mem.get32(pc);//fetch an instruction from the memory at the address in the pc register
    
    if(show_instructions == true)
    {
        std::cout << std::setw(8) << hex::to_hex32(pc) << ": " << std::setw(8) << hex::to_hex32(insn) << "  ";
        
        exec(insn, &std::cout);//execute instruction and render the instruction and simulation details
    }

    else
    {
        exec(insn, nullptr);//execute without rendering anyhting
    }
}
