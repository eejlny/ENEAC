/*
(c) Copyright 2013 - 2016 Xilinx, Inc. All rights reserved. 

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER 
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law: (1) THESE
MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX HEREBY
DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising
under or in connection with these materials, including for any direct, or any
indirect, special, incidental, or consequential loss or damage (including loss
of data, profits, goodwill, or any type of loss or damage suffered as a result
of any action brought by a third party) even if such damage or loss was
reasonably foreseeable or Xilinx had been advised of the possibility of the
same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES. 
*/

#include <stdio.h>
#include <cstdlib>
#ifndef __SDSVHLS__
#include "uio_axi_gpio.h"
#endif
#include "arraycopy.h"
#include "sds_lib.h"

#ifdef DEBUG
#include <stdio.h>
extern void print_int_array(int*);
#endif

void arraycopy(uint32 a[N], uint32 b[N])
{
  for (int i = 0; i < N; i++)
    b[i] = a[i];
}

void arraycopy_sw(uint32 a[N], uint32 b[N])
{
#ifdef DEBUG
  printf("Entering arraycopy_sw:\n");
  printf("a:\n");
  print_int_array((int*)a);
#endif

  for (int i = 0; i < N; i++)
    b[i] = a[i];

#ifdef DEBUG
  printf("Exiting arraycopy_sw:\n");
#endif
}

int compare_uint32_arrays(uint32 *A, uint32 *B)
{
     for (int i = 0; i < N; i++) {
          if (A[i] != B[i]) {
               printf("A[%d] = %x, B[%d] = %x do not match\n", i, A[i], i, B[i]);
               return -1;
          }
     }
     return 0;
}

int main(int argc, char **argv)
{
#ifndef __SDSVHLS__
	axi_gpio axi_gpio_0;
	int status = axi_gpio_init(&axi_gpio_0, "gpio");
	if (status < 0) {
		perror("axi_gpio_init");
		return -1;
	}
#endif

        printf("Allocating arrays\n");
        uint32 *A, *B;
        A = (uint32 *)sds_alloc(NUMBYTES);
        B = (uint32 *)sds_alloc(NUMBYTES);
        
        if (A == NULL) {
             printf("Could not allocate memory A - exiting\n");
        }
        else if (B == NULL) {
             printf("Could not allocate memory B - exiting\n");
        }
        else {
             bool success = true;
             printf("LEDs match iteration index...\n");
             for (int j = 0; j < 64; j++) {
#ifndef __SDSVHLS__
                  axi_gpio_write_reg(axi_gpio_0.baseaddr, 0, (unsigned char) j);
#endif
                  for (int i = N-1; i >= 0; i-- ) {
                       A[i] = rand() % N ;
                       B[i] = 0;
                  }
                  
                  printf("A: ");
                  for (int i = 0; i < N; i++)
                       printf("%5d", A[i]);
                  printf("\n");
                  
                  arraycopy(A, B);
                  
                  if (compare_uint32_arrays(A, B)) {
                       success = false;
                       break;
                  }
                  printf("Iteration %2d matched.  Update LEDs and spin CPU to make change visible.\n",  j);
                  for (int i=0; i < 50000000; i++);
             }
             printf("Freeing array\n");
             sds_free(A);
             printf("%s\n", success ? "Test passed!" : "Better luck next time...");
#ifndef __SDSVHLS__
             axi_gpio_release(&axi_gpio_0);
#endif
        } 
	return 0;
}

// XSIP watermark, do not delete 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
