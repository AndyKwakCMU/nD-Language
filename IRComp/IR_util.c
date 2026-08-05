// ========================================================================= //
// Andy Kwak 2026

// Disassembler for the IR bytecode -- for hand-verifying IRComp's output
// against IR.h's opcode reference, since there's no VM/JIT yet to run it.

// ========================================================================= //
#include <stdio.h>
#include <stdint.h>

#include "IR.h"
#include "IR_util.h"

// ========================================================================= //
void print_IR (IR_Program* I, IR* fun, size_t index)
{
        printf ("Function #%zu: num_args=%u num_vars=%u num_inst=%u\n",
                index, fun->num_args, fun->num_vars, fun->num_inst);

        uint16_t pc = 0;
        while (pc < fun->num_inst) {
                uint8_t op = fun->inst[pc];
                printf ("  %5u: ", pc);

                switch (op) {
                        case 0x00 :
                                printf ("nop\n");
                                pc += 1;
                                break;
                        case 0x57 :
                                printf ("pop\n");
                                pc += 1;
                                break;
                        case 0x59 :
                                printf ("dup\n");
                                pc += 1;
                                break;
                        case 0x5F :
                                printf ("swap\n");
                                pc += 1;
                                break;
                        case 0x60 :
                                printf ("iadd\n");
                                pc += 1;
                                break;
                        case 0x64 :
                                printf ("isub\n");
                                pc += 1;
                                break;
                        case 0x68 :
                                printf ("imul\n");
                                pc += 1;
                                break;
                        case 0x6C :
                                printf ("idiv\n");
                                pc += 1;
                                break;
                        case 0x6F :
                                printf ("ineg\n");
                                pc += 1;
                                break;
                        case 0x61 :
                                printf ("icmpeq\n");
                                pc += 1;
                                break;
                        case 0x62 :
                                printf ("icmplt\n");
                                pc += 1;
                                break;
                        case 0x63 :
                                printf ("icmpleq\n");
                                pc += 1;
                                break;
                        case 0x65 :
                                printf ("icmpgt\n");
                                pc += 1;
                                break;
                        case 0x66 :
                                printf ("icmpgeq\n");
                                pc += 1;
                                break;
                        case 0x10 : {
                                uint8_t b = fun->inst[pc + 1];
                                printf ("bipush %u\n", b);
                                pc += 2;
                                break;
                        }
                        case 0x13 : {
                                uint8_t c1 = fun->inst[pc + 1];
                                uint8_t c2 = fun->inst[pc + 2];
                                uint16_t idx = (uint16_t) (((uint16_t) c1 << 8) | c2);
                                int32_t val = (idx < I->num_int) ? I->int_pool[idx] : 0;
                                printf ("ildc %u\t; int_pool[%u] = %d\n", idx, idx, val);
                                pc += 3;
                                break;
                        }
                        case 0x14 : {
                                uint8_t c1 = fun->inst[pc + 1];
                                uint8_t c2 = fun->inst[pc + 2];
                                uint16_t idx = (uint16_t) (((uint16_t) c1 << 8) | c2);
                                printf ("aldc %u\n", idx);
                                pc += 3;
                                break;
                        }
                        case 0x15 : {
                                uint8_t i = fun->inst[pc + 1];
                                printf ("vload %u\n", i);
                                pc += 2;
                                break;
                        }
                        case 0x36 : {
                                uint8_t i = fun->inst[pc + 1];
                                printf ("vstore %u\n", i);
                                pc += 2;
                                break;
                        }
                        case 0x9F : {
                                uint8_t o1 = fun->inst[pc + 1];
                                uint8_t o2 = fun->inst[pc + 2];
                                int16_t offset = (int16_t) (((uint16_t) o1 << 8) | o2);
                                printf ("if %d\t; -> %d\n", offset, (int) pc + offset);
                                pc += 3;
                                break;
                        }
                        case 0xA7 : {
                                uint8_t o1 = fun->inst[pc + 1];
                                uint8_t o2 = fun->inst[pc + 2];
                                int16_t offset = (int16_t) (((uint16_t) o1 << 8) | o2);
                                printf ("goto %d\t; -> %d\n", offset, (int) pc + offset);
                                pc += 3;
                                break;
                        }
                        case 0xB8 : {
                                uint8_t c1 = fun->inst[pc + 1];
                                uint8_t c2 = fun->inst[pc + 2];
                                uint16_t idx = (uint16_t) (((uint16_t) c1 << 8) | c2);
                                printf ("invokestatic %u\n", idx);
                                pc += 3;
                                break;
                        }
                        case 0xB0 :
                                printf ("return\n");
                                pc += 1;
                                break;
                        case 0x2E :
                                printf ("ptload\n");
                                pc += 1;
                                break;
                        case 0x4E :
                                printf ("ptstore\n");
                                pc += 1;
                                break;
                        case 0x2F :
                                printf ("dyload\n");
                                pc += 1;
                                break;
                        case 0x4F :
                                printf ("dystore\n");
                                pc += 1;
                                break;
                        case 0x6E :
                                printf ("sload\n");
                                pc += 1;
                                break;
                        default :
                                printf ("<unknown opcode 0x%02X>\n", op);
                                pc += 1;
                                break;
                }
        }
}

void print_IR_Program (IR_Program* I)
{
        printf ("=== IR_Program disassembly ===\n");

        printf ("int_pool (%u):\n", I->num_int);
        uint16_t k = 0;
        while (k < I->num_int) {
                printf ("  [%u] = %d\n", k, I->int_pool[k]);
                k++;
        }

        printf ("str_pool (%u):\n", I->num_str);
        k = 0;
        while (k < I->num_str) {
                printf ("  [%u] = \"%s\"\n", k, I->str_pool[k]);
                k++;
        }

        printf ("functions (%u):\n", I->num_fun);
        k = 0;
        while (k < I->num_fun) {
                print_IR (I, I->fun_pool[k], k);
                k++;
        }
}

// ========================================================================= //
