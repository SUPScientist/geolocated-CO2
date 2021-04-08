// AN-126 Demo of K-30 

/*
 K-Series sensor
 
 Original by Jason Berger
 Co2meter.com
 
 Modified by Phil Bresnahan to include datalogging and GPS
 UNCW
*/

#include <SPI.h>
#include <SD.h>
#include "wiring_private.h" // pinPeripheral() function and Uart Serial2

// SD card settings
#define chipSelect 4 
File myFile;

// K30 communications
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25}; //Command packet to read Co2 (see app note)
byte response[] = {0,0,0,0,0,0,0}; //create an array to store the response

//multiplier for value. default is 1. set to 3 for K-30 3% and 10 for K-33 ICB
int valMultiplier = 1;

// Add a second hardware UART
// https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}


void setup() {
  Serial.begin(9600); //Opens the main serial port to communicate with the computer
  Serial2.begin(9600); //Opens the virtual serial port with a baud of 9600
  
  delay(2000); // give serial monitor time to open and establish comms.
  Serial.println(" Demo of AN-126 Software Serial and K-40 Sensor");

  // Start SD stuff
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // Assign pins 10 & 11 SERCOM functionality for 2nd UART
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

}

void loop() {
  Serial.print("Co2 ppm = ");

  // Poll sensor: UART K-30 comms
  sendRequest(readCO2);
  unsigned long valCO2 = getValue(response);
  Serial.print(valCO2);
  Serial.print(", seconds elapsed = ");
  Serial.println(millis()/1000);
  
  // Open the file: SPI SD comms
  File dataFile = SD.open("K30_0000.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(millis()/1000);
    dataFile.print(",");
    dataFile.println(valCO2);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  delay(2000);
}

void sendRequest(byte packet[]){
  while(!Serial2.available()) { //keep sending request until we start to get a response  
//    Serial.println("waiting for Software.serial port availability");
    Serial2.write(readCO2,7);
    delay(50); 
  }
  
  int timeout=0; //set a timeout counter
  
  while(Serial2.available() < 7 ){ //Wait to get a 7 byte response
    timeout++;
    
    if(timeout > 10){ //if it takes too long there was probably an error
      while(Serial2.available()) //flush whatever we have
      Serial2.read();
      break; //exit and try again
    }
    
    delay(50);
  }
  
  for (int i=0; i < 7; i++)  {
    response[i] = Serial2.read();
  }
}

unsigned long getValue(byte packet[]){
  int high = packet[3]; //high byte for value is 4th byte in packet in the packet
  int low = packet[4]; //low byte for value is 5th byte in the packet
  unsigned long val = high*256 + low; //Combine high byte and low byte with this formula to get value
  return val* valMultiplier;
}

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}
