// BIKE UNIT 
// Rx
// 1602 LCD: V0= 6; RS= 10; E=9; D4=5; D5= 4; D6= 3; D7= 2;
// 1602 LCD: VSS, RW, K= GND; VDD, A=5V
// NRF: CE= 7; SCK= 13; MISO= 12; CSN= 8; MOSI= 11;
// ADXL335: X-OUT= A0; Y-OUT=A1; Z-OUT=A2;
// ignition: pin= 1; 

#include <LiquidCrystal.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <math.h>
#define stop_pin 1 
const int x_out = A1;     // connect x_out of module to A1  
const int y_out = A2;     // connect y_out of module to A2 
const int z_out = A3;     // connect z_out of module to A3  
RF24 radio(7, 8);         // CE, CSN
const byte address[6] = "00001";
LiquidCrystal lcd(10, 9, 5, 4, 3, 2); // REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN
int Contrast=75;                      // CONSTANT FOR V0 PIN

void setup() {
  Serial.begin(9600);
  radio.begin();                     // NRF begin
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  analogWrite(6,Contrast);
  lcd.begin(16, 2);
}

void displaylcd(char text);
void crash(float pitch, float roll);

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text)); // NRF receiving data
    delay(1000);
    //Serial.print("rx data");
    Serial.println(text);
    Serial.print("\nrx data\n");
    void displaylcd(char text);

      if(text=="ALCOHOL ALERT IGNITION OFFF"){ // Stop ignition incase of alcohol detected 
       digitalWrite(stop_pin,HIGH);
       Serial.print("Engine stop");
     }
  
    int x_adc_value, y_adc_value, z_adc_value; 
   double x_g_value, y_g_value, z_g_value;
   double roll, pitch;
   x_adc_value = analogRead(x_out); // Digital value of voltage on x_out pin  
   y_adc_value = analogRead(y_out); // Digital value of voltage on y_out pin  
   z_adc_value = analogRead(z_out); // Digital value of voltage on z_out pin  
   Serial.print("x = ");
   Serial.print(x_adc_value);
   Serial.print("\t\t");
   Serial.print("y = ");
   Serial.print(y_adc_value);
   Serial.print("\t\t");
   Serial.print("z = ");
   Serial.print(z_adc_value);
   Serial.print("\t\t");
   delay(100);
   x_g_value = ( ( ( (double)(x_adc_value * 5)/1024) - 1.65 ) / 0.330 ); // Acceleration in x-direction in g 
   y_g_value = ( ( ( (double)(y_adc_value * 5)/1024) - 1.65 ) / 0.330 ); // Acceleration in y-direction in g   
   z_g_value = ( ( ( (double)(z_adc_value * 5)/1024) - 1.80 ) / 0.330 ); // Acceleration in z-direction in g  
   roll = ( ( (atan2(y_g_value,z_g_value) * 180) / 3.14 ) + 180 );       // Formula for roll 
   pitch = ( ( (atan2(z_g_value,x_g_value) * 180) / 3.14 ) + 180 );      // Formula for pitch 
   Serial.print("Roll = ");
   Serial.print(roll);
   Serial.print("\t");
   Serial.print("Pitch = ");
   Serial.print(pitch);
   Serial.print("\n\n");
   delay(1000);
   }
   else{                                    // In case NRF dont get signal
    Serial.print("Helmet not detected\n"); 
    const char msg[]=" WEAR HELMET ";       // Wear helmet  
    void displaylcd(char msg);
   }
}
void displaylcd(char text)  {      // Display receiving data
    lcd.setCursor(0, 0);     
    lcd.print(text);
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print(text);
  }
void crash(float pitch, float roll)  {
    if (150 < pitch <350 || 150 < roll <350 )//condition for fall detected
    {
      const char text[] = "FALL DETECTED"; // NRF transmitting data
      Serial.println("tx data");
      Serial.print(text);
      radio.write(&text, sizeof(text));
      delay(1000);
      }
}
