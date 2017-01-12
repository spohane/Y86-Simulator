#include "y86emul.h"

/*union used to read and write 4byte integers*/
union interpretInt u;

/*memory of the y86 program */
unsigned char *memory;

/*start point of instructions in memory*/
unsigned int instructionStart_end;

/*stores the size of the allocated memory*/
unsigned int memSize;

/* program counter */
unsigned int pc;

/*stores the highest address of an instruction
or value that will determine the size of the stack*/
unsigned int stackLimit;

/*defines program status: AOK, HLT, ADR, INS, STC<-stack overflow*/
status programStatus;

/*
     REGISTERS
    ------------
      eax 0
      ecx 1
      edx 2
      ebx 3
      esp 4
      ebp 5
      esi 6
      edi 7
*/
int reg[8];
int icount=0;
char DEBUG='T';
char STR[5];

/*
           FLAGS
--------------------------
    0 - ZF zero flag
    1 - OF overflow flag
    2 - SF sign flag
*/
int flag[3];


int main(int argc, char ** argv)
{
    FILE *fp;/*used to read in the y86 file*/
    int c;/*used for processing characters from the file*/
    unsigned int address;/*used to process address from file*/
    unsigned int data;/*used to process byte data from file*/
    char byteInterpret[3];/*used in breaking down the y86 instructions*/
    char *ident;/*used to identify .size/.byte/etc instructions*/
    int dotText;/*used to track the number of .text directives */


.........
     .........
     .........
     .........
     .........
    fclose(fp);
    free(memory);
    free(ident);
    return 0;
}

/*function executes the instructions stored in memory*/
void execute()
{
    /* Variable explanation:
                 ________________________________________________
    e.g. rmmovl: | 4   |  0   |  rA   | rB | 32-bit displacement|
                   |      |       |      |
                  num1   num2    src    dst
     */
    int num1, num2;
    int src, dst;
    int byte;/*used in calculating num1,num2*/
   icount=0;
   DEBUG='T';
    while(1)
    {
        if(!validAddress(pc))
        {
            /*invalid address encountered*/
            break;
        }
        if(pc == instructionStart_end)
        {
            /*used to detect end of instruction*/
            break;
        }

        byte = memory[pc];
        num2 = byte %16;
        byte = byte/16;
        num1 = byte % 16;
//DEBUG//printf("\n num1:%d num2:%d m[pcit]:%d m[pc]:%x m[pc+1]:%x m[pc+2]:%x int(pc+2):%x ",num1,num2,memory[pc],memory[pc],memory[pc+1],memory[pc+2],readInt(pc+2));
 //DEBUG//

 if (memory[pc]==96)
 {//printf("MATCH");
  strcpy(STR,"ADDL");
 //*STR='ADDL';
 }
 else if (memory[pc]==101)
 strcpy(STR,"CMPL");
.........
     .........
     .........
     .........
     
            }

    }


}
