// OLED screen stuff
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

int analogPin = 1;  // Alcohol sensor
int minReading; // Stores the lowest number as base
int maxReading;  // Stores the new high
int drunkLevel;  // Highest score
int loadingPercent;
int startSplash;
int firstRun = 1;
int debugMode = 0;
int adding = 500;

// Delay values
unsigned long displayScreen;
unsigned long resetTime;
unsigned long readingSensorWait;
unsigned long splashWait;
unsigned long debugScreen;

// Reset Button stuff
int buttonReset = 7;
long timeReset = 0;
long debounceReset = 200;
int readingReset;
int previousReset;

// Sensor Button stuff
int buttonSensor = 8;
long timeSensor = 0;
long debouneSensor = 200;
int readingSensor;
int previousSensor;

void setup()   {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
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
      startSplash = 5;
      firstRun = 0;
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
      timeSensor = millis(); 
    }
    previousSensor = readingSensor;
    
    
  readingReset = digitalRead(buttonReset);
  if (readingReset == LOW && previousReset == LOW && millis() - timeReset > debounceReset) { 
      //Serial.println("reset");
      debugMode = 1; // Sets debug flag
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
      timeSensor = millis();
    }
    previousReset = readingReset;

    while (millis() >= splashWait){
      //Serial.println(startSplash);
      display.clearDisplay(); // Clears everything off the screen
      display.setTextSize(2); // Set text size
      display.setTextColor(WHITE); // Set text colour
      display.setCursor(25,0); // Set cursor postion
      display.println("#Atmega");
      display.setCursor(35,20); // Set cursor postion
      display.println("Drunk");
      //display.println("Press GREEN button\n to skip warmup\nPress RED button \nfor debug mode"); // Print to screen
      display.display(); // Writes to the screen
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
    //buttonReset
    resetTime = millis() + 2000;
    display.clearDisplay(); // Clears everything off the screen
    display.display(); // Writes to the screen
        
    timeReset = millis();
  }
  previousReset = readingReset;
   
  // -------------- Debound code button Reset end code --------------
  
  // -------------- Debound code button Sensor start code--------------
  
  readingSensor = digitalRead(buttonSensor); // Reads the analogPin

  if (readingSensor == LOW && previousSensor == LOW && millis() - timeSensor > debouneSensor) { 
    
    readingSensorWait = millis() + 3000;
    display.clearDisplay(); // Clears everything off the screen
    display.display(); // Writes to the screen
    
    timeSensor = millis(); 
  }
  previousSensor = readingSensor;
   
  // -------------- Debound code button Sensor end code --------------
  
    while(millis() <= readingSensorWait) {
      int sensor = analogRead(analogPin); // Reads the analogPin
      drunkLevel = map(sensor, minReading, maxReading,0, 10);
      display.clearDisplay(); // Clears everything off the screen
      display.setTextSize(2); // Set text size
      display.setTextColor(WHITE); // Set text colour
      /*display.setCursor(45,0); // Set cursor postion
      display.println("Blow"); // Print to screen
      display.setCursor(50,15); // Set cursor postion
      display.println("for"); // Print to screen
      display.setCursor(60,30); // Set cursor postion
      display.println("3"); // Print to screen
      display.setCursor(25,45); // Set cursor postion
      display.println("seconds"); // Print to screen*/
      display.setCursor(45,0); // Set cursor postion
      display.println("Blow"); // Print to screen
      display.setCursor(50,20); // Set cursor postion
      display.println("now"); // Print to screen
      display.display(); // Writes to the screen
    }
    
    while(millis() <= resetTime){
      display.clearDisplay(); // Clears everything off the screen
      display.setTextSize(2); // Set text size
      display.setTextColor(WHITE); // Set text colour
      display.setCursor(20,0); // Set cursor postion
      display.println("Reseting"); // Print to screen
      display.setCursor(30,20); // Set cursor postion
      display.println("sensor"); // Print to screen
      //display.println(minReading); // Print to screen
      int sensor = analogRead(analogPin); // Reads the analogPin
      minReading = sensor; // Sets the minReading to the curren sensor reading
      maxReading = sensor + adding;
      display.display(); // Writes to the screen
    }
    
  if (debugMode == 0) {
    if (millis() >= displayScreen){
      display.clearDisplay(); // Clears everything off the screen
      display.setTextSize(2); // Set text size
      display.setTextColor(WHITE); // Set text colour
      display.setCursor(5,0); // Set cursor postion
      display.print("How Drunk?"); // Print to screen
      
      if (drunkLevel < 10) { // If its a single digit number the centring needs to be different
        display.setCursor(60,20); // Set cursor postion
        display.println(drunkLevel); // Print to screen
      }
      
      else if (drunkLevel >= 10) { // If its a double digit number the centring needs to be different
        display.setCursor(50,20); // Set cursor postion
        display.println(drunkLevel); // Print to screen
      }
      
      //display.setTextColor(BLACK, WHITE); // Set text colour
      display.setTextSize(1); // Set text size
      
      if (drunkLevel == 0) {
        display.setCursor(10,40); // Set cursor postion
        display.print("Sober as a judge!"); // Print to screen
      }
      
      else if (drunkLevel == 1) {
        display.setCursor(45,40); // Set cursor postion
        display.print("Are you"); // Print to screen
        display.setCursor(35,50); // Set cursor postion
        display.print("even trying"); // Print to screen
      }
      
      else if (drunkLevel == 2) {
        display.setCursor(30,40); // Set cursor postion
        display.print("Can you feel"); // Print to screen
        display.setCursor(40,50); // Set cursor postion
        display.print("the buzz"); // Print to screen
      }
      
      else if (drunkLevel == 3) {
        display.setCursor(13,40); // Set cursor postion
        display.print("Now your talking"); // Print to screen
        display.setCursor(10,50); // Set cursor postion
        display.print("get some music on"); // Print to screen
      }
      
      else if (drunkLevel == 4) {
        display.setCursor(20,40); // Set cursor postion
        display.print("Now the party's"); // Print to screen
        display.setCursor(25,50); // Set cursor postion
        display.print("getting going"); // Print to screen
      }
      
      else if (drunkLevel == 5) {
        display.setCursor(10,40); // Set cursor postion
        display.print("You might want to"); // Print to screen
        display.setCursor(10,50); // Set cursor postion
        display.print("start on the shots"); // Print to screen
      }
      
      else if (drunkLevel == 6) {
        display.setCursor(0,40); // Set cursor postion
        display.print("Your well on your way"); // Print to screen
      }
      
      else if (drunkLevel == 7) {
        display.setCursor(20,40); // Set cursor postion
        display.print("You've on a roll"); // Print to screen
      }
      
      else if (drunkLevel == 8) {
        display.setCursor(15,40); // Set cursor postion
        display.print("Pretty impressive"); // Print to screen
        display.setCursor(10,50); // Set cursor postion
        display.print("Have you puked yet?"); // Print to screen
      }
      
      else if (drunkLevel == 9) {
        display.setCursor(15,40); // Set cursor postion
        display.print("The room will be"); // Print to screen
        display.setCursor(17,50); // Set cursor postion
        display.print("spinning tonight"); // Print to screen
      }
      
      else if (drunkLevel == 10) {
        display.setCursor(25,40); // Set cursor postion
        display.print("Your steaming"); // Print to screen
      }
      
      else if (drunkLevel >= 11) {
        display.setCursor(25,40); // Set cursor postion
        display.print("Off the chart"); // Print to screen
        display.setCursor(5,50); // Set cursor postion
        display.print("Can you even see XXX"); // Print to screen
      }
      
      display.display(); // Writes to the screen
    displayScreen += 100;
  }
  }


  if (debugMode == 1) {
    if(millis() >= debugScreen) {
      display.clearDisplay(); // Clears everything off the screen
      display.setTextSize(1); // Set text size
      display.setTextColor(WHITE); // Set text colour
      display.setCursor(0,0); // Set cursor postion
      display.print("Drunk Level "); // Print to screen
      display.println(drunkLevel); // Print to screen
      display.setCursor(0,35); // Set cursor postion
      display.print("Sensor reading "); // Print to screen
      display.println(sensor); // Print to screen
      display.setCursor(0,45); // Set cursor postion
      display.print("Max reading "); // Print to screen
      display.println(maxReading); // Print to screen
      display.setCursor(0,55); // Set cursor postion
      display.print("Minumum reading "); // Print to screen
      display.println(minReading); // Print to screen
      display.display(); // Writes to the screen
    debugScreen += 100;
  }
  }
}


void splashScreen() {
  // Warm up code
  for(loadingPercent = 0; loadingPercent < 100; loadingPercent++) {
    display.clearDisplay(); // Clears everything off the screen
    display.setTextSize(2); // Set text size
    display.setTextColor(WHITE); // Set text colour
    display.setCursor(5,0); // Set cursor postion
    display.println("Warming up"); // Print to screen
    
    if (loadingPercent < 10) { // If its a single digit number the centring needs to be different
      display.setCursor(55,20); // Set cursor postion
      display.print(loadingPercent); // Print to screen
      display.print("%"); // Print to screen
    }
    
    else if (loadingPercent >= 10) { // If its a double digit number the centring needs to be different
      display.setCursor(50,20); // Set cursor postion
      display.print(loadingPercent); // Print to screen
      display.print("%"); // Print to screen
    }
    
    else if (loadingPercent == 100) { // If its a triple digit number the centring needs to be different
      display.setCursor(40,20); // Set cursor postion
      display.print(loadingPercent); // Print to screen
      display.print("%"); // Print to screen
    }
    
    display.display(); // Writes to the screen
    delay(600); //Dirty delay, god I hate these things
  }
  
  int sensor = analogRead(analogPin); // Reads the analogPin
  minReading = sensor; // Sets the minReading to the curren sensor reading
  maxReading = sensor + adding; // Sets the maxReading to the curren sensor reading plus 300
  startSplash = 5; // Sets startSplash to 5 so it will skip the countdown when done
  firstRun = 0; // Sets firstRun to 0 so it wouldnt run the warmup code again
}

