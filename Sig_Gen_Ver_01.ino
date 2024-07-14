/*
 * Signal Generator Arduino Code
 * Copyright Dick Whipple 2022
 * Version 1.0
 * 5 October 2022
*/
//
// AD0850 pin definitions
//
#define W_CLK 8                       // AD9850 clocking pin
#define FQ_UD 7                       // AD9850 frequency update pin
#define DATA  6                       // AD9850 data pin
#define RESET 5                       // AD9850 reset pin
#define ModPB 2                       // Modulation push button input port
#define FreqPB 4                      // Frequency push buttton input port  
#define Freq_1s 10                    // Frequency 1s digit output port
#define Freq_2s 11                    // Frequency 2s digit output port   
#define Freq_4s 12                    // Frequency 4s digit output port       
#define Mod_1s 13                     // Modulation 1s digit output port
#define PWMout 3                      // Pulse Width Modulation output port 
#define Freq_Options  6               // Number of frequency options exclude 000
#define Mod_Options 1                 // Number of modulation options less 1
#define PWM_Off 0                     // PWM off
#define PWM_On 127                    // PWM on with 50% duty cycle
#define AD9850_CLOCK 125000000        // Module crystal frequency

//
// Create system variables
//

long unsigned int freq;
int freqCounter;
int AMmode;

//
// AD9850 communication code
//

#define pulseHigh(pin); {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

//
// AD9850 byte transfe code
//

void tfr_byte(byte data) {
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

//
// AD9850 set frequency code
//

void sendFrequency(double frequency) {
  int32_t freq1 = frequency * 4294967295/AD9850_CLOCK;  // note 125 MHz clock on 9850
  for (int b = 0; b < 4; b++, freq1 >>= 8) {
    tfr_byte(freq1 & 0xFF);
  }
  tfr_byte(0x000);                     // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);                    // Done!  Should see output
}

void setup(){

  //
  // Configure output pins for AD9850 module.
  //
  
  pinMode(FQ_UD, OUTPUT);              
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);

  //
  // Configure output pins for LEDs
  //
  
  pinMode(Freq_1s, OUTPUT); 
  pinMode(Freq_2s, OUTPUT);
  pinMode(Freq_4s, OUTPUT);
  pinMode(Mod_1s, OUTPUT);

  //
  // Configure input pins for push buttons
  //
  
  pinMode(ModPB, INPUT_PULLUP);
  pinMode(FreqPB, INPUT_PULLUP);
  
  //
  // Initialise AD9850 module.
  // 
  
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);    // Enable serial mode  
  delay(100);
  
  //
  // Initial modulation mode off
  //
  
  AMmode = 0;
  analogWrite(PWMout,0);
  
  //
  // Initial frequency mode off
  //
  
  freqCounter = 0;
  digitalWrite(Freq_1s,HIGH);
  digitalWrite(Freq_2s,HIGH);
  digitalWrite(Freq_4s,HIGH);
  digitalWrite(Mod_1s,HIGH);
 
}
 
void loop(){

  //
  // Frequency set loop
  //
  
  if (digitalRead(FreqPB) == LOW) {  
     freqCounter += 1;
     if (freqCounter > Freq_Options) {
     freqCounter = 0;
     }
     switch (freqCounter) {
      case 0: pulseHigh(RESET);
            digitalWrite(Freq_1s,HIGH);
            digitalWrite(Freq_2s,HIGH);
            digitalWrite(Freq_4s,HIGH);
            break;
      case 1: freq = 455000;
            digitalWrite(Freq_1s,LOW);
            digitalWrite(Freq_2s,HIGH);
            digitalWrite(Freq_4s,HIGH);
            break;
      case 2: freq = 550000;
            digitalWrite(Freq_1s,HIGH);
            digitalWrite(Freq_2s,LOW);
            digitalWrite(Freq_4s,HIGH);
            break;
      case 3: freq = 600000;
            digitalWrite(Freq_1s,LOW);
            digitalWrite(Freq_2s,LOW);
            digitalWrite(Freq_4s,HIGH);
            break;
      case 4: freq = 1400000;
            digitalWrite(Freq_1s,HIGH);
            digitalWrite(Freq_2s,HIGH);
            digitalWrite(Freq_4s,LOW);
            break;
      case 5: freq = 1500000;
            digitalWrite(Freq_1s,LOW);
            digitalWrite(Freq_2s,HIGH);
            digitalWrite(Freq_4s,LOW);
            break;
      case 6: freq = 1620000;
            digitalWrite(Freq_1s,HIGH);
            digitalWrite(Freq_2s,LOW);
            digitalWrite(Freq_4s,LOW);
            break;
      }
     sendFrequency(freq);
     delay(350);
  }

  //
  // Modulation set loop
  //
  
  if (digitalRead(ModPB) == LOW) {  
     AMmode += 1;
     if (AMmode > Mod_Options) {
     AMmode = 0;
     }
     switch (AMmode) {
      case 0: analogWrite(PWMout,PWM_Off);
              digitalWrite(Mod_1s,HIGH);
              break;
      case 1: analogWrite(PWMout,PWM_On);
              digitalWrite(Mod_1s,LOW);
              break;
     }
     delay(350);
  }
}
