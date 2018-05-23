#ifndef KERNELAES_H_
#define KERNELAES_H_
void kernelAES(
uint32_t *state,
uint32_t *cipher,
uint8_t ekey[240],
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end
//unsigned int core_count
);

#endif 
