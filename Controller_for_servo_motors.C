// Import all the required required things as per the example given///
#include <xc.h>
#include <pic.h>
#pragma config FOSC=HS, CP=OFF, DEBUG=OFF, BORV=20, BOREN=0, MCLRE=ON, PWRTE=ON, WDTE=OFF
#pragma config BORSEN=OFF, IESO=OFF, FCMEN=0

// Case Study 4//
// Code in C assembly for stepper motors//
/* Variable declarations */
#define PORTBIT(adr,bit) ((unsigned)(&adr)*8+(bit))
// The function PORTBIT is used to give a name to a bit on a port
// The variable RC0 could have equally been used
	
	// Greenpushbutton connected at ___nedtocheck__
	static bit greenButton @ PORTBIT(PORTC,1); //0 or 1 ----> as per other people are doing
	// Redpushbutton connected at __nedtocheck___
	static bit redButton @ PORTBIT(PORTC,0);   //0 or 1 ------> as per other people are doing
	int switch_mode = 0; // Will read from octal switch reading --> this will state the mode
	// State is the internal register on LED i.e. a copy of PortB
	int State = 0;      //  This can be a copy of port B
	int Motor_state_uni = 0; // Keep track of motor
    int Motor_state_bi = 0; // Keep track of motor
    int Motor_state = 0; // Keep track of the sequnce - special for mode 1
	char i; //counter for time calculation
    char iter; // used for misc purposes

//init fucntion
void init (void)
{
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
PORTA = 0B00000000;
// The LED’s connected to Port B will be used to display the State of the program. The lower 4 bits will hold the mode of operation.
PORTB = 0B00000000;	//Connections to the LED
// Push buttons connected to PORTC
PORTC = 0B00000000; //Connections to the pushbuttons
// The connection to the microcomputer will be such that if you turn on the Port D pins in order (i.e. pin 0, pin 1, pin 2, pin 3), the unipolar stepper motor will rotate clockwise.
PORTD = 0B00000000; // Connection to the Motors
// Connection The octal switches connected to Port E can be change
PORTE = 0B00000000; // Connection to the Octal Switch
//--------Ports Defined-----------------//
//---------Define Input/Output----------//
TRISA = 0B1111111;
// LED are all output
TRISB = 0B11110000; //Output to all the led - 4 in total at pin0,1,2,3
// All the pushbuttons are input type
TRISC = 0B11111111;	//Input from the buttons
// Action to motors are output Type
TRISD = 0B00000000; // Control the MotorsConfigure Port D as all output
// Octal Swtitch is input type
TRISE = 0B11111111; // Octal Switch
ADCON1 = 0b00001111;// if required
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	
}

// Switch Delay for the press wait for switches
void SwitchDelay (void) // Waits for switch debounce
{
for (i=200; i > 0; i--) {} // 1200 us delay
}

// alignement function
// This is alignment function
void align (void)
{
	// Call for the stepper motor with 4 poles A B C D
	PORTD = 0B00001000; // Call to pin 3 as high
	// Call pins untill rotors are aligned
	while(1) // while true ----> forever loop untill break is processed
	{
		if (PORTD == 1)
		{
			PORTD = 0B00001000; // if PORTD goes to 0001 i.e. lst pin then call back to 1000
		}
		else
		{
			PORTD = (PORTD >> 1); // this will do 1000 -> 0100 -> 0010 -> 0001
		}
		SwitchDelay(); // Delay 
		//check aligned
		if (PORTAbits.RA0 == 0)
		{
			// Horizontal interuppter aligned
			break; // break the loop
		}
	}
	
	// Call for the stepper motor with 2 Poles A and B
    PORTD = 0B00000000; //  Turn all to OFF
    // Align the poles
    PORTD = 0b10100000; //  Turn pin 1 and 3 both on at a time
    while(1) // while True ---- forever loop untill break is processed
    {
	    // Full Step Sequence Fig.32
	    // A->Pin 4
	    // B->Pin 6
	    //A ->Low B->High
        PORTD = 0B01000000;
        SwitchDelay();
        //A->Low B->Low
        PORTD = 0B00000000;
        SwitchDelay();
        //A->High B->Low
        PORTD = 0B00010000;
        SwitchDelay();
        //A->High B->High
        PORTD = 0B01010000;
        SwitchDelay();
        // if aligned break the loop
        if (PORTAbits.RA2 == 0)
            break;
    }
}

//--------------------Uni Motor move codes start here---------------------//
// The function names are undestandable by name
// Clk - clockwise
// cclk - counter clockwise
// ver - vertical inturrupter
// hor - horizontal inturrupter
// move uni motor counter clockwise ac_ vertical inturrpter
void Uni_move_cclk_ver (void)
{
    PORTD = 0B00001000;
    char temp = 0B00001000;
    while(1)
    {
        temp = (temp >> 1);
        if (temp == 0)
        {
            temp = 0B00001000;
        }
        PORTD = temp;
        SwitchDelay();

        if (PORTAbits.RA1 == 0)
            break;
        }
}

void Uni_move_clk_ver (void)
{
    PORTD = 0B00000001;
    char temp = 0B00000001;
    while (1)
    {
        temp = (temp << 1);
        if (temp == 0B00010000)
        {
            temp = 0B00000001;
        }
        PORTD = temp;
        SwitchDelay();
        if (PORTAbits.RA1 == 0)
            break;
    }
}

void Uni_move_cclk_hor (void)
{
    PORTD = 0B00001000;
    char temp = 0B00001000;
    while(1)
    {
        temp = (temp >> 1);
        if (temp == 0)
        {
            temp = 0B00001000;
        }
        PORTD = temp;
        SwitchDelay();
        if (PORTAbits.RA0 == 0)
        break;       
    }    
}

void Uni_move_clk_hor (void)
{
    PORTD = 0B00000001;
    char temp = 0B00000001;
    while(1)
    {
        temp = (temp >> 1);
        if (temp == 0)
        {
            temp = 0B00001000;
        }
        PORTD = temp;
        SwitchDelay();
        if (PORTAbits.RA0 == 0)
        break;       
    }    
}
//-------------------Uni motor move codes end here---------------//


//-------------------Bi motor move codes start here---------------//
int get_step (int Motor_state_bi)
{
    // steps of the Bi motor mimic
    switch(Motor_state_bi)
    {
        case 0:
            return 0B01000000;
        case 1:
            return 0B00000000;
        case 2:
            return 0B00010000;
        case 3:
            return 0B01010000;
    }
    return 0;
}
// The function names are undestandable by name
// Clk - clockwise
// cclk - counter clockwise
// ver - vertical inturrupter
// hor - horizontal inturrupter
void Bi_move_cclk_ver (void)
{
    PORTD = 0B10100000;
    while (1)
    {
        PORTD = get_step(Motor_state_bi);
        Motor_state_bi = (Motor_state_bi - 1) % 4;
        SwitchDelay();
        if (PORTAbits.RA3 == 0)
        break;
    }
}

void Bi_move_clk_ver (void)
{
    PORTD = 0B10100000;
    while (1)
    {
        PORTD = get_step(Motor_state_bi);
        Motor_state_bi = (Motor_state_bi +1) % 4;
        SwitchDelay();
        if (PORTAbits.RA3 == 0)
        break;
    }
}

void Bi_move_cclk_hor (void)
{
    PORTD = 0B10100000;
    while (1)
    {
        PORTD = get_step(Motor_state_bi);
        Motor_state_bi = (Motor_state_bi - 1) % 4;;
        SwitchDelay();
        if (PORTAbits.RA2 == 0)
        break;
    }   
}

void Bi_move_clk_hor (void)
{
    PORTD = 0B10100000;
    while (1)
    {
        PORTD = get_step(Motor_state_bi);
        Motor_state_bi = (Motor_state_bi +1) % 4;
        SwitchDelay();
        if (PORTAbits.RA2 == 0)
        break;
    }   
}
//-------------------Bi motor move codes ends here---------------//

//-------------------------------MODE 1 CODE----------------------//
void move_in_1 (void)
{
    // Ref -- Lecture 8 Slide 42
    switch(Motor_state) //Mode 1 of Case study 4
        {
            case 0:
                Uni_move_cclk_ver();
                break;
            case 1:
                Bi_move_clk_ver();
                break;
            case 2:
                Uni_move_clk_hor();
                break;
            case 3:
                Bi_move_cclk_hor();
                break;
        }
        Motor_state = (Motor_state + 1) % 4; 
}
// Mode 1 for the octal position engaged
void mode_1 (void)
{
    // call Alignment of the motor, run a full sequence
	align(); //--DONE--//
    // now wait til the red button is pressed again
    while (1)
    {
        if (redButton == 1)
        {
            // wait for debounce
            SwitchDelay();
            move_in_1();
        }
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
    }

    // Now strt the sequence

}
//------------------------MODE 1 CODE ENDS HERE--------------------//

//-------------------------MODE 2 STARTS HERE---------------------//
void mode_2_move_uni_cclk (void)
{
    // wait for the delay
    SwitchDelay();
    switch (Motor_state_uni)
    {
        case 0:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 1;
            Motor_state_uni = 1;
            break;
        case 1:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 1;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 2;
            break;
        case 2:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 1;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 3;
            break;
        case 3:
            PORTDbits.RD0 = 1;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 3;
            break;
    }
}

void mode_2_move_uni_clk (void)
{
    // wait for the delay
    SwitchDelay();
    switch (Motor_state_uni)
    {
        case 0:
            PORTDbits.RD0 = 1;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 1;
            break;
        case 1:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 1;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 2;
            break;
        case 2:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 1;
            PORTDbits.RD3 = 0;
            Motor_state_uni = 3;
            break;
        case 3:
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 1;
            Motor_state_uni = 3;
            break;
    }
}

void mode_2_move_bi_cclk (void)
{
    // wait for the delay
    SwitchDelay();
    switch (Motor_state_bi)
    {
        case 0:
            PORTDbits.RD4 = 0;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 1;
            PORTDbits.RD7 = 0; 
            Motor_state_bi = 1;
        case 1:
            PORTDbits.RD4 = 0;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 0;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 2;
        case 2:
            PORTDbits.RD4 = 1;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 0;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 3;
        case 3:
            PORTDbits.RD4 = 1;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 1;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 0;
    }
}

void mode_2_move_bi_clk (void)
{
    // wait for the delay
    SwitchDelay();
    switch (Motor_state_bi)
    {
        case 0:
            PORTDbits.RD4 = 1;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 1;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 1;
        case 1:
            PORTDbits.RD4 = 1;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 0;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 2;
        case 2:
            PORTDbits.RD4 = 0;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 0;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 3;
        case 3:
            PORTDbits.RD4 = 0;
            PORTDbits.RD5 = 0;
            PORTDbits.RD6 = 1;
            PORTDbits.RD7 = 0;
            Motor_state_bi = 0;
    }
}
void mode_2 (void)
{
    // firstly align the inturrpter
    align();
    // Done: Both motors start at the horizontal interrupter position.
    //After the red button is pressed, both motors are cyclically driven between the two interrupters continuously by the shortest path.
    //It contains additional circuitry that produces a digital hi output when 
    //the interrupter is open and a digital lo output when the interrupter is blocked.
    while(1)
    {
        if (redButton==1)
        {
            SwitchDelay();
            while(redButton==1){}
            break;
        }
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
    }
    iter = 0;
    while (1)
    {
        // if greenbutton at any time
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
        else if (iter == 0)
        {
            while (!(PORTAbits.RA1==1 && PORTAbits.RA3==1))
            {
                if(PORTAbits.RA1==0)
                {
                    mode_2_move_uni_cclk();
                }
                if (PORTAbits.RA3==0)
                {
                    mode_2_move_bi_clk();
                }

            }
            iter = 1;
        }
        else if (iter ==1)
        {
            while (!(PORTAbits.RA0==1 && PORTAbits.RA2==1))
            {
                if (redButton==1)
                {
                    //If the red button is held down as the motors reach the horizontal interrupters (the starting position), the motion is stopped. Otherwise the motion continues. To start the motion again, the red button must be released and then pressed and released again.
                    while(redButton==1){}
                    while(redButton==0){}
                    while(redButton==1){}
                }
                if(PORTAbits.RA0==0)
                {
                    mode_2_move_uni_clk();
                }
                if (PORTAbits.RA2==0)
                {
                    mode_2_move_bi_cclk();
                }

            }
            iter = 0;           
        }
    }
}
//-------------------------MODE 2 ENDS HERE---------------------//


//--------------------------MODE 3 STARTS HERE------------------//
// alignement function
// This is alignment function
void align_for_mode_3 (void)
{
	// Call for the stepper motor with 4 poles A B C D
	PORTD = 0B00001000; // Call to pin 3 as high
	// Call pins untill rotors are aligned
	while(1) // while true ----> forever loop untill break is processed
	{
		if (PORTD == 1)
		{
			PORTD = 0B00001000; // if PORTD goes to 0001 i.e. lst pin then call back to 1000
		}
		else
		{
			PORTD = (PORTD >> 1); // this will do 1000 -> 0100 -> 0010 -> 0001
		}
		SwitchDelay(); // Delay 
		//check aligned
		if (PORTAbits.RA0 == 0)
		{
			// Horizontal interuppter aligned
			break; // break the loop
		}
	}
	
	// Call for the stepper motor with 2 Poles A and B
    PORTD = 0B00000000; //  Turn all to OFF
    // Align the poles
    PORTD = 0b10100000; //  Turn pin 1 and 3 both on at a time
    while(1) // while True ---- forever loop untill break is processed
    {
	    // Full Step Sequence Fig.32
	    // A->Pin 4
	    // B->Pin 6
	    //A ->Low B->High
        PORTD = 0B01000000;
        SwitchDelay();
        //A->Low B->Low
        PORTD = 0B00000000;
        SwitchDelay();
        //A->High B->Low
        PORTD = 0B00010000;
        SwitchDelay();
        //A->High B->High
        PORTD = 0B01010000;
        SwitchDelay();
        // if aligned break the loop
        if (PORTAbits.RA3 == 0)
            break;
    }
}

void mode_3 (void)
{
    align_for_mode_3();
    while(1)
    {
        if (redButton==1)
        {
            SwitchDelay();
            while(redButton==1){}
            break;
        }
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
    }
    iter = 0;
    while (1)
    {
        // if greenbutton at any time
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
        else if (iter == 0)
        {
            while (!(PORTAbits.RA1==1 && PORTAbits.RA2==1))
            {
                if(PORTAbits.RA1==0)
                {
                    mode_2_move_uni_clk();
                }
                if (PORTAbits.RA3==0)
                {
                    mode_2_move_bi_clk();
                }

            }
            iter = 1;
        }
        else if (iter ==1)
        {
            while (!(PORTAbits.RA0==1 && PORTAbits.RA3==1))
            {
                if (redButton==1)
                {
                    //If the red button is held down as the motors reach the horizontal interrupters (the starting position), the motion is stopped. Otherwise the motion continues. To start the motion again, the red button must be released and then pressed and released again.
                    while(redButton==1){}
                    while(redButton==0){}
                    while(redButton==1){}
                }
                if(PORTAbits.RA0==0)
                {
                    mode_2_move_uni_cclk();
                }
                if (PORTAbits.RA2==0)
                {
                    mode_2_move_bi_cclk();
                }

            }
            iter = 0;           
        }
    }
    return;
}
//--------------------------MODE 3 ENDS HERE-------------------//

//-------------------------MODE 4 STARTS HERE--------------------//
void mode_4 (void)
{
    // firstly align the inturrpter
    align();
    // Done: Both motors start at the horizontal interrupter position.
    //After the red button is pressed, both motors are cyclically driven between the two interrupters continuously by the shortest path.
    //It contains additional circuitry that produces a digital hi output when 
    //the interrupter is open and a digital lo output when the interrupter is blocked.
    while(1)
    {
        if (redButton==1)
        {
            SwitchDelay();
            while(redButton==1){}
            break;
        }
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
    }
    iter = 0;
    while (1)
    {
        // if greenbutton at any time
        if (greenButton==1)
        {
            // End the code
            // a new mode will be given now
            switch_mode = 0; // clear the mode in memory
            turn_leds(); // indicate by lights
            return;
        }
        else if (iter == 0)
        {
            while (!(PORTAbits.RA1==1 && PORTAbits.RA3==1))
            {
                if(PORTAbits.RA1==0)
                {
                    mode_2_move_uni_cclk();
                }
                if (PORTAbits.RA3==0)
                {
                    mode_2_move_bi_clk();
                }

            }
            iter = 1;
        }
        else if (iter ==1)
        {
            while (!(PORTAbits.RA0==1 && PORTAbits.RA2==1))
            {
                if (redButton==1)
                {
                    //If the red button is held down as the motors reach the horizontal interrupters (the starting position), the motion is stopped. Otherwise the motion continues. To start the motion again, the red button must be released and then pressed and released again.
                    while(redButton==1){}
                    while(redButton==0){}
                    while(redButton==1){}
                }
                if(PORTAbits.RA0==0)
                {
                    mode_2_move_uni_clk();
                }
                if (PORTAbits.RA2==0)
                {
                    mode_2_move_bi_cclk();
                }

            }
            iter = 0;           
        }
    }
    return;
}
//-------------------------MODE 4 ENDS HERE--------------------//


//MODE 5
//5. Mode Changes.
//Modes can be changed whenever the motors have stopped by pressing the green pushbutton. Note, in mode 2, 3 and 4, the red button should be held down prior to the motor reaching the horizontal interrupter position. The motor is then stopped. If the red button is pressed and released, the operation continues in the mode selected. If the green button is pressed, the motors can switch to another mode. Note also that if you switch between modes, you have to initialize the motors at the correct interrupters.
// Already implemented


// This is the faulty mode
void mode_fault (void)
{
	// show the light on the LED for error status
	PORTBbits.RB0 = 0; // Turn off in into 1st mode
    PORTBbits.RB1 = 0; // Turn off if into 2nd mode
    PORTBbits.RB2 = 0; // Turn off if into 3rd mode
    PORTBbits.RB3 = 1; // Light number 4
    State = PORTB; // State can be copy of PORTB
    // DO NOTHING OTHER THAN LIGHTS
}

//// This function will read the octal switch and engage the different modes
void apply_mode (void)
{
    if (switch_mode == 1)
    {
        // engage into mode 1
        mode_1();
    }
    else if (switch_mode==2)
    {
        // engage into mode 2
        mode_2();
    }
    else if (switch_mode==3)
    {
        // engage into mode 3
        mode_3();
    }
    else if (switch_mode==4)
    {
        // engage into mode 4
        mode_4();
    }
    else
    {
        // Not a correct selection and call the fault
        mode_fault();
    }

}

// turning the leds to the mode selected
// This function will turn on the lights as per the mode selected
int turn_leds (void)
{
	// turn the number of lights as indicated on switch mode
	PORTB = switch_mode;
	State = PORTB; // State can be a copy of PORTB
}

// main function to run
void main (void)
{	
	// Initialize the pins using init command
	init(); //--DONE--//
    // start a never ending loop 
    while(1) // 1 --> TRUE
    {
        // If no mode is selected at first it will be mode 0 and okay
        // Once we have the modes selected we can run the same
        // Check if redbutton is not_pressed
        if(redButton == 0)
        {
            // we wait untill the red button is finally pressed 
            while(redButton == 0){} // Break the loop when pressed
            // Do nothing untill the red button is not pressed
            // waiting for the switch to debounce
            SwitchDelay(); //--DONE--//
            // start the mode
            apply_mode ();
        }
        ///The program will then wait until the green pushbutton is pressed. The LED’s will indicate ‘0000’ (status - waiting for start, mode 0)
        // Code starts in the new mode when the grren button is pressed to read the modes
        // Check if greenbutton is presed
        // this will call the actual reading 
        if(greenButton == 0) //checking green press
        {
            // if not pressed - wait untill pressed
            while(greenButton == 0){} // infinite loop untill pressed
            // Do nothing untill the green button is not pressed
            // waiting for the switch to debounce
            SwitchDelay(); //--Done--//
            // Read the mode
            // storwe the mode int the switch mode
            // By converting bit into int and then sub 7
            switch_mode = 7 - (PORTEbits.RE0 + 2*PORTEbits.RE1 + 4*PORTEbits.RE2);
            // Now we need to turn the lights as per selected mode
            turn_leds(); //--DONE--//
        }

	}
}