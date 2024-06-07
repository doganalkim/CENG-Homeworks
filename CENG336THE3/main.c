/*
 * File:   main.c
 * Author: e2521482
 *
 * Created on May 24, 2024, 12:01 PM
 */

#include "pragmas.h"
#include <xc.h>

#define SPBRG_VAL (86)      // SPRG value corresponding our settings for 115.2K Baud rate
#define TH (0x0B)           // Timer initalization high byte
#define TL (0xDB)           // Timer initialization low byte
#define BUFSIZE 512         // Buffer size
#define PKTSZ 9             // packet size is equal to 9 bytes since longest
                            // command is at most 9 bytes length
typedef enum {INBUF = 0, OUTBUF = 1} buf_t;
volatile int numberOfCommands = 0;

// These are the  buffer variables directly copied from Uluç Hoca's example
uint8_t inbuf[BUFSIZE];   /* Preallocated buffer for incoming data */
uint8_t outbuf[BUFSIZE];  /* Preallocated buffer for outgoing data  */
uint8_t head[2] = {0, 0}; /* head for pushing, tail for popping */
uint8_t tail[2] = {0, 0};
uint8_t buf_isempty( buf_t buf ) { return (head[buf] == tail[buf])?1:0; }


// operation for pushing the variables into ring buffer
void buf_push( uint8_t v, buf_t buf) {
    if (buf == INBUF) inbuf[head[buf]] = v;
    else outbuf[head[buf]] = v;
    head[buf]++;
    if (head[buf] == BUFSIZE) head[buf] = 0; // to make it circular
}

//operation for popping the variables from the ring buffer
uint8_t buf_pop( buf_t buf ) {
    uint8_t v;
    if (buf_isempty(buf)) { 
        return 0; 
    } else {
        if (buf == INBUF) v = inbuf[tail[buf]];
        else v = outbuf[tail[buf]];
        tail[buf]++;
        if (tail[buf] == BUFSIZE) tail[buf] = 0; // to make it circular
        return v;
    }
}

// enabling and disabling the serial communication of both transmission and retrieve
inline void disable_rxtx( void ) { PIE1bits.RC1IE = 0;PIE1bits.TX1IE = 0;}
inline void enable_rxtx( void )  { PIE1bits.RC1IE = 1;PIE1bits.TX1IE = 1;}

// initializing the ports
void init_ports(){
    SPBRG1 = 0;
     SPBRGH1 = 0;       // Initializing them with 0 just for safety
     RCSTA1 = 0;
     TXSTA1 = 0;
     T0CON = 0;
     PIE1 = 0;
     PIR1 = 0;
     ADCON2 = 0;
     ADCON1 = 0;
     ADCON0 = 0;
     RCON = 0;
     INTCON2 = 0;
     INTCON = 0;
     BAUDCON1  = 0;
     PORTH = 0;
     LATH = 0;
     TRISH = 0;
     PORTA = 0;
     LATA = 0;
     TRISA = 0;
     PORTB = 0;
     LATB = 0;
     TRISB = 0;
     PORTC = 0;
     LATC  = 0;
     TRISC = 0;
     PORTD = 0;
     LATD = 0;
     TRISD = 0;
             
    
    /*
    BAUDCON1 = INTCON = INTCON2 = RCON = ADCON0 = ADCON1 = ADCON2 = PIR1 = PIE1 = T0CON = TXSTA1 = RCSTA1 = SPBRGH1 = SPBRG1 = 0x00;
    TRISH = LATH = PORTH = 0x00;
    TRISA = LATA = PORTA = 0x00;
    TRISB = LATB = PORTB = 0x00;
    TRISC = LATC = PORTC = 0x00;
    TRISD = LATD = PORTD = 0x00;*/
    
    TRISHbits.RH4 = 1; // RH4 is set for ADCON case
    TRISB = 0xF0;      // RB4-7 are input for MAN command
    
    TRISCbits.RC7 = 1; // RC7 is input for serial communication -> retrieve
    TRISCbits.RC6 = 0; // RC6 is output for serial communication -> transmission
    
}

void init_adcon(){           // Initializing the ADCON
    ADCON0 = 0x30;           //  We choose channel 12 
    ADCON1 = 0x00;           // just zero
    ADCON2 = 0x8A;           // Right justified - acquisition time is 2TAD - clock is Flosc/32
    ADRESH = ADRESL = 0x00;  // Registers we read the result of 
}

void init_serial(){
    TXSTA1 = 0x04;           // BRGH =  1 -> high speed is selected
    RCSTA1 = 0x90;           // Serial port is enabled, Continuous receive is enable
    
    BAUDCON1bits.BRG16 = 1;  // Baud rate register is selected to be 16 bits width
    
    // Loading our Baud rate selection value for 115.2K baud rate                                
    SPBRGH1 = (SPBRG_VAL >> 8) & 0xFF;
    SPBRG1 = SPBRG_VAL & 0xFF;
}

void init_interrupts(){
    INTCONbits.TMR0IE = 1;      // Enabling timer
    
    INTCON2bits.RBIP = 1;       // Enabling priorities
    INTCON2bits.RBPU = 0;           
    //INTCON2bits.INTEDG0 = 1;
    //INTCON2bits.INTEDG1 = 1;
    
    //PIE1bits.ADIE = 1;
    
    enable_rxtx();              // Enable serial port communication
    
    INTCONbits.PEIE = 1;        // Enable peripheral interrupts
    INTCONbits.GIE = 1;         // Enable global interrupts
}

void init_timer(){
    T0CONbits.T0PS2 = 0;        // The prescaler chosen as16
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 1; 
}

#define PKT_WAIT_HDR (0)      // Macros for state machine in packet task
#define PKT_WAIT_BDY (1)
#define PKT_WAIT_END (2)
#define PKT_START ('$')
#define PKT_END ('#')


int packet_index = 0, packet_state = PKT_WAIT_HDR, pkt_rcv_valid = 0;
volatile char pkt_rcv[PKTSZ];


void packet_task(){
    disable_rxtx();                             // To prevent buffer corruption
    if(buf_isempty(INBUF)){                     // return if input buffer is empty                    
        enable_rxtx();
        return;
    }
    uint8_t v;
    switch(packet_state){
        case PKT_WAIT_HDR:
            v = buf_pop(INBUF);                     
            if(v == PKT_START){                  // Packet starting detected 
                packet_state = PKT_WAIT_BDY;     // $ has received
                packet_index = 0;
            }
            break;
        case PKT_WAIT_BDY:
            v = buf_pop(INBUF);                                     
            if(v == PKT_END){                     // We are at the body of the packet
                packet_state = PKT_WAIT_END;        
                pkt_rcv_valid = 1;
            }
            else if(v == PKT_START){              // Error handling 
                packet_state = PKT_WAIT_HDR;
                packet_index = 0;
            }
            else {
                pkt_rcv[packet_index++] = v;       // Writing the command to another array
            }                                      // We just write the bytes between $ and # 
            break;
            
        case PKT_WAIT_END:
            if(pkt_rcv_valid == 1){                // # has received
                break;                             // reading the command is done
            }
            packet_index = 0;
            packet_state = PKT_WAIT_HDR;
            break;
    }
    enable_rxtx();
}

void receive_isr(){
    uint8_t val = RCREG1;
    buf_push(val, INBUF);    // Buffer incoming byte
    PIR1bits.RC1IF = 0;      // Acknowledge interrupt
}

volatile int distance = 0,speed = 0;

int get_parsed_int(){           // parse the read integer value and return it
    unsigned int int_val;
    char temp[5];
    temp[0] = pkt_rcv[3];
    temp[1] = pkt_rcv[4];
    temp[2] = pkt_rcv[5];
    temp[3] = pkt_rcv[6];
    temp[4] = 0;
    sscanf(temp ,"%04x", &int_val);
    return int_val;
}

void reset_timer(){             // function for resetting the timer
    //INTCONbits.TMR0IF = 0;
    TMR0H = TH;
    TMR0L = TL;
}

void parse_and_push_dist(){         // function for pushing the
    uint8_t temp[5];                // current distance value to buffer
    sprintf(temp, "%04x",distance);
    for(int i = 0; i < 4 ; i++)
        buf_push( temp[i], OUTBUF);
}

void fill_buffer_distance(){       // function for filling DST command  
    disable_rxtx(); 
    buf_push( '$', OUTBUF);
    buf_push( 'D', OUTBUF);
    buf_push( 'S', OUTBUF);
    buf_push( 'T', OUTBUF);
    parse_and_push_dist();
    buf_push( '#' , OUTBUF);
    enable_rxtx();
}


void parse_and_push_alt(){          // Parsing the current altitude and
    uint8_t temp[5];                // pushing it to buffer
    unsigned int result = (ADRESH << 8) + ADRESL; // Get the result;
    
    if(result < 256 ) result = 9000;        // finding the correct interval of adcon result
    else if(result < 512) result = 10000;
    else if(result <  768) result = 11000;
    else result = 12000;
    
    sprintf(temp, "%04x",result);
    for(int i = 0; i < 4 ; i++)
        buf_push( temp[i], OUTBUF);   
}

void parse_and_push_prs(uint8_t val ){      // pushing the prs integer value into buffer
    uint8_t temp[5];
    sprintf(temp, "%04x",val);
    for(int i = 2; i < 4 ; i++)
        buf_push( temp[i], OUTBUF);
}

void fill_buffer_alt(){                     // pushing altitude command into buffer
    disable_rxtx();
    buf_push( '$', OUTBUF);
    buf_push( 'A', OUTBUF);
    buf_push( 'L', OUTBUF);
    buf_push( 'T', OUTBUF);
    parse_and_push_alt();
    buf_push( '#' , OUTBUF);
    enable_rxtx();
}

void go_command_received(){                 // Handling the GO command case
    reset_timer();
    distance = get_parsed_int();
    T0CONbits.TMR0ON = 1;                   // start timer
}

int get_parsed_man_and_led(){               // parse the integer read from man
    unsigned int int_val;                   // and return the result
    char temp[3];
    temp[0] = pkt_rcv[3];
    temp[1] = pkt_rcv[4];
    temp[2] = 0;
    sscanf(temp ,"%02x", &int_val);
    return int_val;                         
}

volatile int end_sys = 0;

void end_command_received(){                // end command handler
    while( !TXSTA1bits.TRMT);
    TXSTA1bits.TXEN = 0;
    INTCONbits.GIE = 0;
    end_sys = 1;
}


void spd_command_received(){
    speed = get_parsed_int();               // update the current speed
}

volatile int alt_period, alt_counter = 1, alt_value, alt_received_flag = 0;

void alt_command_received(){                // read the period of alt command
    alt_period = alt_counter = get_parsed_int() / 100;
    if(!alt_period){                        // alt0000 is read and end alt command
        ADCON0bits.ADON = 0;
        alt_received_flag = 0;
    }
    else{                                   // start sending periodic alt commands
        alt_received_flag = 1;
    }
}

volatile uint8_t prevb = 0xF0, curb = 0x00;

void turn_off_all_leds(){                   // turning off the all leds
    LATAbits.LA0 = 0;
    LATBbits.LB0 = 0;
    LATCbits.LC0 = 0;
    LATDbits.LD0 = 0;
    PORTAbits.RA0 = 0;
    PORTBbits.RB0 = 0;
    PORTCbits.RC0 = 0;
    PORTDbits.RD0 = 0;
}

volatile int man_received = 0, flagrb4  = 0, flagrb5 = 0, flagrb6 = 0, flagrb7 = 0;

void reset_flags(){                                // turn of all leds
    flagrb4 = 0;
    flagrb5 = 0;
    flagrb6 = 0;
    flagrb7 = 0;
}

void man_command_received(){                       // Function for MAN command handling 
    uint8_t val = get_parsed_man_and_led();        // Read the integer from the command
    if(!val){                                           
        INTCONbits.RBIE = 0;                       // disable interrupts
        INTCONbits.RBIF = 0;
        man_received = 0;
    }
    else{
        man_received = 1;                          //
        INTCONbits.RBIE = 1;                       // Enable PORTB interrupts
    }
}


void led_command_received(){                        // Receive led command and 
    uint8_t val = get_parsed_man_and_led();         // lit the correct bit
    switch(val){
        case 0:
            turn_off_all_leds();
            break;
        case 1:
            LATDbits.LD0 = 1;
            break;
        case 2:
            LATCbits.LC0 = 1;
            break;
        case 3:
            LATBbits.LB0 = 1;
            break;
        case 4:
            LATAbits.LA0 = 1;
            break;
    }
}

void parse_received_packet(){         // function for parsing the commands
    if(pkt_rcv_valid == 0){;          // the packet in the packet buffer is not ready yet
        return;
    }
    pkt_rcv_valid = 0;
    if( pkt_rcv[0] == 'G' && pkt_rcv[1] == 'O' && pkt_rcv[2] == 'O' ){  // go command received
        go_command_received();
    }
    if(pkt_rcv[0] == 'E' && pkt_rcv[1] == 'N' && pkt_rcv[2] == 'D'){    // end command received
        end_command_received();
    }
    if(pkt_rcv[0] == 'S' && pkt_rcv[1] == 'P' && pkt_rcv[2] == 'D'){    // spd command received
        spd_command_received();
    }
    if(pkt_rcv[0] == 'A' && pkt_rcv[1] == 'L' && pkt_rcv[2] == 'T' ){   // alt command received
        alt_command_received();
    }
    if(pkt_rcv[0] == 'M' && pkt_rcv[1] == 'A' && pkt_rcv[2] == 'N'){    // man command received
        man_command_received();
    }
    if(pkt_rcv[0] == 'L' && pkt_rcv[1] == 'E' && pkt_rcv[2] == 'D'){    // led command received
        led_command_received();
    }
}



void transmit_isr(){                // service routine for transmitting
    if (buf_isempty(OUTBUF)){
        while( !TXSTA1bits.TRMT);   // wait for the last byte to be transmitted
        TXSTA1bits.TXEN = 0;        // disable transmission
        return;
    }
    else {
        uint8_t v = buf_pop(OUTBUF);
        TXREG1 = v;                 // transmit a byte
    }
}

void fill_buffer_prs(int pressed_led){      // fill the buffer for conveying prs command
    disable_rxtx();
    buf_push( '$', OUTBUF);
    buf_push( 'P', OUTBUF);
    buf_push( 'R', OUTBUF);
    buf_push( 'S', OUTBUF);
    parse_and_push_prs(pressed_led);
    buf_push( '#' , OUTBUF);
    enable_rxtx();
    
}

volatile int interrupt_flag = 0;


void manager(){
    if(!interrupt_flag) return;
    //INTCONbits.GIE = 0;
    
    if (alt_received_flag)                  // decrement the period counter for alt command
            alt_counter--;
    
    distance -= speed;                      // update the distance each 100 ms      
    
    if(!alt_counter && alt_received_flag){  // if alt command received then start conversion
            ADCON0bits.ADON = 1;            // reset flags
            ADCON0bits.GO = 1;
            ADCON0bits.GODONE = 1;
            PIE1bits.ADIE = 1;
            alt_counter = alt_period;
    }
    else if(flagrb4){                       // rb4 released
        fill_buffer_prs(4);                 // send prs04
        TXSTA1bits.TXEN = 1;
    }   
    else if(flagrb5){                       // rb5 released
        fill_buffer_prs(5);                 // send prs05
        TXSTA1bits.TXEN = 1;
    }else if(flagrb6){                      // rb6 released
        fill_buffer_prs(6);                 // send prs06
        TXSTA1bits.TXEN = 1;
    }
    else if(flagrb7){                       // rb7 released
        fill_buffer_prs(7);                 // send prs07
        TXSTA1bits.TXEN = 1;
    }
    else{                                   // send distance
        fill_buffer_distance();
        TXSTA1bits.TXEN = 1;
    }
    reset_flags();
    
    interrupt_flag = 0;
    //INTCONbits.GIE = 1;
}

void __interrupt(high_priority) highPriorityISR(void) {
    
    if (PIR1bits.RC1IF){
        receive_isr();              // receive
    }
    if (  PIR1bits.TX1IF){
        transmit_isr();             // transmit
    }
    
    
    if(PIR1bits.ADIF && ADCON0bits.ADON  ){ // ADCON conversion has completed
        PIR1bits.ADIF = 0;
        ADCON0bits.ADON = 0;
        PIE1bits.ADIE = 0;
        fill_buffer_alt();
        TXSTA1bits.TXEN = 1;
    }
    
    if(INTCONbits.TMR0IF == 1 && T0CONbits.TMR0ON == 1){
        INTCONbits.TMR0IF = 0;          // timer interrupt occur
        reset_timer();                  // reset timer
        interrupt_flag = 1;             // set timeout flag
    }
    
    // PORTB works 80 % of time. However, it may corrupt the autopilot if 
    // someone spams buttons or mismatch of timing/buttons
    if( INTCONbits.RBIF && INTCONbits.RBIE == 1 ){ 
        curb = PORTB;                   //  read for resetting flag
        INTCONbits.RBIF = 0;            // reset flag
        __delay_us(500);                // Bruh
         if( (PORTDbits.RD0 ) && (PORTB & 0x10 ) && !(prevb & 0x10)){ // released rb4
            reset_flags();
            flagrb4 = 1;
        }
        else if((PORTCbits.RC0) &&  (PORTB & 0x20 ) && !(prevb & 0x20 )){ // released rb5
            reset_flags();
            flagrb5 = 1;
        }
        else if( (PORTBbits.RB0) &&  (PORTB & 0x40 ) && !(prevb & 0x40)){ // released rb6
            reset_flags();
            flagrb6 = 1;
        }
        else if( /*(PORTAbits.RA0) && */ (PORTB & 0x80 ) && !(prevb & 0x80  )){ // released rb7
            reset_flags();
            flagrb7 = 1;
        }
        prevb = curb;
    }
    
}


void main(void) {
    init_ports();           // initialize ports
    init_adcon();           // initialize ADC
    init_serial();          // initialize serial communication
    init_timer();           // initialize timer
    init_interrupts();      // initialize interrupts
   
    while(!end_sys){        // while end command not received
        packet_task();
        parse_received_packet();
        manager();
    }
    return;
}