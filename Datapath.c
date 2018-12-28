// Lawrence O'Connor-Emanuel
// CDA 3103C

#include "spimcore.h"
#include <stdlib.h>
#include <stdio.h>




/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
/*
- Implements the functions of the ALU
- The instructions are passed into ALU control
- Output to ALU result
- Implements 8 seperate instructions
*/
int z = 0;
switch (ALUControl)
{

	case 0x0:
		z = A+B;
		break;

	case 0x1:
		z = A-B;
		break;

	case 0x2:
		if ((int)A < (int)B)
			z = 1;
		else
			z = 0;
		break;
	case 0x3:
		if (A < B)
			z = 1;
		else
			z = 0;
		break;

	case 0x4:
		z = (A & B);
		break;

	case 0x5:
		z = (A | B);
		break;

	case 0x6:
		z = (B << 16);
		break;

	case 0x7:
		z = (~A);
		break;

}
	*ALUresult = z;
	if (*ALUresult == 0)
		*Zero = 1;
	else
		*Zero = 0;

	return;

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Only returns 1 or 0, ultimately writes to the instruction

	// Instruction is an array

	if(PC % 4 != 0) 					
		return 1; 						
	else
		*instruction = Mem[PC >> 2];									

	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction & 0xfc000000) >> 26;
	*r1 = (instruction & 0x03e00000) >> 21;
	*r2 = (instruction & 0x001f0000) >> 16;
	*r3 = (instruction & 0x0000f800) >> 11;
	*funct  =  instruction & 0x0000003f;
	*offset =  instruction & 0x0000ffff;
	*jsec   =  instruction & 0x03ffffff;
	return;

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // R-Type
    if(op == 0)
    {
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        return 0;
    }
    // Addi
    if(op == 8)
    {
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 0;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 0;
        return 0;
    }
    // Load Word
    if(op == 35)
    {
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 0;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 1;
        controls->MemWrite = 0;
        controls->MemtoReg = 1;
        return 0;
    }
    // Store Word
    if(op == 43)
    {
        controls->RegDst = 2;
        controls->RegWrite = 0;
        controls->ALUSrc = 1;
        controls->ALUOp = 0;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 1;
        controls->MemtoReg = 2;
        return 0;
    }
    // Load Upper Immediate
    if(op == 15)
    {
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 6;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 0;
        return 0;
    }
    // Branch on Equal
    if(op == 4)
    {
        controls->RegDst = 2;
        controls->RegWrite = 0;
        controls->ALUSrc = 0;
        controls->ALUOp = 1;
        controls->Branch = 1;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 2;
        return 0;
    }
    // Set on Less Than Immediate
    if(op == 10)
    {
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 2;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 0;
        return 0;
    }
    // Set on Less Than Immediate Unsigned
    if(op == 11)
    {
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 3;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 0;
        return 0;
    }
    // Jump
    if(op == 2)
    {
        controls->RegDst = 2;
        controls->RegWrite = 0;
        controls->ALUSrc = 2;
        controls->ALUOp = 0;
        controls->Branch = 0;
        controls->Jump = 1;
        controls->MemRead = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 2;
        return 0;
    }
    return 1;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    if((offset >> 15) == 1) 				
		*extended_value = offset | 0xffff0000;	
	else
		*extended_value = offset & 0x0000ffff;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    int check =0;

    if(ALUSrc == 1)
    {
        data2 = extended_value;
    }

   if(ALUOp == 7)
    {
        // Determine new ALU operation
        // Add
        if(funct == 32)
        {
            ALU(data1, data2, 0, ALUresult, Zero);
            check =1;  
        }
            // Subtract
            if(funct == 34)
            {
                ALU(data1, data2, 1, ALUresult, Zero);
                check =1;
            }
            // Less than signed
            if(funct == 42)
            {
                ALU(data1, data2, 2, ALUresult, Zero);
                check =1;
            }
            // Less than unsigned
            if(funct == 43)
            {
                ALU(data1, data2, 3, ALUresult, Zero);
                check =1;
            }
            // And
            if(funct == 36)
            {
                ALU(data1, data2, 4, ALUresult, Zero);
                check =1;
            }
            // Or
            if(funct == 37)
            {
                ALU(data1, data2, 5, ALUresult, Zero);
                check =1;
            }
            // Shift Left
            if(funct == 4)
            {
                ALU(data1, data2, 6, ALUresult, Zero);
                check =1;
            }
            // Not
            if(funct == 39)
            {
                ALU(data1, data2, 7, ALUresult, Zero);
                check =1;
            }

           if (check == 0)
           {
                return 1;
           }

      }   

     if (ALUOp != 7)   
        {
            ALU(data1, data2, ALUOp, ALUresult, Zero);
        }

    return 0;
}

   


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	// CHECKING ADDRESS VALIDITY
	//READING OR WRITING
    if(MemRead == 1)
	{ // Read data sroted in Mem with Adress ALUresult
        if (ALUresult % 4 != 0)
    	{ // Address wrong size so Halt
        	return 1;
    	}
        else
        {
		  *memdata = Mem[ALUresult >> 2];
        }
	}
    
    if(MemWrite == 1)
    { // Write to Mem with Address ALUresult
        if (ALUresult % 4 != 0)
    	{ // Address wrong size so Halt
       		return 1;
   	 	}
        else
        {
            Mem[ALUresult >> 2] = data2;
        }
	}
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == 1)
    {
        if(MemtoReg == 1)
        { // Load Word
			Reg[r2] = memdata;
		}
        else
        { // Other Instruction
            if(RegDst == 1)
            { // Write to rd
				Reg[r3] = ALUresult;
			}
            else
            {  // Write to rt
				Reg[r2] = ALUresult;
			}
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
     
    *PC += 4;

    if((Zero == 1) && (Branch == 1))
    {
        *PC += extended_value << 2;
    }

    if(Jump == 1)
    {
        *PC = ((jsec << 2) | (*PC & 0xf0000000));

    }

}
