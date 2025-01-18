#include <LiquidCrystal_I2C.h>
#include<Wire.h>
LiquidCrystal_I2C lcd(0X27,16,2);
#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
#define close 0
#define open 1
const int stepsPerRevolution = 100;
Stepper myStepper = Stepper(stepsPerRevolution, 4, 5, 6, 7);

//pin initialisations for the RFID
#define SS_PIN 10
#define RST_PIN 9
//pin initialisations for the RELAY
#define RELAY 2


byte readCard[4];
int count1,count2=0;
int doorstate=0;
String tag_UID1 = "39CF712";  
String tag_UID2 = "63D61FC";
String tagID = "";
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
void setup()
{
  pinMode(RELAY, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
  digitalWrite(RELAY, LOW);    // turn the LED off by making the voltage LOW
  Serial.begin(115200);        // Initialize serial communications with the PC
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // Initialise MFRC522
  myStepper.setSpeed(100);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);

}
void loop()
{
  
  //Wait until new tag is available
     while (readID())
    {
      if (tagID == tag_UID1 || tagID == tag_UID2)
      {
        Serial.println(doorstate);
        if(tagID==tag_UID1)
        {
            count1++;
             if(count1==1 && doorstate==close)
            {
              Serial.println("DOOR OPENED");
              rotate_motor_open();
              digitalWrite(RELAY, HIGH); 
              lcd_write_open_clear();
              doorstate=open;
              
            }
            else if(count1>=2 && doorstate==open)
            {
              count1=0;
              Serial.println("DOOR CLOSED");
              rotate_motor_close();
              digitalWrite(RELAY, LOW); 
              lcd_write_close_clear();        
              doorstate=close;
        
            }
         }
        if(tagID==tag_UID2)
        {
            count2++;
             if(count2==1 && doorstate==close)
            {
              Serial.println("DOOR OPENED");
              rotate_motor_open();              
              digitalWrite(RELAY, HIGH); 
              lcd_write_open_clear();
              doorstate=open;
            }
            else if(count2>=2 && doorstate==open)
            {
              count2=0;
              Serial.println("DOOR CLOSED");
              rotate_motor_close();
              digitalWrite(RELAY, LOW);
              lcd_write_close_clear();
              doorstate=close; 
            }
         }
      }
      else 
      {
              Serial.println("USER IS NOT REGISTERED");
      }
    }
  }

  //Read new tag if available
  boolean readID()
  {
    //Check if a new tag is detected or not. If not return.
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return false;
    }
    //Check if a new tag is readable or not. If not return.
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return false;
    }
    tagID = "";
    // Read the 4 byte UID
    for ( uint8_t i = 0; i < 4; i++)
    {
      readCard[i] = mfrc522.uid.uidByte[i];
      tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Convert the UID to a single String
    }
    tagID.toUpperCase();
    Serial.println(tagID);
    mfrc522.PICC_HaltA(); // Stop reading
    return true;
  }
  
void rotate_motor_open() {
    for(int i=0;i<20;i++)
    {
       print_dots();            
       myStepper.step(stepsPerRevolution); // Rotate 90 degrees (for example)
    }

}
//note: bug in library is solved by reversing connection of pin 2 and pin 3 in uln2003 driver.
void rotate_motor_close() {
     for(int i=0;i<20;i++)
    {
       print_dots();            
       myStepper.step(-stepsPerRevolution); // Rotate back 90 degrees to close
    }
}
void lcd_write_open_clear()
{
    lcd.clear();
    lcd.print("DOOR OPENED");              
}
void lcd_write_close_clear()
{
    lcd.clear();
    lcd.print("DOOR CLOSED");                         
}
void print_dots()
{
      String message = "...........";
      lcd.clear();
      lcd.print(message);  // Print from current position
}
  
