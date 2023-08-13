
#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define LED BIT6
#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCHES 15

short redrawScreen = 1;
int start = 1; // shows initialization / loading screen
//int lives = 3; // update to change graphics
int switches = 0;
//int buzzer = 0;
//int game = 0;
//int clear = 0;

//int inClock = 0; // game loops
//int loop = 1;

void buzzer_init()
{
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6; /* enable output to speaker (P2.6) */
}

void buzzer_set_period(short cycles) /* buzzer clock = 2MHz. 5000 low-ish */
{
  CCR0 = cycles; 
  CCR1 = cycles >> 1;		/* one half cycle */
}


static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
  start = 0;
  
  // RED COLOR /////////////////////////////////////////////
  // RED = 4300, GREEN = 3950, BLUE = 3600, YELLOW = 3250 //
  //////////////////////////////////////////////////////////
  
  if(!(P2IN & SW1)){
    redScreen();
    
    // +Green combined to make Yellow (questionable having this here)
    if(!(P2IN & SW2)){
      yellowScreen();
      buzzer_set_period(4125);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Blue combined to make Purple
    else if(!(P2IN & SW3)){
      purpleScreen();
      buzzer_set_period(3950);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
    
    // +Yellow combimed to make Orange
    else if(!(P2IN & SW4)){
      orangeScreen();
      buzzer_set_period(3775);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
    
    else { // standard Red Noise
    buzzer_set_period(4300);
    __delay_cycles(2000000);
    buzzer_set_period(0);
       }
  }
  
  // GREEN COLOR /////////////////////////////
  else if(!(P2IN & SW2)){
    greenScreen();
    
    // +Red combined to make Yellow (Still questionable)
    if(!(P2IN & SW1)){
      yellowScreen();
      buzzer_set_period(4125);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Blue combined to make Cyan
    else if(!(P2IN & SW3)){
      cyanScreen();
      buzzer_set_period(3775);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Yellow combined to make Green Yellow
    else if(!(P2IN & SW4)){
      greenYellowScreen();
      buzzer_set_period(3600);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
    
    else {
      buzzer_set_period(3950); // - 350
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
  }

  // BLUE COLOR //////////////////////////////
  else if(!(P2IN & SW3)){
    blueScreen();

    // +Red combined to make Purple
    if(!(P2IN & SW1)){
      purpleScreen();
      buzzer_set_period(3950);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Green combined to make Cyan
    else if(!(P2IN & SW2)){
      cyanScreen();
      buzzer_set_period(3775);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Yellow combined to make Green
    else if(!(P2IN & SW4)){
      greenScreen();
      buzzer_set_period(3425);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
    
    else {
      buzzer_set_period(3600); // -350
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
  }

  // YELLOW COLOR ////////////////////////////
  else if(!(P2IN & SW4)){
    yellowScreen();

    // +Red combined to make Orange
    if(!(P2IN & SW1)){
      orangeScreen();
      buzzer_set_period(3775);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Green combined to make Green-Yellow
    else if(!(P2IN & SW2)){
      greenYellowScreen();
      buzzer_set_period(3600);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }

    // +Blue combined to make Green
    else if(!(P2IN & SW3)){
      greenScreen();
      buzzer_set_period(3425);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
    
    else{
      buzzer_set_period(3250);
      __delay_cycles(2000000);
      buzzer_set_period(0);
    }
  }
  else{
    // empty here
  }
  
}

// Color Changes
void redScreen(){
  fillRectangle(5,15,117,96,COLOR_RED);
}

void greenScreen(){
  fillRectangle(5,15,117,96,COLOR_GREEN);
}

void blueScreen(){
  fillRectangle(5,15,117,96,COLOR_BLUE);
}

void yellowScreen(){
  fillRectangle(5,15,117,96,COLOR_YELLOW);
}

void orangeScreen(){
  fillRectangle(5,15,117,96,COLOR_ORANGE);
}

void purpleScreen(){
  fillRectangle(5,15,117,96,COLOR_PURPLE);
}

void cyanScreen(){
  fillRectangle(5,15,117,96,COLOR_CYAN);
}

void greenYellowScreen(){
  fillRectangle(5,15,117,96,COLOR_GREEN_YELLOW);
}
 
void updateScreen(){
  //fillRectangle(40,60,x,x,COLOR_RED);
  u_char width = screenWidth, height = screenHeight;
  clearScreen(COLOR_GOLDENROD); // yellowish background
  
  // (init-col, init-row, end-col, end-row)) | Button area
  fillRectangle(screenHeight,screenWidth,50,150,COLOR_BLACK);
  fillRectangle(4,screenWidth+3,120,27,COLOR_BROWN); // brown background
  fillRectangle(5,screenWidth+4,27,25,COLOR_RED); // red square
  fillRectangle(35,screenWidth+4,27,25,COLOR_GREEN); // Green square
  fillRectangle(65,screenWidth+4,27,25,COLOR_BLUE); // Blue square
  fillRectangle(95,screenWidth+4,27,25,COLOR_YELLOW); // Yellow Square
  
  // Screen area + Color Names
  fillRectangle(3,13,121,100,COLOR_BLACK); // screen window
  fillRectangle(5,15,117,96,COLOR_TAN);
  drawString5x7(8,117,"Red", COLOR_BLACK,COLOR_GOLDENROD);
  drawString5x7(31,117,"Green",COLOR_BLACK,COLOR_GOLDENROD);
  drawString5x7(64,117,"Blue",COLOR_BLACK,COLOR_GOLDENROD);
  drawString5x7(90,117,"Yellow",COLOR_BLACK,COLOR_GOLDENROD);

}

void topScreen(){
  
  if(start){
    // (init-col, init-row, end-col, end-row)
    fillRectangle(9,4,4,7,COLOR_RED); // vertical
    fillRectangle(7,6,8,3,COLOR_RED); // horizontal
    fillRectangle(114,4,4,7,COLOR_RED); // vertical 2
    fillRectangle(112,6,8,3,COLOR_RED); // horizontal 2
  }
  
  // Title
  drawString5x7(20,4,"Paint The Wall!",COLOR_BLACK,COLOR_GOLDENROD);
}

void wdt_c_handler(){
  
  static int secCount = 0;
  secCount++;

  // if idle, loading sequence || POSSIBLE BUG, only button work when text is off screen
  if(start){
    if(secCount > 200){ // originally 200
      drawString5x7(43,60,"MSP-430",COLOR_BLACK,COLOR_TAN);
      drawString5x7(13,100,"Press any button..",COLOR_BLACK,COLOR_TAN);
    }
    if(secCount >= 300){ // originally 400
      drawString5x7(43,60,"MSP-430",COLOR_TAN,COLOR_TAN);
      secCount = 0;
    }
    redrawScreen = 1;
  }
}

void main(){
  P1DIR |= LED;
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();
 
  enableWDTInterrupts();
  or_sr(0x8);

  // set Screen - Graphics Prior to running any code 
  updateScreen();
  topScreen();
  
  while(1){
    if(redrawScreen){
      redrawScreen = 0; 
     
    }
    P1OUT &= ~LED;
    or_sr(0x10);
    P1OUT |= LED;
  }
}

void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
