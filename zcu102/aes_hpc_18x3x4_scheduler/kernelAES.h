#ifndef KERNELAES_H_
#define KERNELAES_H_

void kernelAES1(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)

void kernelAES2(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)


void kernelAES3(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)


void kernelAES4(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)


#endif 
