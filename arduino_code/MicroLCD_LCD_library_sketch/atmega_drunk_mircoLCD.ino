#include <Arduino.h>
#include <Wire.h>
#include <MicroLCD.h>

LCD_SSD1306 lcd; /* for SSD1306 OLED module */

int analogPin = 1;  // Alcohol sensor
int minReading; // Lowest number as base
int maxReading;  // Max reading 
int drunkLevel;  // Highest score
int loadingPercent; // Loading present number
int startSplash; // Start screen count up number
int firstRun = 1; // First run flag for splash screen
int debugMode; // Debug flag
int readMode = 0; // Reading sensor flag
int resetMode = 0; // Recalibrating flag
int adding = 500; // added to the minimum sensor to give max

// Delay values
unsigned long resetTime;
unsigned long readingSensorWait;
unsigned long splashWait;
unsigned long debugScreen;

// Reset Button stuff
int buttonReset = 7;
unsigned long timeReset = 0;
unsigned long debounceReset = 200;
int readingReset;
int previousReset;

// Sensor Button stuff
int buttonSensor = 8;
unsigned long timeSensor = 0;
unsigned long debouneSensor = 200;
int readingSensor;
int previousSensor;

void splashScreen() {
  // Warm up code
  lcd.clear(); // Clear lcd screen
  for(loadingPercent = 0; loadingPercent < 101; loadingPercent++) {
    lcd.setCursor(20, 1); // Set cursor postion
    lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
    lcd.print("Warming up "); // Print to screen
    
    if (loadingPercent < 10) { // If its a single digit number the centring needs to be different
      lcd.clear(128,30,128,30); // Clears the screen
      lcd.setCursor(55, 3); // Set cursor postion
      lcd.print(loadingPercent); // Print to screen
      lcd.print("%"); // Print to screen
    }
    
    else if (loadingPercent >= 10 && loadingPercent <= 99) { // If its a double digit number the centring needs to be different
      lcd.clear(128,30,128,30); // Clears the screen
      lcd.setCursor(50, 3); // Set cursor postion
      //lcd.clear(); 
      lcd.print(loadingPercent); // Print to screen
      lcd.print("%"); // Print to screen
    }
    
    else if (loadingPercent == 100) { // If its a triple digit number the centring needs to be different
      lcd.clear(); // Clears the screen
    }
    delay(600); //Dirty delay, god I hate these things
  }
  
  int sensor = analogRead(analogPin); // Reads the analogPin
  minReading = sensor; // Sets the minReading to the curren sensor reading
  maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
  startSplash = 5; // Sets startSplash to 5 so it will skip the countdown when done
  firstRun = 0; // Sets firstRun to 0 so it wouldnt run the warmup code again
}

void setup()   {
  Serial.begin(9600); // Starts serialA
  Serial.println("Starting");
  lcd.begin(); // Starts the lcd 
  pinMode(debouneSensor, INPUT);  // Set button input
  digitalWrite(buttonReset, HIGH);  // Set internal pull up resistor
  pinMode(buttonSensor, INPUT);  // Set button input
  digitalWrite(buttonSensor, HIGH);  // Set internal pull up resistor
}
  
void loop() {
  int sensor = analogRead(analogPin); // Reads the analogPin
  while (startSplash < 5) {
    
    readingSensor = digitalRead(buttonSensor);
    if (readingSensor == LOW && previousSensor == LOW && millis() - timeSensor > debouneSensor) { 
      lcd.clear();
      startSplash = 5;
      firstRun = 0;
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
      timeSensor = millis(); 
    }
    previousSensor = readingSensor;
    
  readingReset = digitalRead(buttonReset);
    if (readingReset == LOW && previousReset == LOW && millis() - timeReset > debounceReset) { 
      debugMode = 1; // Sets debug flag
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
      timeSensor = millis();
      }
      previousReset = readingReset;

    while (millis() >= splashWait){
      lcd.setCursor(5, 1); // Set cursor postion
      lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
      lcd.println("#Atmega_drunk"); // Print to screen
      startSplash += 1; // Adds 1 onto startSplash
      splashWait += 1000;
    }
  }
  // Runs splashscreen
  if (firstRun == 1) {
    splashScreen();
  }
  
  // -------------- Debound code button Reset start code--------------
  readingReset = digitalRead(buttonReset);

  if (readingReset == LOW && previousReset == HIGH && millis() - timeReset > debounceReset) { 

    lcd.clear(); // Clears the screen
    resetTime = millis() + 3000;
    resetMode = 1;
  }
  previousReset = readingReset;
   
  // -------------- Debound code button Reset end code --------------
  
  // -------------- Debound code button Sensor start code--------------
  
  readingSensor = digitalRead(buttonSensor); // Reads the analogPin

  if (readingSensor == LOW && previousSensor == LOW && millis() - timeSensor > debouneSensor) { 
    lcd.clear(); // Clears the screen
    readingSensorWait = millis() + 3000;
    timeReset = millis();
    timeSensor = millis(); 
    readMode = 1;
  }
  previousSensor = readingSensor;
   
  // -------------- Debound code button Sensor end code --------------
  
    if (readMode == 1) {
      while(millis() <= readingSensorWait) {
        int sensor = analogRead(analogPin); // Reads the analogPin
        drunkLevel = map(sensor, minReading, maxReading,0, 10);
        lcd.setCursor(30, 2); // Set cursor postion
        lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
        lcd.println("Blow now"); // Print to screen
    }
    readMode = 0;
    }
   
   if (resetMode == 1) {
    while(millis() <= resetTime){
      lcd.setCursor(5, 2); // Set cursor postion
      lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
      lcd.println("Recalibrating"); // Print to screen
      int sensor = analogRead(analogPin); // Reads the analogPin
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding;
    }
    resetMode = 0;
   } 

  if (debugMode == 0) {
    if (readMode == 0) {
      //int sensor = analogRead(analogPin); // Reads the analogPin
      lcd.setCursor(20, 0); // Set cursor postion
      lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
      lcd.print("How Drunk?"); // Print to screen

      if (drunkLevel < 10) { // If its a single digit number the centring needs to be different
        lcd.clear(128,20,128,20); // Clears the screen
        lcd.setCursor(60, 2); // Set cursor postion
        lcd.println(drunkLevel); // Print to screen
      }
        
      else if (drunkLevel >= 10) { // If its a double digit number the centring needs to be different
        lcd.clear(128,20,128,20); // Clears the screen
        lcd.setCursor(55, 2); // Set cursor postion
        lcd.println(drunkLevel); // Print to screen
      }
        
        lcd.setFontSize(FONT_SIZE_SMALL); // Set text size
        
      if (drunkLevel == 0) {
        lcd.setCursor(15, 4); // Set cursor postion
        lcd.print("Sober as a judge!"); // Print to screen
      }
        
      else if (drunkLevel == 1) {
        lcd.setCursor(45, 4); // Set cursor postion
        lcd.print("Are you"); // Print to screen
        lcd.setCursor(25, 5); // Set cursor postion
        lcd.print("even trying!!!"); // Print to screen
      }
        
      else if (drunkLevel == 2) {
        lcd.setCursor(30, 4); // Set cursor postion
        lcd.print("Can you feel"); // Print to screen
        lcd.setCursor(40, 5); // Set cursor postion
        lcd.print("the buzz"); // Print to screen
      }
        
      else if (drunkLevel == 3) {
        lcd.setCursor(17, 4); // Set cursor postion
        lcd.print("Now your talking"); // Print to screen
        lcd.setCursor(15, 5); // Set cursor postion
        lcd.print("get some music on"); // Print to screen
      }
        
      else if (drunkLevel == 4) {
        lcd.setCursor(20, 4); // Set cursor postion
        lcd.print("Now the party's"); // Print to screen
        lcd.setCursor(25, 5); // Set cursor postion
        lcd.print("getting going"); // Print to screen
      }
        
      else if (drunkLevel == 5) {
        lcd.setCursor(12, 4); // Set cursor postion
        lcd.print("You might want to"); // Print to screen
        lcd.setCursor(10, 5); // Set cursor postion
        lcd.print("start on the shots"); // Print to screen
      }
        
      else if (drunkLevel == 6) {
        lcd.setCursor(0, 4); // Set cursor postion
        lcd.print("Your well on your way"); // Print to screen
        lcd.setCursor(35, 5); // Set cursor postion
        lcd.print("keep it up"); // Print to screen
      }
        
      else if (drunkLevel == 7) {
        lcd.setCursor(25, 4); // Set cursor postion
        lcd.print("Your on a roll"); // Print to screen
        lcd.setCursor(15, 5); // Set cursor postion
        lcd.print("grab another drink"); // Print to screen
      }
        
      else if (drunkLevel == 8) {
        lcd.setCursor(15, 4); // Set cursor postion
        lcd.print("Pretty impressive"); // Print to screen
        lcd.setCursor(10, 5); // Set cursor postion
        lcd.print("have you puked yet?"); // Print to screen
      }
        
      else if (drunkLevel == 9) {
        lcd.setCursor(15, 4); // Set cursor postion
        lcd.print("The room will be"); // Print to screen
        lcd.setCursor(17, 5); // Set cursor postion
        lcd.print("spinning tonight"); // Print to screen
      }
        
      else if (drunkLevel == 10) {
        lcd.setCursor(25, 4); // Set cursor postion
        lcd.print("Your steaming"); // Print to screen
      }
        
      else if (drunkLevel > 10) {
        lcd.setCursor(25, 4); // Set cursor postion
        lcd.print("Off the chart"); // Print to screen
        lcd.setCursor(5, 5); // Set cursor postion
        lcd.print("Can you even see XXX"); // Print to screen
      }
    }
  }

  if (debugMode == 1) {
    lcd.setCursor(20, 0); // Set cursor postion
    lcd.setFontSize(FONT_SIZE_MEDIUM); // Set text size
    lcd.print("Debug Mode"); // Print to screen
    lcd.clear(128,20,128,20); // Clears the screen
    lcd.setFontSize(FONT_SIZE_SMALL); // Set text size
    lcd.setCursor(0, 3); // Set cursor postion
    lcd.print("Drunk Level "); // Print to screen
    lcd.print(drunkLevel); // Print to screen
    lcd.setCursor(0, 4); // Set cursor postion
    lcd.print("Sensor reading "); // Print to screen
    lcd.print(sensor); // Print to screen
    lcd.setCursor(0, 5); // Set cursor postion
    lcd.print("Max reading "); // Print to screen
    lcd.print(maxReading); // Print to screen
    lcd.setCursor(0, 6); // Set cursor postion
    lcd.print("Minumum reading "); // Print to screen
    lcd.print(minReading); // Print to screen
  }
}

