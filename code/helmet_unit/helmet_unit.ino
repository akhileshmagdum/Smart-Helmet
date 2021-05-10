// HELMET UNIT 
// Tx
// NRF: CE= 7; SCK= 13; MISO= 12; CSN= 8; MOSI= 11;
// DHT = 2;
// MQ3 =A0;
// BLUE: TX =3; RX =4; 
// SOS = 10;

#include <SoftwareSerial.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
#define DHTPIN 2                // Selecting the pin at which we have connected DHT11
#define DHTTYPE DHT11           // Selecting the type of DHT sensors
#define pot_pin A0              // Variable pin of MQ3 is to be connected to analog pin A0
#define sos_button 10           // Selecting 10 pin as SOS button pin   
  DHT dht ( DHTPIN, DHTTYPE ) ;
  RF24 radio(7, 8);             // CE, CSN
  SoftwareSerial EEBlue(4,3);   // RX | TX
  const byte address[6] = "00001";
  
void setup() {
  Serial.begin(9600);
  EEBlue.begin(9600);           // Default Band for comm
  Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  
  pinMode(pot_pin,INPUT);       // Setting A0 (POT pin) as input
  pinMode(sos_button,INPUT);    // Setting 10 as input
  
  radio.begin();                // NRF begin
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
void task(int value);
void temp_check(float t, float h);
void loop() {
  if (EEBlue.available()){                   // Feed any data from bluetooth to Terminal.
    Serial.write(EEBlue.read());
    int check =0;
    int potpin = analogRead(pot_pin);        // Reading analog value at pin A0 and storing in varible 'potpin'
    float humi = dht.readHumidity ( ) ;      // Declaring humi a variable and storing the humidity in it.
    float temp = dht.readTemperature ( ) ;   // Declaring temp a variable and storing the temperature in it.
    Serial.print ( " Temp is " ) ;  
    Serial.print ( temp ) ;                  // Printing the temperature on display.
    Serial.println ( " *C " ) ;              // Printing “ *C ”  on display.
    Serial.print ( " Humidity in % is : " ) ;                    
    Serial.print ( humi ) ;                  // Printing the humidity on display.
    Serial.print ( " % \t " ) ;              // Printing “%” on display.
  
    void task(int potpin );                  // Recalling task function. 
    const char test=("alcohol check; TEMP:%f; HUMI:%f ",temp,humi);
    Serial.print(test);
    delay(1000);
  
    const char text[] = "WLCOME!! Ready to go";// NRF transmitting data
    Serial.println("tx data");
    Serial.print(text);
    radio.write(&text, sizeof(text));
    delay(1000);
  
    void temp_check(float temp,float humi);   // Recalling temp_check function  
    delay(1000);
   }

  if (digitalRead(sos_button)==HIGH){        // Manually Sending Emergency SMS
    EEBlue.println("SOS...Send Emergency SMS ");
   }
   
  if (radio.available()) {                  // NRF receiving mode
    char text[32] = "";
    radio.read(&text, sizeof(text));        // NRF receiving data
    delay(1000);
    Serial.println(text);
    Serial.print("\n rx data\n");
    if (char text="FALL DETECTED"){
    EEBlue.println("SOS.....Sending Emergency SMS "); // Sending Emergency SMS
  }}
}

void task(int value){
  int data = map( value , 0 , 1023 , 0 , 255 );    // Convering the 10 bit value to 8 bit 
  // Feed data from bluetooth to termial 
  EEBlue.write(data);
  if(0<data<12)                                    // No alcohol detected
  {
        Serial.print("Good to Go");
        EEBlue.println("Good to Go");
  }
  else if(12<data<16)                              // Some amount alcohol detected
  {
        Serial.print("Are you sure to drive? Coz you seem to be little drunk ");
        EEBlue.println("Are you sure to drive? Coz you seem to be little drunk ");

  }
   else if(16<data)                                // More amount of alcohol detected
  {
        Serial.print("Sorry we can not allow you to drive");
        EEBlue.println("Sorry we can not allow you to drive");
        const char text2[] = "ALCOHOL ALERT IGNITION OFFF"; // NRF transmitting data
        Serial.println("tx stop data ");
        Serial.print(text2);
        radio.write(&text2, sizeof(text2));
        delay(1000);

  }
  EEBlue.println(data);
  Serial.println(data);
  delay(1000);
}

void temp_check(float t,float h)
{
  if( t < 40 || h <40){
    const char text3[] = "Alert!! High Temperature";// NRF transmitting data
    Serial.println("tx stop data ");
    Serial.print(text3);
    radio.write(&text3, sizeof(text3));
    delay(1000);
    }
}
