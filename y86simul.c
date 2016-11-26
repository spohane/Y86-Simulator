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


/*
    if(argc < 2)
    {
        fprintf(stderr, "%s\n", "ERROR. You did not enter enough arguments for the program to run properly.");
        return 0;
    }
    if(argc>2)
    {
        fprintf(stderr, "%s\n", "ERROR. You entered too many arguments.");
        return 0;
    }
    if(strcmp(argv[1], "-h")==0)
    {
        printf("Usage: y86emul <y86 instruction file>\n");
        return 0;
    }
*/

//argv[1]="prog1w.y86";
argv[1]="prog5.y86";
    dotText=0;
    programStatus = AOK;
    stackLimit=0;

    ident= (char *)malloc(sizeof(char)*8);


    if( (fp = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "%s\n", "ERROR. The file you provided does not exist.");
        return 0;
    }

    /*allocate the amount of memory needed specified by the program*/
    fscanf(fp, "%s\t%x", ident, &memSize);

    if(strcmp(ident, ".size")!=0)
    {
        fprintf(stderr, "%s\n", "ERROR. The file you provided does contain the .size directive as the first line.");
        return 0;
    }
    memory = (unsigned char *)malloc(sizeof(unsigned char)*memSize);

    while( fscanf(fp, "%s", ident) != EOF)
    {
        if (strcmp(ident, ".text")==0)
        {
            /*interpreting the .text line byte by byte and storing it in memory*/
            fscanf(fp, "\t %x \t", &instructionStart_end);
            pc = instructionStart_end;
            while( ((c = fgetc(fp)) != '\n')  )
            {
                byteInterpret[0]=c;

                c=fgetc(fp);
                byteInterpret[1]=c;
                byteInterpret[2]='\0';


                sscanf(byteInterpret, "%x", &data);

                memory[instructionStart_end]=data;

//                printf("byteInterpret:%s data:%x byteInterpret[0]:%c byteInterpret[1]:%c instructionStart_end %x memory[instructionStart_end]:%x \n",byteInterpret,data,byteInterpret[0], byteInterpret[1], instructionStart_end, memory[instructionStart_end]);
                //printf("instructionStart_end %x memory[instructionStart_end]:%x \n", instructionStart_end, memory[instructionStart_end]);


  if(icount>855)
{
DEBUG='T';
 }
   if(DEBUG=='T')
                printf("%x\t%x \n", instructionStart_end, memory[instructionStart_end]);
                instructionStart_end++;

            }

            /*used for finding the largest memory address being used*/
            setStackLimit(instructionStart_end);
            dotText++;
        }
        else if(strcmp(ident, ".byte")==0)
        {
            /*store byte in memory */
            fscanf(fp, "\t %x \t %x",&address, &data);

            /*used for finding the largest memory address being used*/
            setStackLimit(address);

            memory[address]=data;
            icount=icount+1;
                printf("%d Byte addr %x\t memory[addr] %x \n", icount,address, memory[address]);

        }
        else if(strcmp(ident, ".string")==0)
        {
            /*store .string in memory*/
            fscanf(fp, "\t %x \t", &address);

            c=fgetc(fp);
            while((c = fgetc(fp)) != '\"')
            {
                memory[address]=c;
                address++;
            }

            /*used for finding the largest memory address being used*/
            setStackLimit(address);

        }
        else if(strcmp(ident, ".long")==0)
        {
            /*store .long in memory*/
            fscanf(fp, "\t %x \t %x", &address, &data);

            /*used for finding the largest memory address being used*/
            setStackLimit(address);

            writeInt(address,data);
        }
        else if(strcmp(ident, ".bss")==0)
        {
            /*professor said to do nothing with .bss
            so I'm doing nothing when/if this instruction
            is encountered*/
        }

    }

    if(dotText==0)
    {
        fprintf(stderr, "%s\n", "ERROR. No \'.text\' directive was found in the file.");
        return 0;
    }
    else if(dotText>=2)
    {
        printf("Warning. More than one \'.text\' directive was found in the file.\n");
    }

    /*run the program*/
    execute();

    /*print program status*/
    printStatus();

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
 else if (memory[pc]==115)
  strcpy(STR,"JE  ");
 else if (memory[pc]==208)
  strcpy(STR,"WRIT");
   else if (memory[pc]==112)
  strcpy(STR,"JMP ");
 else if (memory[pc]==224)
  strcpy(STR,"MSBL");
   else if (memory[pc]==48)
 strcpy(STR,"IRMOVL");
 else if (memory[pc]==32)
  strcpy(STR,"RRMOVL");
 else if (memory[pc]==128)
  strcpy(STR,"CALL");
   else if (memory[pc]==160)
  strcpy(STR,"PUSHL");
 else if (memory[pc]==97)
  strcpy(STR,"SUBL");
   else if (memory[pc]==116)
  strcpy(STR,"JNE");
     else if (memory[pc]==193)
  strcpy(STR,"*--READL");
       else if (memory[pc]==80)
  strcpy(STR,"MRMOVL");
        else if (memory[pc]==64)
  strcpy(STR,"RMMOVL");
         else if (memory[pc]==98)
  strcpy(STR,"ANDL");
    else
  strcpy(STR,"   ");


 if(DEBUG=='T')
// printf("\n %d %s m[pc]:%x m[pc-1]:%x m[pc+1]:%x m[pc+2]:%x m[pc+3]:%x int(pc+2):%x ",icount,STR,memory[pc],memory[pc-1],memory[pc+1],memory[pc+2],memory[pc+3],readInt(pc+2));
//DEBUG//getch();

getch();
        if(num1 ==0)

        {
            /*NOP*/
            pc++;
        }
        else if(num1 ==1)
        {
            /*HALT*/
            programStatus=HLT;
            break;
        }
        else if(num1 ==2)
        {
            /*RRMOVL*/
            if(!validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }
            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            reg[dst]=reg[src];

            pc+=2;
        }
        else if(num1 ==3)
        {
            /*IRMOVL*/
            if( !validAddress(pc+1) || !validAddress(pc+2) )
            {
                /*invalid address encountered*/
                break;
            }
            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            reg[dst]= readInt(pc+2);

            pc+=6;

        }
        else if(num1 ==4)
        {
            /*RMMOVL*/
            if( !validAddress(pc+1) || !validAddress(pc+2) )
            {
                /*invalid address encountered*/
                break;
            }
            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            if( !validAddress(readInt(pc+2)+reg[dst]) )
            {
                /*invalid address encountered*/
                break;
            }
            writeInt(readInt(pc+2)+reg[dst],reg[src]);

            pc+=6;
        }
        else if(num1 ==5)
        {
            /*MRMOVL*/
            if( !validAddress(pc+1) || !validAddress(pc+2) )
            {
                /*invalid address encountered*/
                break;
            }
            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            if( !validAddress(readInt(pc+2)+reg[dst]) )
            {
                /*invalid address encountered*/
                break;
            }

            reg[src]=readInt(readInt(pc+2)+reg[dst]);

            pc+=6;
        }
        else if(num1 ==6)
        {
            /*OPL
            --------------
                 0--addl
                 1--subl
                 2--andl
                 3--xorl
                 4--mull
                 5--divl
            */

            int result;

            if(!validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            if(num2==0)
            {
                /*ADDL*/
                result = reg[dst]+reg[src];

                setFlags(reg[dst], reg[src], result, num2);
                if(flag[1]==1)
                {
                    printf("Overflow occured on the following instruction: 60%i%i\n", src, dst);
                }

                reg[dst]=result;
            }
            else if (num2==1)
            {
                /*SUBL*/
                result = reg[dst]-reg[src];

                setFlags(reg[src], reg[dst], result, num2);
                if(flag[1]==1)
                {
                    printf("Overflow occured on the following instruction: 61%i%i\n", src, dst);
                }

                reg[dst]=result;
            }
            else if(num2 ==2)
            {
                /*ANDL*/
                result = reg[dst]&reg[src];

                setFlags(reg[dst], reg[src], result, num2);

                reg[dst]=result;
            }
            else if(num2 ==3)
            {
                /*XORL*/
                result = reg[dst]^reg[src];

                setFlags(reg[dst], reg[src], result, num2);

                reg[dst]=result;
            }
            else if(num2 ==4)
            {
                /*MULL*/
                result = reg[dst]*reg[src];

                setFlags(reg[dst], reg[src], result, num2);
                if(flag[1]==1)
                {
                    printf("Overflow occured on the following instruction: 64%i%i\n", src, dst);
                }

                reg[dst]=result;
            }
            else if(num2 ==5)
            {
/*                /*DIVL
                result = reg[dst]/reg[src];

                setFlags(reg[dst], reg[src], result, num2);
                if(flag[1]==1)
                {
                    printf("Overflow occured on the following instruction: 65%i%i\n", src, dst);
                }

                reg[dst]=result;
*/


               /*CMPL*/


//                setFlags(reg[dst], reg[src], result, num2);
/*
           FLAGS
--------------------------
    0 - ZF zero flag
    1 - OF overflow flag
    2 - SF sign flag
*/
    //           int result=0;

  reg[dst]=11;
//                result=reg[dst]-reg[src];
result=reg[dst]-reg[src];


               if (result==0)
                {flag[0]=1;
                flag[1]=0;
                flag[2]=0;
                }
                else if(result<0)
                {flag[0]=0;
                flag[1]=0;
                flag[2]=1;

                }
                else {flag[0]=0;
                flag[1]=0;
                flag[2]=0;
                }
//memory[pc+1]=11;
//memory[pc+2]=11;


            //    setFlags(reg[dst], reg[src], result, num2);


            }

            pc+=2;
        }
        else if(num1 ==7)
        {
            /*JXX jump*/

            /*     FLAGS
               --------------------
                0 - ZF zero flag
                1 - OF overflow flag
                2 - SF sign flag
            */
            int destination;

            if(!validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            destination = readInt(pc+1);

            if(!validAddress(destination))
            {
                /*invalid address encountered*/
                break;
            }

            if(num2==0)
            {
                /*jmp*/

                pc=destination;
                continue;
            }
            else if(num2==1)
            {
                /*jle*/
                /*(SF^OF)|ZF*/
                if( (flag[2]^flag[1]) | flag[0])
                {
                    pc=destination;
                    continue;
                }
            }
            else if(num2==2)
            {
                /*jl*/
                /*SF^OF*/
                if( (flag[2]^flag[1]))
                {
                    pc=destination;
                    continue;
                }
            }
            else if(num2==3)
            {
                /*je*/
                /*ZF*/
                if( (flag[0]))
                {
                    pc=destination;

                    continue;
                }
            }
            else if(num2==4)
            {
                /*jne*/
                /*~ZF*/
                if( !(flag[0]))
                {
                    pc=destination;
                    continue;
                }
            }
            else if(num2==5)
            {
                /*jge*/
                /*~(SF^OF)*/
                if( !(flag[2]^flag[1]) )
                {
                    pc=destination;
                    continue;
                }
            }
            else if(num2==6)
            {
                /*jg*/
                /*~(SF^OF) & ~ZF*/
                if( !(flag[2]^flag[1]) & (!flag[0]) )
                {
                    pc=destination;
                    continue;
                }
            }

            pc+=5;
        }
        else if(num1 ==8)
        {
            /*CALL*/
            if(!validAddress(pc+5) || !validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            if(push(pc+5))
            {
                /*stack overflow*/
                break;
            }

            pc = readInt(pc+1);
            continue;
        }
        else if(num1 ==9)
        {
            /*RET*/

            pc = pop();

        }
        else if(num1 ==10)
        {
            /*PUSHL*/

            /*eax 0
              ecx 1
              edx 2
              ebx 3
              esp 4
              ebp 5
              esi 6
              edi 7
            */

            if(!validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            if(push(reg[src]))
            {
                /*stack overflow*/
                break;
            }
            pc+=2;

        }
        else if(num1 ==11)
        {
            /*POPL*/

            if(!validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            reg[src]=pop();
            pc+=2;
        }
        else if(num1 ==12)
        {
            /*READX*/
            int displacement;

            if(!validAddress(pc+2) || !validAddress(pc+1))
            {
                /*invalid address encountered*/
                break;
            }

            displacement = readInt(pc+2);
            calcSRCDST(&src, &dst, memory[pc+1]);
            setFlags(0, 0, 0, 12);
            if(num2==0)
            {
                int process;
                /*read single ASCII character (byte) into memory*/
                char charByte;
                flag[0]=0;
                process = scanf("%c", &charByte);
                if(process== EOF)
                {
                    flag[0]=1;
                }
                else
                {
                    if(!validAddress(reg[src]+displacement))
                    {
                        /*invalid address encountered*/
                        break;
                    }
                    memory[reg[src]+displacement]=charByte;
                }

            }
            else if (num2==1)
            {
                int process;
                /*read in single ASCII decimal number (word) into memory*/
                flag[0]=0;
                process = scanf("%d", &byte); /*byte variable used
                                    here just to process data, name
                                    has no meaning here*/
                if(process== EOF)
                {
                    flag[0]=1;
                }
                else
                {
                    if(!validAddress(reg[src]+displacement))
                    {
                        /*invalid address encountered*/
                        break;
                    }
                    writeInt(reg[src]+displacement, byte);
                }

            }
            pc+=6;
        }
        else if(num1 ==13)
        {
            /*WRITEX*/
            int displacement;

            if(!validAddress(pc+1) || !validAddress(pc+2))
            {
                /*invalid address encountered*/
                break;
            }

            displacement = readInt(pc+2);
            calcSRCDST(&src, &dst, memory[pc+1]);

            if(!validAddress(reg[src]+displacement) )
            {
                /*invalid address encountered*/
                break;
            }
            if(num2==0)
            {
                /*write byte to terminal*/
                printf("%c", memory[reg[src]+displacement]);

                 if(DEBUG=='T')
                printf("memory[reg[src]+displacement]=%x ",memory[reg[src]+displacement]);

            }
            else if(num2==1)
            {
                /*write word to terminal*/
                printf("%d", readInt(reg[src]+displacement));
            }
            pc+=6;
        }

        else if(num1 ==14)
        {
            /*MOVSBL*/
            if( !validAddress(pc+1) || !validAddress(pc+2) )
            {
                /*invalid address encountered*/
                break;
            }
            /*calculates the source and destination variables*/
            calcSRCDST(&src, &dst, memory[pc+1]);

            if( !validAddress(readInt(pc+2)+reg[dst]) )
            {
                /*invalid address encountered*/
                break;
            }

//long int li=0x00000000;
//reg[dst]=reg[dst]|li;
//if ((reg[dst]-reg[src])<0)
//flag[2]=1;

// reg[dst]='ffffffff';



//printf("\nLong int li:%ld",li);
//printf("\nreg[dst]||0xFFFFF00:");
//dec_hex(reg[dst]|li);
//dec_hex(reg[dst]);

/*
printf("\n***START BEFORE\n");
printf("\nreg[dst]:");
dec_hex(reg[dst]);
printf("\nreg[dst]:");
dec_hex(reg[dst]);
printf("\nreadInt(pc+2):");
dec_hex(readInt(pc+2));
//printf("\n***END BEFORE\n");
//printf("BEFORE readInt(pc+2) =%ld +hex-reg[dst]= %ld reg[src]=%ld",readInt(pc+2),dec_hex(reg[dst]),reg[src]);
printf("\n\nBEFORE readInt(pc+2) =%ld +reg[dst]= %ld reg[src]=%ld",readInt(pc+2),reg[dst],reg[src]);
 //          reg[src]=readInt(readInt(pc+2)+reg[dst]);

  */

//            reg[dst]=reg[dst]|li;
  // reg[dst]=reg[dst]|li;


//printf("\n\nAAFTER readInt(pc+2) =%ld +reg[dst]= %ld reg[src]=%ld\n\n",readInt(pc+2),reg[dst],reg[src]);
//  dec_hex(reg[src]);
//         getch();
icount=icount+1;
if(icount>855)
{
DEBUG='T';
    getch();

}

           pc+=6;
        }


        else
        {
            /*invalid instruction*/
            programStatus = INS;
            break;








        }

 //DEBUG//      printf("    reg[dst]:%x reg[src]:%x ",reg[dst],reg[src]);
if (DEBUG=='T')
  printf("    reg[dst]:%x reg[src]:%x ",reg[dst],reg[src]);

    }
    return;
}

/*calculates the source and destination registers based
on the byte provided*/
void calcSRCDST(int *src, int*dst, int byte)
{
    *dst = byte %16;
    byte = byte/16;
    *src = byte % 16;
 //DEBUG//
 if(DEBUG=='T')
 printf("       calcSRCDST src:%x dst:%x ",*src,*dst);
}

/*recreates and returns the integer that is stored across
four memory locations*/
int readInt(int loc)
{
    u.byte[0]= memory[loc];
    loc++;
    u.byte[1]= memory[loc];
    loc++;
    u.byte[2]= memory[loc];
    loc++;
    u.byte[3]= memory[loc];
    return u.value;
}

/*given the location and integer value to be stored,
the fuction breaks down the integer and stores it across
four meomry locations*/
void writeInt(int loc, int value)
{
    u.value = value;
    memory[loc]= u.byte[0];
    loc++;
    memory[loc]= u.byte[1];
    loc++;
    memory[loc]= u.byte[2];
    loc++;
    memory[loc]= u.byte[3];
}

/*function that simulates a push of a register given
the value to be stored on the stack. Returns 1 if stack
overflow occured, 0 if not*/
int push(int value)
{
    reg[4]= reg[4]-4;
    if(reg[4]<=stackLimit)
    {
        /*stack overflow*/
        programStatus=STC;
        return 1;
    }
    writeInt(reg[4], value);
    return 0;
}

/*funtion simulates the pop and returns the value
that was stored on the stack*/
int pop()
{
    int value=readInt(reg[4]);
    reg[4]= reg[4]+4;
    return value;
}

/*given an address, the function determines
whether the address is within the bounds of
the allocated memory space. returns 0 if errorenous
address, 1 if correct address*/
int validAddress(int addr)
{
    if( (addr<0) || (addr>=memSize) )
    {
        programStatus=ADR;
        return 0;
    }
    return 1;
}

/*sets the value of the stackLimit variable
which is used to determine stack overflow*/
void setStackLimit(int value)
{
    if(value>stackLimit)
    {
        stackLimit=value;
    }
}

/*function prints program status at the end
of execution*/
void printStatus()
{
    if(programStatus==AOK)
    {
        printf("Program Status: AOK. Program executed correctly.\n");
    }
    else if(programStatus==HLT)
    {
        printf("Program Status: HLT. Program encountered a halt instruction.\n");
    }
    else if(programStatus==ADR)
    {
        printf("Program Status: ADR. Program encountered an invalid address.\n");
    }
    else if(programStatus==INS)
    {
        printf("Program Status: INS. Program encountered an invalid instruction.\n");
    }
    else if(programStatus==STC)
    {
        printf("Program Status: STC. Program stoped execution do to a stack overflow.\n");
    }
}

/*function sets the condition flags based on the two values,
operation result, and operation code provided*/
void setFlags(int val1, int val2, int result, int opCode)
{
    /*Operation Codes
    -----------------
    0 -- addl
    1 -- subl
    2 -- andl
    3 -- xorl
    4 -- mull
    5 -- divl
    12-- readb/readw
    */
    /*FLAGS
    -----------------------
    0 - ZF zero flag
    1 - OF overflow flag
    2 - SF sign flag
    */
    flag[0]=0;
    flag[1]=0;
    flag[2]=0;
    if(result==0)
    {
        /*zero result*/
        flag[0]=1;
        flag[1]=0;
        flag[2]=0;
    }
    else if(result<0)
    {
        /*negative result*/
        flag[0]=0;
        flag[1]=0;
        flag[2]=1;
    }
    else
    {
        if(opCode==0)
        {
            /*addl*/
            if( ( (val1<0)==(val2<0) ) && ( (result<0)!=(val1<0) ) )
            {
                /*overflow occured*/
                flag[0]=0;
                flag[1]=1;
                flag[2]=0;
            }
        }
        else if(opCode==1)
        {
            /*subl*/
            if( (val1<0 && val2>0 && result <0) || (val1>0 && val2<0 && result >0) )
            {
                /*overflow occured*/
                flag[0]=0;
                flag[1]=1;
                flag[2]=0;
            }
        }
        else if(opCode==4)
        {
            /*mull*/
            if( (result/val2)!=val1 )
            {
                /*overflow occured*/
                flag[0]=0;
                flag[1]=1;
                flag[2]=0;
            }
        }
        else if(opCode==5)
        {
/*            /*divl
            if( (val1==-2147483648 && val2 ==-1) || (val2==-2147483648 && val1 ==-1) )
            {
                /*overflow occured
                flag[0]=0;
                flag[1]=1;
                flag[2]=0;
            }
*/



        }

    }

    if(opCode==2 || opCode ==3)
    {
        /*andl xorl*/
        flag[1]=0;
    }
    if(opCode==12)
    {
        /*readb readw*/
        flag[1]=0;
        flag[2]=0;
    }


}
