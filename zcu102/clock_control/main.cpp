/*Copyright (c) 2018, Jose Nunez-Yanez*/
/*University of Bristol. ENEAC project*/

#include "aes_enc.h"
#include "kernelAES.h"
#include <fcntl.h>
#include <sys/mman.h>
//#include <asm/cachectl.h>


#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)



uint8_t key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};  // initial key

#define TIME_STAMP_INIT_SW  unsigned long long clock_start, clock_end;  clock_start = sds_clock_counter();
#define TIME_STAMP_SW  { clock_end = sds_clock_counter(); printf("CPU ON: Average number of processor cycles : %llu \n", (clock_end-clock_start)); clock_start = sds_clock_counter();  }


#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW  { clock_end_hw = sds_clock_counter(); printf("FPGA ON: Average number of processor cycles : %llu \n", (clock_end_hw-clock_start_hw)); clock_start_hw = sds_clock_counter();  }


#define TIME_STAMP_INIT_HW2  unsigned long long clock_start_hw2, clock_end_hw2;  clock_start_hw2 = sds_clock_counter();
#define TIME_STAMP_HW2  { clock_end_hw2 = sds_clock_counter(); printf("FPGA ON: Average number of processor cycles : %llu \n", (clock_end_hw2-clock_start_hw2)); clock_start_hw = sds_clock_counter();  }


#define TIME_STAMP_INIT_V8  unsigned long long clock_start_v8, clock_end_v8;  clock_start_v8 = sds_clock_counter();
#define TIME_STAMP_V8  { clock_end_v8 = sds_clock_counter(); printf("CPU V8 ON: Average number of processor cycles : %llu \n", (clock_end_v8-clock_start_v8)); clock_start_v8 = sds_clock_counter();  }

#define standalone 1

void read_input(uint8_t *state, char *file, unsigned int& block_size)
{

	#ifdef standalone
		for(int i=0; i< block_size; i++)
		{
			*(state+i) = i;
		}
		printf("Loaded generated data\n");
	#else
	   FILE *fp;

	   fp = fopen (file, "r");
	   if (!fp)
	   {
		  printf("file could not be opened for reading\n");
		   exit(1);
	   }
	   fseek(fp, 0L, SEEK_END);
	   block_size = ftell(fp);
	   fread(state, block_size, 1, fp); // Read in the entire block
	   fclose(fp);
   #endif
}

void write_output(uint8_t *state, char *file, unsigned int block_size)
{
	#ifdef standalone
	#else
	 FILE *fp;

	 fp = fopen (file, "w");
	 if (!fp)
	 {
 		printf("file could not be opened for writing\n");
		exit(1);
	 }
	 fwrite(state, block_size, 1, fp); // Read in the entire block
	 fclose(fp);
	#endif
}

void keyexpansion(uint8_t key[32], uint8_t ekey[240]);

int main(int argc, char* argv[])
{

	int  value_clkps;
	int  value_clkpl;
	int  value_clkwp;

  	//open clkwp write protect register

         int memfd_clkwp;
         void *mapped_base_clkwp, *mapped_dev_base_clkwp;
         off_t dev_base_clkwp = 0xFD1A001C;

          memfd_clkwp = open("/dev/mem", O_RDWR | O_SYNC);
          if (memfd_clkwp == -1) {
               	printf("Can't open /dev/mem clkwp.\n");
                   exit(0);
             }
             printf("/dev/mem opened for clkwp.\n");

             // Map one page of memory into user space such that the device is in that page, but it may not
                     // be at the start of the page.


           mapped_base_clkwp = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd_clkwp, dev_base_clkwp & ~MAP_MASK);
           if (mapped_base_clkwp == (void *) -1) {
                   printf("Can't map the memory to user space clkwp.\n");
                   exit(0);
             }
             printf("CLKWP mapped at address %p.\n", mapped_base_clkwp);



             // get the address of the device in user space which will be an offset from the base
             // that was mapped as memory is mapped at the start of a page

         mapped_dev_base_clkwp = mapped_base_clkwp + (dev_base_clkwp & MAP_MASK);


	 value_clkwp = *((volatile unsigned int *) (mapped_dev_base_clkwp));

         printf("CLKWP value is %x \n",value_clkwp);

	 

         //open clkpl

         int memfd_clkpl;
         void *mapped_base_clkpl, *mapped_dev_base_clkpl;
         off_t dev_base_clkpl = 0xFF5E00C0;

          memfd_clkpl = open("/dev/mem", O_RDWR | O_SYNC);
          if (memfd_clkpl == -1) {
               	printf("Can't open /dev/mem clkpl.\n");
                   exit(0);
             }
             printf("/dev/mem opened for clkpl.\n");

             // Map one page of memory into user space such that the device is in that page, but it may not
                     // be at the start of the page.


           mapped_base_clkpl = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd_clkpl, dev_base_clkpl & ~MAP_MASK);
           if (mapped_base_clkpl == (void *) -1) {
                   printf("Can't map the memory to user space clkpl.\n");
                   exit(0);
             }
             printf("CLKPL mapped at address %p.\n", mapped_base_clkpl);



             // get the address of the device in user space which will be an offset from the base
             // that was mapped as memory is mapped at the start of a page

         mapped_dev_base_clkpl = mapped_base_clkpl + (dev_base_clkpl & MAP_MASK);


	 value_clkpl = *((volatile unsigned int *) (mapped_dev_base_clkpl));

         printf("CLKPL value is %x \n",value_clkpl);

	 //*((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1011E00; //100MHz div1 21:16 div2 13:8
	 //*((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1011400; //150MHz
	 //*((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1010F00; //200MHz
	 //*((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1031900; //40MHz 
	 //*((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1013C00; //50MHz
 	 *((volatile unsigned int *) (mapped_dev_base_clkpl)) = 0x1023C00; //25MHz


	//0x1031900 


	 value_clkpl = *((volatile unsigned int *) (mapped_dev_base_clkpl));

         printf("CLKPL value is %x \n",value_clkpl);





         //open clkps

         int memfd_clkps;
         void *mapped_base_clkps, *mapped_dev_base_clkps;
         off_t dev_base_clkps = 0xFD1A0060;

          memfd_clkps = open("/dev/mem", O_RDWR | O_SYNC);
          if (memfd_clkps == -1) {
               	printf("Can't open /dev/mem clkps.\n");
                   exit(0);
             }
             printf("/dev/mem opened for clkps.\n");

           // Map one page of memory into user space such that the device is in that page, but it may not
           // be at the start of the page.


           mapped_base_clkps = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd_clkps, dev_base_clkps & ~MAP_MASK);
           if (mapped_base_clkps == (void *) -1) {
                   printf("Can't map the memory to user space clkps.\n");
                   exit(0);
             }
             printf("CLKPS mapped at address %p.\n", mapped_base_clkps);


             // get the address of the device in user space which will be an offset from the base
             // that was mapped as memory is mapped at the start of a page

         mapped_dev_base_clkps = mapped_base_clkps + (dev_base_clkps & MAP_MASK);

	 value_clkps = *((volatile unsigned int *) (mapped_dev_base_clkps));

         printf("CLKPS value is %x \n",value_clkps);

	*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000100; // 1200 MHz clock divider from bits 8 to 13
         //*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000200; // 600 MHz clock divider from bits 8 to 13
	//*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000300; // 400 MHz clock divider from bits 8 to 13
	//*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000400; // 300 MHz clock divider from bits 8 to 13
	//*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000600; // 200 MHz clock divider from bits 8 to 13
	//*((volatile unsigned int *) (mapped_dev_base_clkps)) = 0x03000C00; // 100 MHz clock divider from bits 8 to 13




	 value_clkps = *((volatile unsigned int *) (mapped_dev_base_clkps));

         printf("CLKPS value is %x \n",value_clkps);




        // done clock mapping




	uint32_t x,y,i;
	
	unsigned int block_size=1000000;
	uint8_t *state;
	uint8_t *cipher;
	uint8_t *cipher_sw;

	char *ifile, *ofile;

	printf("Launching AES\n");

    #ifdef standalone
	#else
		if (argc != 3)
		{
			printf("incorrect number of inputs: aes in_file out_file\n");
			exit(1);
		}
		ifile = argv[1];
		ofile = argv[2];

    #endif




	uint8_t *ekey;
    	state = (uint8_t *) sds_alloc(block_size * sizeof(uint8_t));
    	if(!state)
   	{
   			printf("could not allocate state memory\n");
   			exit(0);
   	}
	ekey = (uint8_t *) sds_alloc(240 *sizeof(uint8_t));
	cipher = (uint8_t *) sds_alloc(block_size * sizeof(uint8_t));
	if(!cipher)
	{
		printf("could not allocate cipher memory\n");
		exit(0);
	}

	cipher_sw = (uint8_t *)  sds_alloc(block_size * sizeof(uint8_t));
	if(!cipher_sw)
	{
		printf("could not allocate cipher memory\n");
		exit(0);
	}


	keyexpansion(key,ekey);

	read_input(state, ifile,block_size);

	printf("block_size is %d\n",block_size);

	TIME_STAMP_INIT_HW
	int block_count = block_size/16;
	int new_block_size = block_count*16;
	int begin = 0;
	int end = new_block_size;
	printf("new_block_size is %d\n",new_block_size);

    	kernelAES1((uint8_t*)state,(uint8_t*)cipher,ekey,begin,end);
	TIME_STAMP_HW


        //int cacheflush((uint8_t*)cipher, block_size*sizeof(uint8_t), DCACHE);
	printf("Computation finished\n");

	#ifdef standalone
	#else
		write_output(cipher, ofile,block_size);
	#endif

	for(x=0;x<32;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");

	for(x=33;x<64;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");


	for(x=200000;x<200032;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");


	for(x=900000;x<900032;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");

	for(x=0;x<32;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");



	read_input(state, ifile,block_size);


	TIME_STAMP_INIT_V8
	aes_enc_v8(key,ekey,nr,(uint8_t*)state,(uint8_t*)cipher_sw,block_size);
	TIME_STAMP_V8

	for(x=0;x<32;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");

	for(x=33;x<64;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");


	for(x=200000;x<200032;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");



	for(x=900000;x<900032;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");

	


	read_input(state, ifile,block_size);


	TIME_STAMP_INIT_SW
	aes_enc_sw((uint8_t*)state,(uint8_t*)cipher_sw,ekey,block_size);
	TIME_STAMP_SW


	for(x=0;x<32;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");

	for(x=33;x<64;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");


	for(x=200000;x<200032;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");



	for(x=900000;x<900032;x++){
			printf(" %x", (uint8_t)cipher_sw[x]);
	}
	printf("\n");



	read_input(state, ifile,block_size);

	printf("block_size is %d\n",block_size);




	TIME_STAMP_INIT_HW2
	
    	kernelAES1((uint8_t*)state,(uint8_t*)cipher,ekey,begin,end);
	TIME_STAMP_HW2

	printf("Computation finished\n");

	#ifdef standalone
	#else
		write_output(cipher, ofile,block_size);
	#endif

	for(x=0;x<32;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");

	for(x=33;x<64;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");


	for(x=200000;x<200032;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");


	for(x=900000;x<900032;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");

	



	
	




	exit(0);
}

void keyexpansion(uint8_t key[32], uint8_t ekey[240])
{
	  uint32_t i, j, k;
	  uint8_t temp[4];

	  for(i = 0; i < nk; ++i)
	  {
	    ekey[(i * 4) + 0] = key[(i * 4) + 0];
	    ekey[(i * 4) + 1] = key[(i * 4) + 1];
	    ekey[(i * 4) + 2] = key[(i * 4) + 2];
	    ekey[(i * 4) + 3] = key[(i * 4) + 3];
	  }


	  for(; (i < (nb * (nr + 1))); ++i)
	  {
	    for(j = 0; j < 4; ++j)
	    {
	      temp[j]= ekey[(i-1) * 4 + j];
	    }
	    if (i % nk == 0)
	    {
	      {
	        k = temp[0];
	        temp[0] = temp[1];
	        temp[1] = temp[2];
	        temp[2] = temp[3];
	        temp[3] = k;
	      }


	      {
	        temp[0] = sbox[temp[0]];
	        temp[1] = sbox[temp[1]];
	        temp[2] = sbox[temp[2]];
	        temp[3] = sbox[temp[3]];
	      }

	      temp[0] =  temp[0] ^ Rcon[i/nk];
	    }
	    else if (nk > 6 && i % nk == 4)
	    {
	      // Function Subword()
	      {
	        temp[0] = sbox[temp[0]];
	        temp[1] = sbox[temp[1]];
	        temp[2] = sbox[temp[2]];
	        temp[3] = sbox[temp[3]];
	      }
	    }
	    ekey[i * 4 + 0] = ekey[(i - nk) * 4 + 0] ^ temp[0];
	    ekey[i * 4 + 1] = ekey[(i - nk) * 4 + 1] ^ temp[1];
	    ekey[i * 4 + 2] = ekey[(i - nk) * 4 + 2] ^ temp[2];
	    ekey[i * 4 + 3] = ekey[(i - nk) * 4 + 3] ^ temp[3];
	  }

}

