#include <stdio.h>
#include <stdbool.h>

#define numBits 8
#define numSettings 19
#define stack_size 4

#define debugCycles 100

#define OR(a, b) a + b - a*b
#define NOR(a, b) 1 - a - b + a*b
#define AND(a, b) a*b

#define R1  1, 0, 0, 0
#define R2  0, 1, 0, 0
#define R3  1, 1, 0, 0
#define R4  0, 0, 1, 0
#define R5  1, 0, 1, 0
#define R6  0, 1, 1, 0
#define R7  1, 1, 1, 0
#define R8  0, 0, 0, 1
#define R9  1, 0, 0, 1
#define R10 0, 1, 0, 1
#define R11 1, 1, 0, 1
#define R12 0, 0, 1, 1
#define R13 1, 0, 1, 1
#define R14 0, 1, 1, 1
#define R15 1, 1, 1, 1

#define ADD 0, 1, 0, 0,
#define XOR 0, 1, 1, 0,
#define LDI 0, 0, 0, 1,
#define ADI 1, 0, 0, 1,
#define HLT 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#define JMP 0, 1, 0, 1,
#define BRH 1, 1, 0, 1,
#define CAL 0, 0, 1, 1,
#define RET 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#define LOD 0, 1, 1, 1,
#define STR 1, 1, 1, 1,

typedef struct d_latch {
    bool q;
    bool qbar;
} d_latch;

typedef struct register_file {
    d_latch registers[15][numBits];    
} register_file;

typedef struct instruction_memory {
    bool instructions[32][32][16];
} instruction_memory;

typedef struct program_counter {
    d_latch registers[10];    
} program_counter;

typedef struct alu_flags {
    d_latch flags[2];
} alu_flags;

typedef struct call_stack {
    d_latch registers[stack_size][10];
} call_stack;

typedef struct data_memory {
    d_latch data[16][16][numBits];
} data_memory;

void init_d_latch(d_latch* d_latch);
void read_able_update_d_latch(bool enable, bool data, d_latch* d_latch);
void update_d_latch(bool enable, bool data, d_latch* d_latch);
void printDLatch(d_latch* d_latch);
void init_register_file(register_file* register_file);
void write_to_register(register_file* register_file, bool enable, bool address[4], bool data[numBits]);
void read_from_register(register_file* register_file, bool address[4], bool output[numBits]);
void print_register_file(register_file* register_file);

void alu(bool A[numBits], bool B[numBits], bool S[numBits], bool *cout, bool cin, bool invertA, bool invertB, bool flood_carry, bool xorToOr, bool invertS, bool rightShift);

void set_instruction(bool bits[32][32][16], int instruction_index,
                     bool b0, bool b1, bool b2, bool b3, bool b4, bool b5,
                     bool b6, bool b7, bool b8, bool b9, bool b10,
                     bool b11, bool b12, bool b13, bool b14, bool b15);
void set_10bits(bool data[10], bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9);                     
void set_8bits(bool data[numBits], bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
void set_4bits(bool address[4], bool b0, bool b1, bool b2, bool b3);

void jump_mux(bool condition, bool conditionRequired, bool switch_in, /*bool use_stack_output,*/ bool address[10], bool jmp_addr[10], /*bool stack_output[10],*/ bool output[10]);
void data_mux(bool switch_in, bool data[numBits], bool data2[numBits], bool output[numBits]);
void dest_mux(bool switch_in, bool address[4], bool address2[4], bool out_addr[4]);
void control_rom(bool opcode[4], bool settings[numSettings]);

void init_instruction_memory(instruction_memory* instruction_memory);
void read_instruction_memory(instruction_memory* im, bool address[10], bool instruction[16]);
void print_instruction_address(bool address[10]);

void init_program_counter(program_counter* program_counter);
void increment_program_counter(program_counter* program_counter);
void write_to_program_counter(program_counter* program_counter, bool enable, bool data[10]);
void read_from_program_counter(program_counter* program_counter, bool output[10]);
void print_program_counter(program_counter* program_counter);

void init_alu_flags(alu_flags* alu_flags);
void condition_flag_decoder(alu_flags *alu_flags, bool zero, bool carry, bool *condition);

void init_call_stack(call_stack* call_stack);
void print_call_stack(call_stack* call_stack);
void use_call_stack(call_stack *call_stack, bool push_addr[10], bool push_or_pop, bool enable/*, bool out_addr[10]*/);
void peek_call_stack(call_stack* call_stack, bool out_addr[10]);

void init_data_memory(data_memory* data_memory);
void print_data_memory(data_memory *data_memory);
void read_write_data_memory(data_memory* data_memory, bool address[8], bool data[numBits], bool readOrWrite, bool enable, bool output[numBits]);

int main() 
{
    register_file register_file;
    init_register_file(&register_file);
    bool address[4];
    bool d1[8], d2[8], output[8];
    bool cout;
    int cycles = 0;
    
    
    instruction_memory instruction_memory;
    init_instruction_memory(&instruction_memory);
    program_counter program_counter;
    init_program_counter(&program_counter);
    alu_flags alu_flags;
    init_alu_flags(&alu_flags);
    call_stack call_stack;
    init_call_stack(&call_stack);
    data_memory data_memory;
    init_data_memory(&data_memory);
    //if(cycles < debugCycles)
    //    print_call_stack(&call_stack);
    //alu(d2, d1, output, &cout, 0, 0, 0, 0, 0, 0, 0);
    bool output_addr[4], output_addr2[4];
    bool instruction[16];
    bool instruction_address[10];
    bool opcode[4];
    bool settings[numSettings];
    for(int i = 0; i < numSettings; i++) {
        settings[i] = 0;
    }
    bool data2[numBits];
    bool out_dest[4];
    bool out_data[numBits];
    while(1-settings[10]) {
        read_from_program_counter(&program_counter, instruction_address);
        if (cycles < debugCycles) {
            printf("current ");
            print_instruction_address(instruction_address); 
        }
        read_instruction_memory(&instruction_memory, instruction_address, instruction);
        set_4bits(output_addr, instruction[12], instruction[13], instruction[14], instruction[15]);
        //write_to_register(&register_file, output_addr, output);
        set_4bits(opcode, instruction[0],instruction[1],instruction[2],instruction[3]);
        set_4bits(address, instruction[4], instruction[5], instruction[6], instruction[7]);
        read_from_register(&register_file, address, d1);
        set_4bits(address, instruction[8], instruction[9], instruction[10], instruction[11]);
        read_from_register(&register_file, address, d2);
        
        control_rom(opcode, settings);
        set_8bits(data2, instruction[8], instruction[9], instruction[10], instruction[11], instruction[12], instruction[13], instruction[14], instruction[15]);
        bool dmOffset[numBits];
        bool n17 = 1-settings[17];
        set_8bits(dmOffset, instruction[12], instruction[13], instruction[14], instruction[15], 0, 0, 0, 0);
        data_mux(settings[11], d2, data2, d2);
        for(int i = 0; i < numBits; i++) {
            d2[i] = settings[17] * dmOffset[i] + n17*d2[i];
        }
        alu(d1, d2, output, &cout, settings[1], settings[2], settings[3], settings[4], settings[5], settings[6], settings[7]);
        bool toWriteToDataMemory[numBits];
        read_from_register(&register_file, address, toWriteToDataMemory);
        bool data_memory_output[numBits];
        read_write_data_memory(&data_memory, output, toWriteToDataMemory, settings[18], settings[17], data_memory_output);
        bool zero_output = 0;
        for(int i = 0; i < numBits; i++)
            zero_output += output[i];
        update_d_latch(settings[13], 1-zero_output, &alu_flags.flags[0]);
        update_d_latch(settings[13], cout, &alu_flags.flags[1]);
        if(cycles < debugCycles) {
            printf("zero  flag: %d \n", alu_flags.flags[0].q);
            printf("carry flag: %d \n", alu_flags.flags[1].q);
        }
        set_4bits(output_addr2, instruction[4], instruction[5], instruction[6], instruction[7]);
        
        
        data_mux(settings[8], output, data2, out_data);
        printf("output_addr: ");
        for(int i = 0; i < 4; i++) {
            printf("%d", output_addr[i]);
        }
        printf("\n");
        printf("output_addr2: ");
        for(int i = 0; i < 4; i++) {
            printf("%d", output_addr2[i]);
        }
        printf("\n");
        dest_mux(settings[9], output_addr, output_addr2, out_dest);
        if(settings[11] && cycles < debugCycles){
            printf("out1 address: ");
            for(int i = 0; i < 4; i++)
                printf("%d", output_addr[i]);
            printf("\n");
            printf("out2 address: ");
            for(int i = 0; i < 4; i++)
                printf("%d", output_addr2[i]);
            printf("\n");
            printf("dest address: ");
            for(int i = 0; i < 4; i++)
                printf("%d", out_dest[i]);
            printf("\n");
        }
        printf("reg_addr: ");
        for(int i = 0; i < 4; i++) {
            out_dest[i] = settings[17]*address[i] + n17 * out_dest[i];
            printf("%d", out_dest[i]);
        }
        printf("\n");
        printf("out_data: ");
        for(int i = 0; i < numBits; i++) {
            out_data[i] = settings[17]*data_memory_output[i] + n17 * out_data[i];
            printf("%d", out_data[i]);
        }
        printf("\n");
        
        write_to_register(&register_file, settings[0], out_dest, out_data);
        if(cycles < debugCycles)
            print_register_file(&register_file);
        //
        increment_program_counter(&program_counter);
        bool instruction_pointer[10];
        bool jump_address[10] = {instruction[6], instruction[7], instruction[8], instruction[9], instruction[10], instruction[11], instruction[12], instruction[13], instruction[14], instruction[15]};
        read_from_program_counter(&program_counter, instruction_pointer);
        bool condition = 0;
        condition_flag_decoder(&alu_flags, instruction[4], instruction[5], &condition);
        if(condition && settings[14] && cycles < debugCycles)
            printf("condition: %d\n", condition);
        bool call_stack_output_address[10];
        peek_call_stack(&call_stack, call_stack_output_address);
        
        // below forces to push/pop
        if(cycles < debugCycles && settings[15]) {
            if(settings[16] == 1)
                printf("(push)");
            else if(settings[16] == 0)
                printf("(pop)");
            print_instruction_address(call_stack_output_address);
        }
        use_call_stack(&call_stack, instruction_pointer, settings[16], settings[15]/*, call_stack_output_address*/);
        
        if(cycles < debugCycles && settings[15]) {
            if(settings[16] == 1)
                printf("(push)");
            else if(settings[16] == 0)
                printf("(pop)");
            print_instruction_address(call_stack_output_address);
        }
        
        //if (cycles < debugCycles)
        //    print_call_stack(&call_stack);
        if(cycles < debugCycles) {
            
            printf("pointer ");
            print_instruction_address(instruction_pointer);
            printf("jump ");
            print_instruction_address(jump_address);
            printf("stack ");
            print_instruction_address(call_stack_output_address);
        }
        bool req = settings[15]*(1-settings[16]);
        bool not_req = 1 - settings[15]*(1-settings[16]);
            set_10bits(jump_address, req*call_stack_output_address[0] + not_req*jump_address[0], req*call_stack_output_address[1] + not_req*jump_address[1], req*call_stack_output_address[2] + not_req*jump_address[2], req*call_stack_output_address[3] + not_req*jump_address[3], req*call_stack_output_address[4] + not_req*jump_address[4], req*call_stack_output_address[5] + not_req*jump_address[5], req*call_stack_output_address[6] + not_req*jump_address[6], req*call_stack_output_address[7] + not_req*jump_address[7], req*call_stack_output_address[8] + not_req*jump_address[8], req*call_stack_output_address[9] + not_req*jump_address[9]);
        jump_mux(condition, settings[14], settings[12], /*settings[15],*/ instruction_pointer, jump_address, /*call_stack_output_address,*/ instruction_pointer);
        // printf("next ");
        // print_instruction_address(instruction_pointer);
        if(cycles < debugCycles) {

            printf("jump_mux ");
            print_instruction_address(instruction_pointer);
        }
        write_to_program_counter(&program_counter, 1, instruction_pointer);
        cycles++;
        //print_data_memory(&data_memory);
    }
    
    return 0;
}

void alu(bool A[numBits], bool B[numBits], bool S[numBits], bool *cout, bool cin, bool invertA, bool invertB, bool flood_carry, bool xorToOr, bool invertS, bool rightShift) {
    bool P[numBits], G[numBits];
    for(int i = 0; i < numBits; i++) {
        bool copyA = (1-invertA)*A[i] + (invertA)*(1-A[i]);
        bool copyB = (1-invertB)*B[i] + (invertB)*(1-B[i]);
        P[i] = copyA + copyB - (2-xorToOr)*copyA*copyB;
        G[i] = copyA*copyB;
    }

    bool current_carry = cin + flood_carry - cin * flood_carry;

    for(int i = 0; i < numBits; i++) {
        S[i] = P[i] + current_carry - 2*P[i]*current_carry;
        current_carry = P[i] * current_carry + G[i] + flood_carry;
    }

    for(int i = 0; i < numBits; i++)
        S[i] = (1-invertS)*S[i] + invertS*(1-S[i]);

    for(int i = 0; i < numBits-1; i++)
        S[i] = (1-rightShift)*S[i] + rightShift*(S[i+1]);

    *cout = current_carry - flood_carry * current_carry;
}


void init_d_latch(d_latch* d_latch) {
    d_latch->q = false;
    d_latch->qbar = true;
}

void read_able_update_d_latch(bool enable, bool data, d_latch* d_latch) {
    bool r = AND((1 - data), enable);
    bool s = AND(enable, data);
    bool old_q = d_latch->q;
    bool old_qbar = d_latch->qbar;

    d_latch->q = NOR(r, old_qbar);
    d_latch->qbar = NOR(old_q, s);

    old_q = d_latch->q;
    old_qbar = d_latch->qbar;

    d_latch->q = NOR(r, old_qbar);
    d_latch->qbar = NOR(old_q, s);
}

void update_d_latch(bool enable, bool data, d_latch* d_latch) {
    bool s = AND(enable, data);
    bool r = enable - s;
    bool old_q = d_latch->q;
    bool old_qbar = d_latch->qbar;

    d_latch->q = NOR(r, old_qbar);
    d_latch->qbar = NOR(old_q, s);

    old_q = d_latch->q;
    old_qbar = d_latch->qbar;

    d_latch->q = NOR(r, old_qbar);
    d_latch->qbar = NOR(old_q, s);
}

void printDLatch(d_latch* d_latch) {
    printf("struct d_latch q    = %d\n", d_latch->q);
    printf("struct d_latch qbar = %d\n", d_latch->qbar);
}

void init_register_file(register_file* register_file) {
    for (int r = 0; r < 15; r++)
        for (int i = 0; i < numBits; i++)
            init_d_latch(&register_file->registers[r][i]);
}

void set_instruction(bool bits[32][32][16], int instruction_index,
                     bool b0, bool b1, bool b2, bool b3, bool b4, bool b5,
                     bool b6, bool b7, bool b8, bool b9, bool b10,
                     bool b11, bool b12, bool b13, bool b14, bool b15) {
    int row = instruction_index / 32;  // high 5 bits
    int col = instruction_index % 32;  // low 5 bits
    bits[row][col][0]  = b0;
    bits[row][col][1]  = b1;
    bits[row][col][2] = b2;
    bits[row][col][3] = b3;
    bits[row][col][4] = b4;
    bits[row][col][5] = b5;
    bits[row][col][6] = b6;
    bits[row][col][7] = b7;
    bits[row][col][8] = b8;
    bits[row][col][9] = b9;
    bits[row][col][10] = b10;
    bits[row][col][11] = b11;
    bits[row][col][12] = b12;
    bits[row][col][13] = b13;
    bits[row][col][14] = b14;
    bits[row][col][15] = b15;
}

void set_10bits(bool bits[10], bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9) {
    bits[0] = b0;
    bits[1] = b1;
    bits[2] = b2;
    bits[3] = b3;
    bits[4] = b4;
    bits[5] = b5;
    bits[6] = b6;
    bits[7] = b7;
    bits[8] = b8;
    bits[9] = b9;

}

void set_8bits(bool bits[8], bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7) {
    bits[0] = b0;
    bits[1] = b1;
    bits[2] = b2;
    bits[3] = b3;
    bits[4] = b4;
    bits[5] = b5;
    bits[6] = b6;
    bits[7] = b7;
}

void set_4bits(bool bits[4], bool b0, bool b1, bool b2, bool b3) {
    bits[0] = b0;
    bits[1] = b1;
    bits[2] = b2;
    bits[3] = b3;
}

void jump_mux(bool condition, bool conditionRequired, bool switch_in, /*bool use_stack_output,*/ bool address[10], bool jmp_addr[10], /*bool stack_output[10],*/ bool output[10])
{
    bool switch_in_negated = 1 - switch_in;
    bool conditionNotRequired = 1 - conditionRequired;
    bool conditionNotMet = 1 - condition;
    /*bool use_stack_output_negated = 1 - use_stack_output;*/
    for(int i = 0; i < 10; i++)
        output[i] = switch_in_negated * address[i]
                  + conditionNotRequired * switch_in * jmp_addr[i]
                  + condition * conditionRequired * switch_in * jmp_addr[i]
                  + conditionNotMet * conditionRequired * switch_in * address[i];
    /*for(int i = 0; i < 10; i++)
        output[i] = output[i]*(use_stack_output_negated) + stack_output[i]*use_stack_output;*/
}   

void data_mux(bool switch_in, bool data[numBits], bool data2[numBits], bool output[numBits]) {
    bool switch_in_negated = 1-switch_in;
    for(int i = 0; i < numBits; i++)
        output[i] = 0 + switch_in_negated*data[i] + switch_in*data2[i];
}

void dest_mux(bool switch_in, bool address[4], bool address2[4], bool out_addr[4]){
    bool switch_in_negated = 1-switch_in;
    for(int i = 0; i < 4; i++)
        out_addr[i] = 0 + switch_in_negated*address[i] + switch_in*address2[i];
}

void control_rom(bool opcode[4], bool settings[numSettings]) {
    bool op[16];
    bool opcodeNegated[4];
    set_4bits(opcodeNegated, 1-opcode[0], 1-opcode[1], 1-opcode[2], 1-opcode[3]);
    op[0] = opcodeNegated[0]*opcodeNegated[1]*opcodeNegated[2]*opcodeNegated[3];
    op[1] = opcode[0]*opcodeNegated[1]*opcodeNegated[2]*opcodeNegated[3];
    op[2] = opcodeNegated[0]*opcode[1]*opcodeNegated[2]*opcodeNegated[3];
    op[3] = opcode[0]*opcode[1]*opcodeNegated[2]*opcodeNegated[3];
    op[4] = opcodeNegated[0]*opcodeNegated[1]*opcode[2]*opcodeNegated[3];
    op[5] = opcode[0]*opcodeNegated[1]*opcode[2]*opcodeNegated[3];
    op[6] = opcodeNegated[0]*opcode[1]*opcode[2]*opcodeNegated[3];
    op[7] = opcode[0]*opcode[1]*opcode[2]*opcodeNegated[3];
    op[8] = opcodeNegated[0]*opcodeNegated[1]*opcodeNegated[2]*opcode[3];;
    op[9] = opcode[0]*opcodeNegated[1]*opcodeNegated[2]*opcode[3];
    op[10] = opcodeNegated[0]*opcode[1]*opcodeNegated[2]*opcode[3];
    op[11] = opcode[0]*opcode[1]*opcodeNegated[2]*opcode[3];
    op[12] = opcodeNegated[0]*opcodeNegated[1]*opcode[2]*opcode[3];
    op[13] = opcode[0]*opcodeNegated[1]*opcode[2]*opcode[3];
    op[14] = opcodeNegated[0]*opcode[1]*opcode[2]*opcode[3];
    op[15] = opcode[0]*opcode[1]*opcode[2]*opcode[3];
    /*
    // ADD 2         
    alu(0, 0, 0, 0, 0, 0, 0);
    // SUB 3
    alu(1, 0, 1, 0, 0, 0, 0);
    // NOR 4
    alu(0, 0, 0, 1, 1, 0, 0);
    // AND 5
    alu(0, 1, 1, 1, 1, 0, 0);
    // XOR 6
    alu(0, 1, 0, 1, 0, 0, 0);
    // RSH 7
    alu(0, 0, 0, 0, 0, 0, 1);
    */
    bool    enable =  0 + op[2] + op[3] + op[4] + op[5] + op[6] + op[7] + op[8] + op[9] + op[14], 
            invertA = 0 + op[5] + op[6], 
            invertB = 0 + op[3] + op[5], 
            cin = 0 + op[3], 
            flood_carry = 0 + op[4] + op[5] + op[6], 
            xorToOr = 0 + op[4] + op[5], 
            invertS = 0, 
            rightShift = 0 + op[7],
            datamux = 0 + op[8],
            destmux = 0 + op[8] + op[9],
            halt = 0 + op[1],
            aluBmux = 0 + op[9],
            jumpmux = 0 + op[10] + op[11] + op[12] + op[13],
            setFlags = 0 + op[2] + op[3] + op[4] + op[5] + op[6] + op[7] + op[9],
            conditional_jump = 0 + op[11],
            enable_call_stack = 0 + op[12] + op[13],
            pop_or_push = 0 + op[12],
            enable_data_memory = op[14] + op[15],
            write_to_data_memory = op[15];
    
    settings[0] = enable;
    settings[1] = cin;
    settings[2] = invertA;
    settings[3] = invertB;
    settings[4] = flood_carry;
    settings[5] = xorToOr;
    settings[6] = invertS;
    settings[7] = rightShift;
    settings[8] = datamux;
    settings[9] = destmux;
    settings[10] = halt;
    settings[11] = aluBmux;
    settings[12] = jumpmux;
    settings[13] = setFlags;
    settings[14] = conditional_jump;
    settings[15] = enable_call_stack;
    settings[16] = pop_or_push;
    settings[17] = enable_data_memory;
    settings[18] = write_to_data_memory;
}

void init_instruction_memory(instruction_memory *instruction_memory) {
    for (int r = 0; r < 32; r++)
        for (int c = 0; c < 32; c++)
            for (int b = 0; b < 16; b++)
                instruction_memory->instructions[r][c][b] = 0;

    /*set_instruction(instruction_memory->instructions, 0, LDI R1, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 1, LDI R3, 1, 1, 1, 1, 1, 1, 1, 1);
    set_instruction(instruction_memory->instructions, 2, ADD R1, R1, R2);
    set_instruction(instruction_memory->instructions, 3, XOR R1, R1, R2);
    set_instruction(instruction_memory->instructions, 4, ADD R2, R3, R4);
    set_instruction(instruction_memory->instructions, 5, ADD R1, R3, R4);
    set_instruction(instruction_memory->instructions, 6, HLT);*/
    /*set_instruction(instruction_memory->instructions, 0, LDI R1, 1, 1, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 1, ADI R1, 1, 1, 1, 1, 1, 1, 1, 1);
    set_instruction(instruction_memory->instructions, 2, BRH 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 3, JMP 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 4, HLT);//*/
    
    set_instruction(instruction_memory->instructions, 0, LDI R1, 0, 1, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 1, LDI R2, 1, 1, 1, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 2, CAL 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 3, HLT);
    set_instruction(instruction_memory->instructions, 4, ADD R3, R2, R3);
    set_instruction(instruction_memory->instructions, 5, ADI R1, 1, 1, 1, 1, 1, 1, 1, 1);
    set_instruction(instruction_memory->instructions, 6, BRH 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 7, RET);
    set_instruction(instruction_memory->instructions, 8, HLT);/**/

    /*set_instruction(instruction_memory->instructions, 0, CAL 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 1, HLT);
    set_instruction(instruction_memory->instructions, 2, CAL 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 3, CAL 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 4, RET);
    set_instruction(instruction_memory->instructions, 5, ADI R1, 1, 0, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 6, RET);
    set_instruction(instruction_memory->instructions, 7, ADI R1, 0, 1, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 8, RET);*/

    /*
    set_instruction(instruction_memory->instructions, 0, LDI R1, 1, 1, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 1, LDI R2, 0, 1, 0, 0, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 2, LDI R3, 0, 0, 0, 1, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 3, STR R1, R2, 0, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 4, LOD R2, R1, 1, 0, 0, 0);
    set_instruction(instruction_memory->instructions, 5, HLT);*/

    //*/
    //set_instruction(instruction_memory->instructions, 5, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0);
    
    // set_instruction(instruction_memory->instructions, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
}

void init_program_counter(program_counter *program_counter)
{
    for(int i = 0; i < 10; i++)
        init_d_latch(&program_counter->registers[i]);
}

void increment_program_counter(program_counter *program_counter)
{
    bool A[10];
    bool B[10] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    read_from_program_counter(program_counter, A);
    
    bool P[10], G[10];
    for(int i = 0; i < 10; i++) {
        bool copyA = A[i];
        bool copyB = B[i];
        P[i] = copyA + copyB - 2*copyA*copyB;
        G[i] = copyA*copyB;
    }

    bool current_carry = 0;

    for(int i = 0; i < 10; i++) {
        A[i] = P[i] + current_carry - 2*P[i]*current_carry;
        current_carry = P[i] * current_carry + G[i];
    }
    write_to_program_counter(program_counter, 1, A);
}

void write_to_program_counter(program_counter *program_counter, bool enable, bool data[10])
{
    for (int i = 0; i < 10; i++) {
        update_d_latch(enable, data[i], &program_counter->registers[i]);
    }
}

void read_from_program_counter(program_counter *program_counter, bool output[10])
{
    for (int i = 0; i < 10; i++)
        output[i] = 0;
    for (int i = 0; i < 10; i++)
        output[i] += program_counter->registers[i].q;
}

void print_program_counter(program_counter *program_counter)
{
    printf("program counter: ");
    for (int i = 0; i < 10; i++)
        printf("%d", program_counter->registers[i].q);
    printf("\n");
}

void init_alu_flags(alu_flags *alu_flags)
{
    for (int i = 0; i < 2; i++)
        init_d_latch(&alu_flags->flags[i]);
}

void condition_flag_decoder(alu_flags *alu_flags, bool zero, bool carry, bool *condition)
{
    bool zero_flag          = (1-zero)*(1-carry);
    bool zero_flag_negated  = zero*(1-carry);
    bool carry_flag         = carry*(1-zero);
    bool carry_flag_negated = carry*zero;

    *condition = 
        alu_flags->flags[0].q*zero_flag + 
        alu_flags->flags[0].qbar*zero_flag_negated + 
        alu_flags->flags[1].q*carry_flag + 
        alu_flags->flags[1].qbar*carry_flag_negated;
}

void read_instruction_memory(instruction_memory *im, bool address[10], bool instruction[16]) {
    bool a0  = address[0],  a0n = 1 - address[0];
    bool a1  = address[1],  a1n = 1 - address[1];
    bool a2  = address[2],  a2n = 1 - address[2];
    bool a3  = address[3],  a3n = 1 - address[3];
    bool a4  = address[4],  a4n = 1 - address[4];
    bool a5  = address[5],  a5n = 1 - address[5];
    bool a6  = address[6],  a6n = 1 - address[6];
    bool a7  = address[7],  a7n = 1 - address[7];
    bool a8  = address[8],  a8n = 1 - address[8];
    bool a9  = address[9],  a9n = 1 - address[9];

    bool row[32];
    row[0]  = a5n * a6n * a7n * a8n * a9n;  // 00000
    row[1]  = a5   * a6n * a7n * a8n * a9n;  // 00001
    row[2]  = a5n * a6   * a7n * a8n * a9n;  // 00010
    row[3]  = a5   * a6   * a7n * a8n * a9n;  // 00011
    row[4]  = a5n * a6n * a7   * a8n * a9n;  // 00100
    row[5]  = a5   * a6n * a7   * a8n * a9n;  // 00101
    row[6]  = a5n * a6   * a7   * a8n * a9n;  // 00110
    row[7]  = a5   * a6   * a7   * a8n * a9n;  // 00111
    row[8]  = a5n * a6n * a7n * a8   * a9n;  // 01000
    row[9]  = a5   * a6n * a7n * a8   * a9n;  // 01001
    row[10] = a5n * a6   * a7n * a8   * a9n;  // 01010
    row[11] = a5   * a6   * a7n * a8   * a9n;  // 01011
    row[12] = a5n * a6n * a7   * a8   * a9n;  // 01100
    row[13] = a5   * a6n * a7   * a8   * a9n;  // 01101
    row[14] = a5n * a6   * a7   * a8   * a9n;  // 01110
    row[15] = a5   * a6   * a7   * a8   * a9n;  // 01111
    row[16] = a5n * a6n * a7n * a8n * a9;    // 10000
    row[17] = a5   * a6n * a7n * a8n * a9;    // 10001
    row[18] = a5n * a6   * a7n * a8n * a9;    // 10010
    row[19] = a5   * a6   * a7n * a8n * a9;    // 10011
    row[20] = a5n * a6n * a7   * a8n * a9;    // 10100
    row[21] = a5   * a6n * a7   * a8n * a9;    // 10101
    row[22] = a5n * a6   * a7   * a8n * a9;    // 10110
    row[23] = a5   * a6   * a7   * a8n * a9;    // 10111
    row[24] = a5n * a6n * a7n * a8   * a9;    // 11000
    row[25] = a5   * a6n * a7n * a8   * a9;    // 11001
    row[26] = a5n * a6   * a7n * a8   * a9;    // 11010
    row[27] = a5   * a6   * a7n * a8   * a9;    // 11011
    row[28] = a5n * a6n * a7   * a8   * a9;    // 11100
    row[29] = a5   * a6n * a7   * a8   * a9;    // 11101
    row[30] = a5n * a6   * a7   * a8   * a9;    // 11110
    row[31] = a5   * a6   * a7   * a8   * a9;    // 11111

    
    bool col[32];
    col[0]  = a0n * a1n * a2n * a3n * a4n;  // 00000
    col[1]  = a0   * a1n * a2n * a3n * a4n;  // 00001
    col[2]  = a0n * a1   * a2n * a3n * a4n;  // 00010
    col[3]  = a0   * a1   * a2n * a3n * a4n;  // 00011
    col[4]  = a0n * a1n * a2   * a3n * a4n;  // 00100
    col[5]  = a0   * a1n * a2   * a3n * a4n;  // 00101
    col[6]  = a0n * a1   * a2   * a3n * a4n;  // 00110
    col[7]  = a0   * a1   * a2   * a3n * a4n;  // 00111
    col[8]  = a0n * a1n * a2n * a3   * a4n;  // 01000
    col[9]  = a0   * a1n * a2n * a3   * a4n;  // 01001
    col[10] = a0n * a1   * a2n * a3   * a4n;  // 01010
    col[11] = a0   * a1   * a2n * a3   * a4n;  // 01011
    col[12] = a0n * a1n * a2   * a3   * a4n;  // 01100
    col[13] = a0   * a1n * a2   * a3   * a4n;  // 01101
    col[14] = a0n * a1   * a2   * a3   * a4n;  // 01110
    col[15] = a0   * a1   * a2   * a3   * a4n;  // 01111
    col[16] = a0n * a1n * a2n * a3n * a4;    // 10000
    col[17] = a0   * a1n * a2n * a3n * a4;    // 10001
    col[18] = a0n * a1   * a2n * a3n * a4;    // 10010
    col[19] = a0   * a1   * a2n * a3n * a4;    // 10011
    col[20] = a0n * a1n * a2   * a3n * a4;    // 10100
    col[21] = a0   * a1n * a2   * a3n * a4;    // 10101
    col[22] = a0n * a1   * a2   * a3n * a4;    // 10110
    col[23] = a0   * a1   * a2   * a3n * a4;    // 10111
    col[24] = a0n * a1n * a2n * a3   * a4;    // 11000
    col[25] = a0   * a1n * a2n * a3   * a4;    // 11001
    col[26] = a0n * a1   * a2n * a3   * a4;    // 11010
    col[27] = a0   * a1   * a2n * a3   * a4;    // 11011
    col[28] = a0n * a1n * a2   * a3   * a4;    // 11100
    col[29] = a0   * a1n * a2   * a3   * a4;    // 11101
    col[30] = a0n * a1   * a2   * a3   * a4;    // 11110
    col[31] = a0   * a1   * a2   * a3   * a4;    // 11111   

    for(int i = 0; i < 16; i++)
        instruction[i] = 0;
    for (int r = 0; r < 32; r++)
        for (int c = 0; c < 32; c++) {
            bool en = row[r] * col[c];
            for (int i = 0; i < 16; i++)
                instruction[i] += en * im->instructions[r][c][i];
        }
}   

void init_call_stack(call_stack* call_stack) {
    for(int i = 0; i < stack_size; i++)
        for(int a = 0; a < 10; a++)
            init_d_latch(&call_stack->registers[i][a]);
}

void print_call_stack(call_stack *call_stack)
{
    printf("stack: \n");
    for(int i = 0; i < stack_size; i++) {
        for(int a = 0; a < 10; a++)
            printf("%d", call_stack->registers[i][a].q);
        printf("\n");
    }
    printf("\n");
}

// 0 push, 1 pop
void use_call_stack(call_stack *call_stack, bool push_addr[10], bool push_or_pop, bool enable/*, bool out_addr[10]*/)
{
    /*printf("push_or_pop: %d\n", push_or_pop);
    printf("enable: %d\n", enable);*/
    bool execute_push = (push_or_pop) * enable;
    bool execute_pop  = (1-push_or_pop) * enable;
    /*printf("execute_push: %d\n", execute_push);
    printf("execute_pop: %d\n", execute_pop);*/

    
    for(int i = stack_size - 1; i > 0; i--) {
        for(int a = 0; a < 10; a++) {
            update_d_latch(execute_push, call_stack->registers[i-1][a].q, &call_stack->registers[i][a]);
        }
    }
    /*if(execute_push) {
            printf("push: ");
        }*/
    for(int a = 0; a < 10; a++) {
        /*if(execute_push) {
            printf("%d", push_addr[a]);
        }*/
        update_d_latch(execute_push, push_addr[a], &call_stack->registers[0][a]);
    }
    /*if(execute_push) {
            printf("\n");
        }*/

    for(int i = 0; i < stack_size - 1; i++) {
        for(int a = 0; a < 10; a++) {
            update_d_latch(execute_pop, call_stack->registers[i+1][a].q, &call_stack->registers[i][a]);
        }
    }
    for(int a = 0; a < 10; a++) {
        update_d_latch(execute_pop, 0, &call_stack->registers[stack_size-1][a]);
    }
}

void peek_call_stack(call_stack *call_stack, bool out_addr[10])
{
    for(int a = 0; a < 10; a++)
        out_addr[a] = call_stack->registers[0][a].q;
}

void init_data_memory(data_memory *data_memory)
{
    for (int r = 0; r < 16; r++)
        for (int c = 0; c < 16; c++)
            for (int b = 0; b < numBits; b++)
                update_d_latch(1, 0, &data_memory->data[r][c][b]);
}

void print_data_memory(data_memory *data_memory)
{
    for (int r = 0; r < 5; r++)
        for (int c = 0; c < 5; c++) 
        {
            printf("row: %d, col: %d, \tdata: ", r, c);
            for (int b = 0; b < numBits; b++)
                printf("%d", data_memory->data[r][c][b].q);
            printf("\n");
        }
}

void read_write_data_memory(data_memory *data_memory, bool address[8], bool data[numBits], bool readOrWrite, bool enable, bool output[numBits])
{
    bool a0  = address[0],  a0n = 1 - address[0];
    bool a1  = address[1],  a1n = 1 - address[1];
    bool a2  = address[2],  a2n = 1 - address[2];
    bool a3  = address[3],  a3n = 1 - address[3];
    bool a4  = address[4],  a4n = 1 - address[4];
    bool a5  = address[5],  a5n = 1 - address[5];
    bool a6  = address[6],  a6n = 1 - address[6];
    bool a7  = address[7],  a7n = 1 - address[7];

    bool row[16];
    row[0]  = a4n * a5n * a6n * a7n;  // 0000
    row[1]  = a4   * a5n * a6n * a7n;  // 0001
    row[2]  = a4n * a5   * a6n * a7n;  // 0010
    row[3]  = a4   * a5   * a6n * a7n;  // 0011
    row[4]  = a4n * a5n * a6   * a7n;  // 0100
    row[5]  = a4   * a5n * a6   * a7n;  // 0101
    row[6]  = a4n * a5   * a6   * a7n;  // 0110
    row[7]  = a4   * a5   * a6   * a7n;  // 0111
    row[8]  = a4n * a5n * a6n * a7;    // 1000
    row[9]  = a4   * a5n * a6n * a7;    // 1001
    row[10] = a4n * a5   * a6n * a7;    // 1010
    row[11] = a4   * a5   * a6n * a7;    // 1011
    row[12] = a4n * a5n * a6   * a7;    // 1100
    row[13] = a4   * a5n * a6   * a7;    // 1101
    row[14] = a4n * a5   * a6   * a7;    // 1110
    row[15] = a4   * a5   * a6   * a7;    // 1111

    bool col[16];
    col[0]  = a0n * a1n * a2n * a3n;  // 0000
    col[1]  = a0   * a1n * a2n * a3n;  // 0001
    col[2]  = a0n * a1   * a2n * a3n;  // 0010
    col[3]  = a0   * a1   * a2n * a3n;  // 0011
    col[4]  = a0n * a1n * a2   * a3n;  // 0100
    col[5]  = a0   * a1n * a2   * a3n;  // 0101
    col[6]  = a0n * a1   * a2   * a3n;  // 0110
    col[7]  = a0   * a1   * a2   * a3n;  // 0111
    col[8]  = a0n * a1n * a2n * a3;    // 1000
    col[9]  = a0   * a1n * a2n * a3;    // 1001
    col[10] = a0n * a1   * a2n * a3;    // 1010
    col[11] = a0   * a1   * a2n * a3;    // 1011
    col[12] = a0n * a1n * a2   * a3;    // 1100
    col[13] = a0   * a1n * a2   * a3;    // 1101
    col[14] = a0n * a1   * a2   * a3;    // 1110
    col[15] = a0   * a1   * a2   * a3;    // 1111   

    for(int i = 0; i < numBits; i++)
        output[i] = 0;
    for (int r = 0; r < 16; r++)
        for (int c = 0; c < 16; c++) {
            bool en = row[r] * col[c];
            for (int i = 0; i < numBits; i++)
                {
                    update_d_latch(en * readOrWrite * enable, data[i], &data_memory->data[r][c][i]);
                    output[i] += en * data_memory->data[r][c][i].q;
                }
        }
}

void print_instruction_address(bool address[10]) {
    printf("instruction address: ");
    for(int i = 0; i < 10; i++)
        printf("%d", address[i]);
    printf("\n");
}

void write_to_register(register_file* register_file, bool enable, bool address[4], bool data[numBits]) {
    bool en[15];
    bool address_negated[4];
    set_4bits(address_negated, 1 - address[0], 1 - address[1], 1 - address[2], 1 - address[3]);

    en[0]  =    address[0]  * address_negated[1] * address_negated[2] * address_negated[3]; // 0001 (Reg 1)
    en[1]  = address_negated[0] *    address[1]  * address_negated[2] * address_negated[3]; // 0010 (Reg 2)
    en[2]  =    address[0]  *    address[1]  * address_negated[2] * address_negated[3]; // 0011 (Reg 3)
    en[3]  = address_negated[0] * address_negated[1] *    address[2]  * address_negated[3]; // 0100 (Reg 4)
    en[4]  =    address[0]  * address_negated[1] *    address[2]  * address_negated[3]; // 0101 (Reg 5)
    en[5]  = address_negated[0] *    address[1]  *    address[2]  * address_negated[3]; // 0110 (Reg 6)
    en[6]  =    address[0]  *    address[1]  *    address[2]  * address_negated[3]; // 0111 (Reg 7)
    en[7]  = address_negated[0] * address_negated[1] * address_negated[2] *    address[3];  // 1000 (Reg 8)
    en[8]  =    address[0]  * address_negated[1] * address_negated[2] *    address[3];  // 1001 (Reg 9)
    en[9]  = address_negated[0] *    address[1]  * address_negated[2] *    address[3];  // 1010 (Reg 10)
    en[10] =    address[0]  *    address[1]  * address_negated[2] *    address[3];  // 1011 (Reg 11)
    en[11] = address_negated[0] * address_negated[1] *    address[2]  *    address[3];  // 1100 (Reg 12)
    en[12] =    address[0]  * address_negated[1] *    address[2]  *    address[3];  // 1101 (Reg 13)
    en[13] = address_negated[0] *    address[1]  *    address[2]  *    address[3];  // 1110 (Reg 14)
    en[14] =    address[0]  *    address[1]  *    address[2]  *    address[3];  // 1111 (Reg 15)

    for (int r = 0; r < 15; r++) {
        for (int i = 0; i < numBits; i++) {
            update_d_latch(en[r]*enable, data[i], &register_file->registers[r][i]);
        }
    }
}

void read_from_register(register_file* register_file, bool address[4], bool output[numBits]) {
    bool en[15];
    bool address_negated[4];
    set_4bits(address_negated, 1 - address[0], 1 - address[1], 1 - address[2], 1 - address[3]);

    en[0]  =    address[0]  * address_negated[1] * address_negated[2] * address_negated[3]; // 0001 (Reg 1)
    en[1]  = address_negated[0] *    address[1]  * address_negated[2] * address_negated[3]; // 0010 (Reg 2)
    en[2]  =    address[0]  *    address[1]  * address_negated[2] * address_negated[3]; // 0011 (Reg 3)
    en[3]  = address_negated[0] * address_negated[1] *    address[2]  * address_negated[3]; // 0100 (Reg 4)
    en[4]  =    address[0]  * address_negated[1] *    address[2]  * address_negated[3]; // 0101 (Reg 5)
    en[5]  = address_negated[0] *    address[1]  *    address[2]  * address_negated[3]; // 0110 (Reg 6)
    en[6]  =    address[0]  *    address[1]  *    address[2]  * address_negated[3]; // 0111 (Reg 7)
    en[7]  = address_negated[0] * address_negated[1] * address_negated[2] *    address[3];  // 1000 (Reg 8)
    en[8]  =    address[0]  * address_negated[1] * address_negated[2] *    address[3];  // 1001 (Reg 9)
    en[9]  = address_negated[0] *    address[1]  * address_negated[2] *    address[3];  // 1010 (Reg 10)
    en[10] =    address[0]  *    address[1]  * address_negated[2] *    address[3];  // 1011 (Reg 11)
    en[11] = address_negated[0] * address_negated[1] *    address[2]  *    address[3];  // 1100 (Reg 12)
    en[12] =    address[0]  * address_negated[1] *    address[2]  *    address[3];  // 1101 (Reg 13)
    en[13] = address_negated[0] *    address[1]  *    address[2]  *    address[3];  // 1110 (Reg 14)
    en[14] =    address[0]  *    address[1]  *    address[2]  *    address[3];  // 1111 (Reg 15)

    for (int i = 0; i < numBits; i++)
        output[i] = 0;
    for (int r = 0; r < 15; r++) {
        for (int i = 0; i < numBits; i++) {
            output[i] += en[r] * register_file->registers[r][i].q;
        }
    }
}

void print_register_file(register_file* register_file) {
    for (int r = 0; r < 3; r++) {
        printf("r%2d: ", r+1);
        for (int i = 0; i < numBits; i++)
            printf("%d", register_file->registers[r][i].q);
        printf("\n");
    }
}