// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Adafruit_GPS.h> // Initialize GPS 
Adafruit_GPS GPS(&Serial);




char buff [255];
volatile byte indx;
volatile boolean process;
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int buzzer = 9; //buzzer to arduino pin 9
void setup() {
   Serial.begin (115200);
Serial1.begin(115200); // Debugging serial 
GPS.begin(9600); // GPS baud rate 
GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate delay(1000);


   pinMode(MISO, OUTPUT); // have to send on master in so it set as output
   SPCR |= _BV(SPE); // turn on SPI in slave mode
   indx = 0; // buffer empty
   process = false;
   SPI.attachInterrupt(); // turn on interrupt
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  pinMode(buzzer, OUTPUT);
  pinMode(8, OUTPUT);
    pinMode(10, OUTPUT);
}


void turnOnGreen(){
  digitalWrite(8,HIGH);
}


ISR (SPI_STC_vect) // SPI interrupt routine
{
   byte c = SPDR; // read byte from SPI Data Register
   
   if (indx < sizeof(buff)) {
      buff[indx++] = c; // save data in the next index in the array buff
      if (c == '\n') {
        buff[indx - 1] = 0; // replace newline ('\n') with end of string (0)
        process = true;
      }
   }  
}
int value;
void loop() {
 
 if (process) {
      process = false; //reset the process
     if(buff[0] == 'M'){
       lcd.clear();
       noTone(buzzer);
digitalWrite(8,0);
      Serial.print (buff[23]); //print the array on serial monitor
      Serial.print(buff[24]); //print the array on serial monitor
      Serial.print (buff[25]); //print the array on serial monitor
      Serial.println(buff[26]); //print the array on serial monitor
    if(buff[23]=='-'){
    value = buff[24] - '0';
    value = value * 100;
    value = value + (buff[25] - '0') * 10;
    value = value + (buff[26] - '0');
    value = value * -1;
    }
    else{
        value = buff[24] - '0';
    value = value * 100;
    value = value + (buff[25] - '0') * 10;
    value = value + (buff[26] - '0');
    }
        Serial.print("This is the value: ");
       Serial.println( value); //print the array on serial monitor
       Serial.println(buff);
      }
      indx= 0; //reset button to zero
  if(value < 200 && value > 100){
    lcd.print("Boat is Stable");
   // turnOnGreen();
  }
  else if(value < -200 && value > -400){
    lcd.print("Tilting Right!!");
       // Serial.println("right");


   digitalWrite(8,1);


  }
  else if(value < -400){
    lcd.print("Ship Sinking");


     for(int i = 0; i <10; i++ ){
    digitalWrite(8,1);
    delay(20);
    digitalWrite(8,0);
    if(i ==4){
      tone(buzzer,1000);
    }
    }
  }
  else if(value < 100 && value > -200 && value != 0){
   
    lcd.print("Tilting Left!!");
        Serial.println(value);


   digitalWrite(8,1);
   // Serial.println("left");
  }
  else if(value < -200){
    lcd.print("Ship Sinking");


    for(int i = 0; i <10; i++ ){
    digitalWrite(8,1);
    delay(20);
    digitalWrite(8,0);
      if(i ==4){
      tone(buzzer,1000);
    }
    }
  }
     value = 0;


// GPS data display 
if (GPS.fix) { 
lcd.setCursor(0, 1); 
lcd.print("Lat:"); 
lcd.print(GPS.latitude, 4); 
lcd.setCursor(8, 1); 
lcd.print("Lon:"); 
lcd.print(GPS.longitude, 4); }
 }
}
