#ifndef KERNELAES_H_
#define KERNELAES_H_

void kernelAES1(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end
//unsigned int core_count
);

void kernelAES2(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end
//unsigned int core_count
);

void kernelAES3(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end
//unsigned int core_count
);

void kernelAES4(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end
//unsigned int core_count
);




#endif 
