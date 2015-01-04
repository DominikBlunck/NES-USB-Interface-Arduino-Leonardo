/*
NES Controller USB Interface for the Arduino Leonardo

Author:      Dominik Blunck
Version:     1.0
Changelog:   With this Program you can use one NES Controller as USB-Device
*/

/* PIN Belegung */

int latch = 2; // set the latch pin
int clock = 3; // set the clock pin
int datin = 4;// set the data in pin
byte controller_data = 0;
JoyState_t joySt;

/* SETUP */
void setup()
{
  //Joystick Tasten vorbereiten
  joySt.xAxis = 127;
  joySt.yAxis = 127;
  joySt.xRotAxis = 0;
  joySt.yRotAxis = 0;
  joySt.throttle = 0;
  joySt.rudder = 0;
  joySt.hatSw1 = 0;
  joySt.hatSw2 = 0;
  joySt.buttons = 0;
  
  
  //Keyboard.begin();
  pinMode(latch,OUTPUT);
  pinMode(clock,OUTPUT);
  pinMode(datin,INPUT);
  
  //setze latch & clock auf High
  digitalWrite(latch,HIGH);
  digitalWrite(clock,HIGH);
}

/* Controller auslesen */
void controllerRead()
{
  //Controller Data auf 0 setzen
  controller_data = 0; 
  
  //latch & clock auf LOW setzen
  digitalWrite(latch,LOW);
  digitalWrite(clock,LOW);
  
  //latch auf HIGH setzen
  digitalWrite(latch,HIGH);

  //2 microsekunden warten
  delayMicroseconds(2);
  
  //latch auf LOW
  digitalWrite(latch,LOW);
  
  //daten vom Controller lesen
  controller_data = digitalRead(datin);
  
  //8 Bit vom Schieberegister lesen
  for(int i = 1; i <= 7; i++)
  {
    //clock auf High
    digitalWrite(clock,HIGH);
    
    //2 microsekunden warten
    delayMicroseconds(2);
    
    //Daten schreiben
    controller_data = controller_data << 1;
    controller_data = controller_data + digitalRead(datin);
    
    //4 microsekunden warten
    delayMicroseconds(4);
    
    //clock auf LOW setzen
    digitalWrite(clock,LOW);
  }
}

/* Tasten wieder freigeben */
void releaseKey()
{
  joySt.buttons = joySt.buttons & 254;
  joySt.buttons = joySt.buttons & 253;
  joySt.buttons = joySt.buttons & 252;  
  joySt.buttons = joySt.buttons & 251;
  joySt.buttons = joySt.buttons & 247;
  
  //Y-Achse auf null
  joySt.yAxis = 127;
  joySt.xAxis = 127;
}
  
  
/* Main */
void loop()
{
  //Controller auslesen
  controllerRead();
  
  //Keyboard simulieren
  //UP = 11110111 bit3
  //DOWN=11111011 bit2
  //LEFT=11111101 bit1
  //RIGHT=11111110 bit0
  //SELECT=11011111 bit5
  //START=11101111 bit4
  //A=01111111 bit7
  //B=10111111 bit6

  //UP
  if((controller_data & B00001000) == 0)
  {
    joySt.yAxis = 0;
  }
  
  //DOWN
  if((controller_data & B00000100) == 0)
  {
    joySt.yAxis = 255;
  }
  
  //LEFT
  if((controller_data & B00000010) == 0)
  {
    joySt.xAxis = 0;
  }
  
  //RIGHT
  if((controller_data & B00000001) == 0)
  {
    joySt.xAxis = 255;
  }
  
  //SELECT
  if((controller_data & B00100000) == 0)
  {
    joySt.buttons = joySt.buttons | 1;
  }
  
  //START
  if((controller_data & B00010000) == 0)
  {
    joySt.buttons = joySt.buttons | 2;
  }
  
  //A
  if((controller_data & B10000000) == 0)
  {
    joySt.buttons = joySt.buttons | 4;
  }
  
  //B
  if((controller_data & B01000000) == 0)
  {
    joySt.buttons = joySt.buttons | 8;
  }
  
  
  //Tastenkombinationen
  //A + B
  if(controller_data == (B01111111 & B10111111))
  {
    joySt.buttons = joySt.buttons | 4;
    joySt.buttons = joySt.buttons | 8;
    
  }
  
  //RIGHT + B
  if(controller_data == (B11111110 & B10111111))
  {
    joySt.buttons = joySt.buttons | 8;
    joySt.xAxis = 255;   
  }
  
  //LEFT + A
  if(controller_data == (B11111101 & B01111111))
  {
    joySt.buttons = joySt.buttons | 4;
    joySt.xAxis = 0; 
  }
  
  //RIGHT + A
  if(controller_data == (B11111110 & B01111111))
  {
    joySt.buttons = joySt.buttons | 4;
    joySt.xAxis = 255; 
  }
  
  //LEFT + B  
  if(controller_data == (B11111101 & B10111111))
  {
    joySt.buttons = joySt.buttons | 8;
    joySt.xAxis = 0;
  }
  
  //RIGHT + B + A
  if(controller_data == (B11111110 & B10111111 & B01111111))
  {
    joySt.buttons = joySt.buttons | 4;
    joySt.buttons = joySt.buttons | 8;    
    joySt.xAxis = 255; 
  }
    
  
  // Send joystick state
  Joystick.setState(&joySt);
  
  //Tasten wieder freigeben
  releaseKey();
}
  
