#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Definição dos opcodes
#define STORE 0x10
#define LOAD 0x20
#define SUM 0x30
#define INVERT 0x60
#define GOTO 0x80
#define IFNEG 0x90
#define IFZERO 0xA0
#define STOP 0xF0

// Função para exibir o estado da memória e do processador
void show_memory(uint8_t *mem, size_t size, uint8_t accumulator, uint8_t ip, bool is_zero, bool is_negative) {
  size_t pos = 0;

  printf("ACC: %03d  | IP: %03d\n", accumulator, ip);
  printf("ZERO: %s | NEG: %s\n", is_zero ? "true" : "false", is_negative ? "true" : "false");

  while (pos < size) {
    printf("%08zx: ", pos);

    for (size_t i = 0; i < 16; i++) {
      if (pos + i < size)
        printf("%02x ", mem[pos + i]);
      else
        printf("   ");
    }

    printf("\n");
    pos += 16;
  }

  printf("\n\n");
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Uso: %s <arquivo_binario>\n", argv[0]);
    return 1;
  }

  uint8_t accumulator = 0, instruction_pointer = 0;
  bool zero_flag = false, negative_flag = false;

  const char *file_name = argv[1];
  FILE *binary_file = fopen(file_name, "rb");
  if (!binary_file) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }

  uint8_t memory[516]; // Memória principal
  uint8_t header[4];   // Cabeçalho do arquivo binário

  fread(header, 1, 4, binary_file);
  fread(memory + 4, 1, 512, binary_file);
  fclose(binary_file);

  show_memory(memory, 516, accumulator, instruction_pointer, zero_flag, negative_flag);

  while (memory[instruction_pointer] != STOP) {
    // Atualiza as flags de estado
    zero_flag = (accumulator == 0);
    negative_flag = (accumulator & 0x80) != 0;

    switch (memory[instruction_pointer]) {
      case 0x00: // NOP (No Operation)
        break;
      case STORE:
        memory[memory[instruction_pointer + 2] * 2 + 4] = accumulator;
        break;
      case LOAD:
        accumulator = memory[memory[instruction_pointer + 2] * 2 + 4];
        break;
      case SUM:
        accumulator += memory[memory[instruction_pointer + 2] * 2 + 4];
        break;
      case 0x40: // OR
        accumulator |= memory[memory[instruction_pointer + 2] * 2 + 4];
        break;
      case 0x50: // AND
        accumulator &= memory[memory[instruction_pointer + 2] * 2 + 4];
        break;
      case INVERT:
        accumulator = ~accumulator;
        instruction_pointer += 2;
        continue;
      case GOTO:
        instruction_pointer = memory[instruction_pointer + 2] * 2 + 4;
        continue;
      case IFNEG:
        if (negative_flag) {
          instruction_pointer = memory[instruction_pointer + 2] * 2 + 4;
          continue;
        }
        break;
      case IFZERO:
        if (zero_flag) {
          instruction_pointer = memory[instruction_pointer + 2] * 2 + 4;
          continue;
        }
        break;
    }

    instruction_pointer += 4; // Avança para a próxima instrução
    show_memory(memory, 516, accumulator, instruction_pointer, zero_flag, negative_flag);
  }

  show_memory(memory, 516, accumulator, instruction_pointer, zero_flag, negative_flag);

  return 0;
}