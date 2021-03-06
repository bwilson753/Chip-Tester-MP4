/******************************************************************
*
*   file:   chiptest.c
*   author: Ethan Bolker
*   date:   October, 1991
*   edited by Betty O'Neil, Apr, 1992, Apr, 1996, Nov, 1996
*
*   Chip testing program stub
*
*   Link with file containing the following global data structures 
*   describing the particular chip to be tested
*
*/
 
#include <stdio.h>
#include "chip.h"
#ifdef SAPC
#include <lp.h>
#include <cpu.h>
#include <serial.h>
#endif

#define MAXBUF 200
/* final program should have INTERACTIVE defined, but
 * you may want to comment this out while debugging
 * to get rid of the user input.
 */
#define INTERACTIVE
/* non-interactive values-- */
#define DEFAULT_CHIPNUM 0

/* These arrays specify the logical order of LPT input and output pins */
static int PDR_pins[] = {2, 3, 4, 5, 6, 7, 8, 9};/* chip inputs, LPT output */
static int PSR_pins[] = {15, 13, 12, 10, 11}; /* chip outputs, LPT input */

int get_int_in_range(int, int);
void getline(char *buf, int maxbuf);
void chiptest(Chip *chip);

/* add to these-- */
extern Chip ls00;
extern Chip ls138;
Chip *chips[]={&ls00, &ls138};
#define NCHIPS (sizeof(chips)/sizeof(chips[0]))

int main(void)
{
  int i, chipnum = DEFAULT_CHIPNUM;

  printf("Chip tester for chip(s):\n");
  for (i=0;i<NCHIPS;i++) 
    printf("%d--  %s\n",i,chips[i]->chipname);
#ifdef INTERACTIVE
  printf("\nEnter chip to test: ");
  chipnum = get_int_in_range(0,NCHIPS-1);
#endif
  chiptest(chips[chipnum]);
  return 0;
}

/* This needs filling out.  This code should work for any combinational
   chip with properly setup Chip data (unless it has too many input or
   output pins.)  Thus it should not do special-case code for LS138
   vs. LS00, for example.  On the other hand, it does need to know 
   about the LPT1 port, for ex., that outpins bit 0 needs to be read
   from PSR bit 3, and one of these PSR bits needs to be inverted 
   to get the chip's signal.
*/
/*****************************************/
/* This part is written to give the user */
/* pin connection instructions that are  */
/* derived from the Chip info only.      */
/* Note that some chips are already      */
/* connected up to mtip systems for you. */
/* See chiptest.txt for more information. */
/*****************************************/
void chiptest(Chip *chip)
{
  int i;
  int chip_input, chip_output, correct_output;
  int bit_mask; /*added*/

  printf("Testing %s chip (%s)\n\n", chip->chipname,chip->chipdesc);
  printf("Connection requirements:\n\n");

  printf("1. Connect the following pins to VCC:\n");
  for (i=0; i<chip->n_highpins; i++) {
    printf("\tpin #%d\n", chip->highpins[i]);
  }
  printf("\n");

  printf("2. Connect the following pins to GND:\n");
  /* you fill in */
  for (i = 0; i<chip->n_lowpins; i++) {
    printf("\tpin #%d\n", chip->lowpins[i]);
  }
  printf("\n");

  printf("3. Make the following LPT to %s connections:\n\n", chip->chipname);
  printf("\t###(Chip_Input pins)###\n");
  printf("\tLPT pin:\t%s pin:\n", chip->chipname);
  for (i=0; i<chip->n_inpins; i++)
	printf ("\t  %d\t\t  %d\n", PDR_pins[i], chip->inpins[i]);

  printf("\t###(Output pins)###\n");
  /* you fill in */
   printf("\tLPT pin:\t%s pin:\n", chip->chipname);
   for (i=0; i<chip->n_outpins; i++)
         printf ("\t  %d\t\t  %d\n", PSR_pins[i], chip->outpins[i]);


#ifdef INTERACTIVE  
  printf("\n<CR> when ready: ");
  while (getchar()!='\n')
    ;
#endif

  /* generate all bit patterns by counting up to the right power of 2 */
  for (chip_input = 0; chip_input < (1 << chip->n_inpins); chip_input++) {
    /* you fill in */
    /* First use put chip_input value out LPT1, read back chip's result. */
   static int alter1, alter2;
   /*chip_input = 0xc3;*/

   /*THIS IS THE ACTUAL INPUT AND OUTPUT*/
   /*set the 3 leftmost bits to 001*/
   /*first set them to 000*/
   /*chip_input = chip_input & 0x1f;*/
   /*now set them to 001*/
   /*chip_input = chip_input & 0x3f;*/
   /*printf("modified input:  %x", chip_input);*/

   outpt(LPT1_BASE + LP_DATA, chip_input);
   chip_output = inpt(LPT1_BASE + LP_STATUS);
   /*printf("chip input:  %x\n", chip_input);*/ 
   /*printf("unfiltered result = %x\n", chip_output);*/

   /*TEST*/
   /*chip_output = 0x7f;
   printf("test value:  %x\n", chip_output);*/

   /*BEGIN MDOIFIED OUTPUT*/

   /*invert busy  bit 7*/
   alter1 = 0x80;
   alter2 = chip_output & alter1;
   /*printf("alter2: %x\n", alter2);*/

   /*this inverts the 7 bit*/
   if (alter2 != 0) {
     /*printf("before and:  %x\n", chip_output);*/
   	chip_output = chip_output & 0x7f;
  	 /*0111 1111*/
        /*printf("anded:  %x\n", chip_output);*/
    }
   else {
   	chip_output = chip_output | 0x80;
  	 /*1000 0000*/
        /*printf("ored:  %x\n", chip_output);*/
   }

   /*printf("inverted 7 bit:  %x\n", chip_output);*/

  /*shift the bits to the right by 3*/
  chip_output = chip_output >> 3;
  /*printf("shifted 3:  %x\n", chip_output);*/

  /*turn on appropriate bit amount (4 for ls00, 5 for ls138*/
  bit_mask = 2; /*assumes at least 1 outpin*/
  for (i = 1; i < chip->n_outpins; i++) {
	bit_mask = bit_mask * 2;
  }  
  bit_mask = bit_mask - 1; /*bit mask value equals out pins*/ 
  chip_output = chip_output & bit_mask;  
  /*printf("bits mask is on:  %x\n", chip_output);*/

  /*END MODIFIED OUTPUT*/

   /*BEGIN EXPECTED OUTPUT*/
   chip->softchip(chip_input, &correct_output);
   /*printf("chip_input = %x\n", chip_input);*/ 
   /*printf("correct output = %x\n", correct_output);*/
   /*END EXPECTED OUTPUT*/

    /* Invert the LP_BUSY bit to compensate for its inversion in the */
    /* device: see pg 631 in S&S.  Shift the bits to the right by 3 */
    /* to get them in bits 0-4 to prepare for comparison. */
    /* Also make sure all bits at bit 5 or above are off. */
    /* Call "softchip" for chip to get correct result, also in bits 0-4. */
    /* Then compare the two results and report-- */

    if (chip_output != correct_output) {
      printf("Failure on input <%x>:\n", chip_input);
      printf("-- Expected output <%x> does not match chip output <%x>!\n",
              correct_output, chip_output);
      return;  /* give up on first failure */
    }
  }
  printf("*** Chip is OK. No failures encountered. ***\n");
}

/* portable function to get int value from user */
int get_int_in_range(int low, int high)
{
  char buf[MAXBUF];
  int num;

  getline(buf, MAXBUF);
  while ((sscanf(buf,"%d",&num)!=1) || (num<low) || (num>high)) {
    printf("Please enter a decimal number between %d and %d: ",low, high);
    getline(buf, MAXBUF);
  }
  return num;
}    

/* Turn platform-dependent library function calls into platform-independent
   function to get one line from the user.
*/
void getline(char *buf, int maxbuf)
{
#ifdef SAPC
    fgets(buf, maxbuf, CONSOLE);
#else
    fgets(buf, maxbuf, stdin);
#endif
}

