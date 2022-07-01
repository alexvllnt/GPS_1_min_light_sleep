/* ---- Declaration of librairies ---- */
#include <TinyGPS++.h>
#include "boards.h"

/* ---- Declaration of constants ---- */
/*
    In this sheet, we use two constants :
    - uS_TO_S_FACTOR, a conversion factor for micro seconds to seconds.
    - TIME_TO_SLEEP, the time during the ESP32 will go to sleep (in seconds)
*/
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  58

/* ---- Declaration of variables ---- */
/*
    In this sheet, we use one variable :
   - message_gps, the string will receive the GPS position of the ESP32.
*/
String message_gps = "";

/* ---- Procedure for sending UBX message ---- */
void sendUBX(uint8_t *MSG, uint8_t len) {
  for (int i = 0; i < len; i++) {
    Serial1.write(MSG[i]);
  }
}

/* ---- Procedure for configurating the ESP32 ---- */
void configurar_tramas() {
  //Desactivate trames
  uint8_t setGSA[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};//trame GSA
  sendUBX(setGSA, sizeof(setGSA) / sizeof(uint8_t));
  uint8_t setGLL[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B};//trame GLL
  sendUBX(setGLL, sizeof(setGLL) / sizeof(uint8_t));
  uint8_t setGSV[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};//trame GSV
  sendUBX(setGSV, sizeof(setGSV) / sizeof(uint8_t));
  uint8_t setRMC[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40};//trame RMC
  sendUBX(setRMC, sizeof(setRMC) / sizeof(uint8_t));
  uint8_t setVTG[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};//trame VTG
  sendUBX(setVTG, sizeof(setRMC) / sizeof(uint8_t));
  uint8_t setINF[] = {0xB5, 0x62, 0x06, 0x02, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xF0};//trame TXT
  sendUBX(setINF, sizeof(setINF) / sizeof(uint8_t));

  uint8_t setRXM[] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x04, 0x25, 0x95}; //Activate the Eco Mode or the Power Save Mode
  sendUBX(setRXM, sizeof(setRXM) / sizeof(uint8_t));
  /* 
   {0xB5, 0x62, //SYNC CHAR 1, SYNC CHAR 2
   0x06, 0x11, //ID, CLASS
   0x02, 0x00, //LENGTH
   0x08, //RESERVED 1
   0x04, //LP MODE (ECO MODE)
   0x25, 0x95} //ACK_A, ACK_B
   */
}

/* ---- Procedure for capturing the GPS position of the ESP32 ---- */
void capturar_datos() {
  message_gps = message_gps + (char)Serial1.read();
}

/* ---- Procedure for printing the GPS position of the ESP32 ---- */
void mostrar_trama(String str) {
  Serial.print(str);
  message_gps = "";
}

/* ---- Setup part ---- */
void setup() {
  initBoard();
  //When the power is turned on, a delay is required.
  delay(1500);
  configurar_tramas();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //Activate the Timer Wakeup reason for 58 seconds
}

/* ---- Loop part ---- */
void loop() {
  while (Serial1.available() > 0)
    capturar_datos();
  mostrar_trama(message_gps);
  delay(2000);
  esp_light_sleep_start(); //Begin of the light sleep
}
