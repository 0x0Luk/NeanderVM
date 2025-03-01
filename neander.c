#include "neander.h"
#include <stdio.h>
#include <stdlib.h>

// Função para inicializar a CPU
void initialize_cpu(Neander *cpu, const char *filepath) {
    cpu->accumulator = 0; // ACC
    cpu->instruction_pointer = 0; // IP
    cpu->zero_flag = false; // ZERO
    cpu->negative_flag = false; // NEG

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fread(cpu->memory, 1, MEM_SIZE, file);
    fclose(file);
}

// Função para executar as instruções na CPU
void run_cpu(Neander *cpu) {
    while (cpu->memory[cpu->instruction_pointer] != STOP) {
        uint8_t operand = cpu->memory[cpu->instruction_pointer + 2];
        uint8_t address = operand * 2 + 4;

        switch (cpu->memory[cpu->instruction_pointer]) {
            case STORE:
                cpu->memory[address] = cpu->accumulator;
                break;
            case LOAD:
                cpu->accumulator = cpu->memory[address];
                break;
            case SUM:
                cpu->accumulator += cpu->memory[address];
                break;
            case INVERT:
                cpu->accumulator = ~cpu->accumulator;
                cpu->instruction_pointer += 2;
                continue;
            case GOTO:
                cpu->instruction_pointer = address;
                continue;
            case IFNEG:
                if (cpu->negative_flag) {
                    cpu->instruction_pointer = address;
                    continue;
                }
                break;
            case IFZERO:
                if (cpu->zero_flag) {
                    cpu->instruction_pointer = address;
                    continue;
                }
                break;
            default:
                break;
        }

        // Atualiza as flags de estado
        cpu->zero_flag = (cpu->accumulator == 0);
        cpu->negative_flag = (cpu->accumulator & 0x80) != 0;

        cpu->instruction_pointer += 4;
    }
}

// Função para exibir o estado da memória e da CPU
void show_memory(const Neander *cpu) {
    printf("\n\nACC: %03d  | IP: %03d\n", cpu->accumulator, cpu->instruction_pointer);
    printf("ZERO: %s | NEG: %s\n", cpu->zero_flag ? "true" : "false", cpu->negative_flag ? "true" : "false");

    for (size_t offset = 0; offset < MEM_SIZE; offset += 16) {
        printf("%08zx: ", offset);
        for (size_t i = 0; i < 16; i++) {
            if (offset + i < MEM_SIZE)
                printf("%02x ", cpu->memory[offset + i]);
            else
                printf("   ");
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_binario>\n", argv[0]);
        return 1;
    }

    Neander cpu;
    initialize_cpu(&cpu, argv[1]); 
    show_memory(&cpu); 
    run_cpu(&cpu); 
    show_memory(&cpu); 

    return 0;
}