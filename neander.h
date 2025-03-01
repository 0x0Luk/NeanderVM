#ifndef NEANDER_H
#define NEANDER_H

#include <stdbool.h>
#include <stdint.h>

#define MEM_SIZE 516

// Conjunto de instruções
#define STORE 0x10
#define LOAD 0x20
#define SUM 0x30
#define INVERT 0x60
#define GOTO 0x80
#define IFNEG 0x90
#define IFZERO 0xA0
#define STOP 0xF0

// Estrutura CPU Neander
typedef struct {
    uint8_t accumulator;       // Acumulador (ACC)
    uint8_t instruction_pointer; // Ponteiro de instrução (IP)
    bool zero_flag;            // Flag ZERO
    bool negative_flag;        // Flag NEG
    uint8_t memory[MEM_SIZE];  // Memória principal
} Neander;

void initialize_cpu(Neander *cpu, const char *filepath);
void run_cpu(Neander *cpu);
void show_memory(const Neander *cpu);

#endif 