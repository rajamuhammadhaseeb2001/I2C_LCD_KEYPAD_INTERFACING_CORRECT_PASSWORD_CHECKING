#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the I2C address for the PCF8574
#define PCF8574_ADDRESS 0x20  // Update this address if your PCF8574 has a different one

// Define the number of rows and columns on the keypad
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char pass[100];
char correctpass[] = "1290";
int i=0;

// Define the keymap (the layout of the keys)
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Define the PCF8574 pins connected to the rows and columns of the keypad
byte rowPins[ROWS] = {0, 1, 2, 3};    // P0 to P3 on the PCF8574 connected to rows
byte colPins[COLS] = {4, 5, 6};       // P4 to P6 on the PCF8574 connected to columns

// Create an instance of the LCD display
LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust the address and size to match your LCD

void setup() {
  //Serial.begin(115200);  // Initialize serial communication at 115200 baud
  Wire.begin();          // Initialize I2C communication

  // Initialize the LCD display
  lcd.init();            // Use init() instead of begin() if needed
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CHECKING PASSWORD");
  delay(2000);
  lcd.clear();
  delay(100);
  //lcd.setCursor(0, 0);
  //lcd.print("ENTER PASSWORD");
  
  // Set all pins on PCF8574 to HIGH (inactive)
  writePCF8574(0xFF);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("ENTER PASSWORD:");
  char key = getKey();
  if (key)
  {
    if ((key!='#') && (key!='*'))
    {
    if(i<4)
    {
      pass[i]=key;
      lcd.setCursor(i, 1);
      lcd.print(pass[i]);
      i++;
    }
    pass[i] = '\0';
    //Serial.print(pass);
    }
  }
  if (key == '#')
  {
  if ((i >= 4) && strcmp(pass, correctpass) != 0) {
    lcd.clear();
    lcd.setCursor(2, 2);
    lcd.print("WRONG PASSWORD");
    i = 0;
    memset(pass, 0, sizeof(pass));  // Reset the pass array
    //lcd.setCursor(0, 2);
    //lcd.print("                    ");  // Clear the input row
    //lcd.setCursor(0, 3);
    //lcd.print("                    ");  // Clear the message row
    delay(3000);
    lcd.clear();
  }
  else if(strcmp(pass, correctpass) == 0) {
    lcd.clear();
    lcd.setCursor(2, 2);
    lcd.print("CORRECT PASSWORD");   
    //Serial.print("hello");
    i=0;
    memset(pass, 0, sizeof(pass)); //it sets all the elements in the array as 0 
    //lcd.setCursor(0, 3);         // Move cursor to the second row
    //lcd.print("                   ");  // Print 20 spaces to clear the second row
    //lcd.setCursor(0, 2);         // Move cursor to the second row
    //lcd.print("                   ");
    //pass[1] = 0; this is used to false the above strcmp check
    delay(3000);
    lcd.clear();
  }
  }
  if(key == '*')
  {
    if (i>0)
    {
      i--;
      pass[i]='\0';
      lcd.setCursor(i,1);
      lcd.print(' '); // Erase the last character on the display
      lcd.setCursor(i, 1); // Move the cursor back to the erased position
    }
  }
}

// Function to write data to the PCF8574
void writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  Wire.endTransmission();
}

// Function to read a key press from the keypad
char getKey() {
  for (byte r = 0; r < ROWS; r++) {
    writePCF8574(~(1 << rowPins[r]));
    byte colValue = readPCF8574() & 0x7F; // Ensure this masking is correct
    for (byte c = 0; c < COLS; c++) {
      if (!(colValue & (1 << colPins[c]))) {
        delay(500);
        return keys[r][c];
      }
    }
  }
  return '\0';
}

// Function to read data from the PCF8574
byte readPCF8574() {
  Wire.requestFrom(PCF8574_ADDRESS, (uint8_t)1);
  return Wire.read();
}