#include <LiquidCrystal.h>
#include <SD.h>
#include <SPI.h>

// LCD pin configuration
const int lcdRs = 8, lcdEn = 7, lcdD4 = 5, lcdD5 = 6, lcdD6 = 3, lcdD7 = 2;

// SD card pin configuration
const int sdCardCs = 10; // CS pin for SD card

// Button and LED pin configuration
const int buttonPin = A0;
const int whiteButtonPin = 4;
const int yellowLedPin = A1, blueLedPin = A2, redLedPin = A3;

// Game parameters
const int totalReactions = 10;
const int penaltyTimeMs = 5000;
const int reactionThresholdMs = 2000;

// Variables to store reaction times and penalties
unsigned long reactionTimes[totalReactions];
int penaltyCount = 0;
int currentReaction = 0;
bool gameRunning = false;

LiquidCrystal lcd(lcdRs, lcdEn, lcdD4, lcdD5, lcdD6, lcdD7);

void setup() {
  Serial.begin(9600);
  pinMode(sdCardCs, OUTPUT);
  pinMode(whiteButtonPin, INPUT_PULLUP);

  // Initialize the LCD
  lcd.begin(16, 2);

  // Initialize LEDs
  pinMode(yellowLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // Initialize the SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(sdCardCs)) {
    Serial.println("SD Init Failed");
    while (1);
  } else {
    Serial.println("SD Init Successful");
  }

  lcd.print("Press white btn");
  lcd.setCursor(0, 1);
  lcd.print("to start!");

}

void loop() {
  if (!gameRunning) {
    // Serial.println(digitalRead(whiteButtonPin));
    // delay(1000);
    // Serial.println(analogRead(buttonPin));
    if (digitalRead(whiteButtonPin) == LOW) { // Check if the white button is pressed
      startGame();
    }
  } else {
    playGame();
  }
}

void startGame() {
  gameRunning = true;
  currentReaction = 0;
  penaltyCount = 0;
  memset(reactionTimes, 0, sizeof(reactionTimes));
  lcd.clear();
  lcd.print("Game started!");
  delay(5000);
  lcd.clear();
}

void playGame() {
  if (currentReaction < totalReactions) {
    // Randomly choose an LED to light up
    int led = random(3);
    switch (led) {
      case 0: digitalWrite(blueLedPin, HIGH); break;
      case 1: digitalWrite(yellowLedPin, HIGH); break;
      case 2: digitalWrite(redLedPin, HIGH); break;
    }

    int leftRight = random(2);
    lcd.clear();
    if (leftRight == 0) {
      lcd.print("LEFT!");
    } else {
      lcd.print("RIGHT!");
    }

    unsigned long startTime = millis();
    bool correctPress = false;

    while (millis() - startTime < reactionThresholdMs && !correctPress) {
      int buttonValue = analogRead(buttonPin);
      Serial.println(buttonValue);
      if (
          (led == 0 && ( // BLUE
            ((buttonValue >= 9 && buttonValue <= 13) && leftRight == 0) ||
            ((buttonValue >= 1010 && buttonValue <= 1020) && leftRight == 1)
            )) ||
          (led == 1 && ( // YELLOW
            ((buttonValue >= 14 && buttonValue <= 19) && leftRight == 0) ||
            ((buttonValue >= 514 && buttonValue <= 520) && leftRight == 1)
            )) ||
          (led == 2 && ( // RED
            ((buttonValue >= 4 && buttonValue <= 7) && leftRight == 0) ||
            ((buttonValue >= 764 && buttonValue <= 780) && leftRight == 1)
            ))
          ) {
        unsigned long reactionTime = millis() - startTime;
        reactionTimes[currentReaction++] = reactionTime;
        correctPress = true;
        break;
      }
    }

    if (!correctPress) {
      reactionTimes[currentReaction++] = penaltyTimeMs;
      penaltyCount++;
    }

    // Turn off all LEDs
    digitalWrite(blueLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    delay(500); // Short delay before next reaction
  } else {
    endGame();
  }
}

void endGame() {
  gameRunning = false;
  unsigned long totalTime = 0;
  for (int i = 0; i < totalReactions; i++) {
    totalTime += reactionTimes[i];
  }
  float averageTime = totalTime / (float)totalReactions;

  lcd.clear();
  lcd.print("Total time:");
  lcd.setCursor(0, 1);
  lcd.print(totalTime);
  delay(5000);

  lcd.clear();
  lcd.print("Avg time:");
  lcd.setCursor(0, 1);
  lcd.print(averageTime);
  delay(5000);

  // Display rank
  int rank = getScoreRanking(totalTime);
  lcd.clear();
  lcd.print("Top rank:");
  lcd.setCursor(0, 1);
  lcd.print(rank);
  delay(5000);

  lcd.clear();
  lcd.print("Press white btn");
  lcd.setCursor(0, 1);
  lcd.print("to save score!");
  
  // Wait for user input to save the score
  unsigned long startTime = millis();
  bool decisionMade = false;
  while (millis() - startTime < 10000) {
    if (digitalRead(whiteButtonPin) == LOW) { // Check if the white button is pressed
      saveScore(totalTime);
      decisionMade = true;
      break;
    }
  }

  lcd.clear();
  if (decisionMade) {
    lcd.print("Score saved!");
  } else {
    lcd.print("Score not saved.");
  }
  delay(5000);

  lcd.clear();
  lcd.print("Press white btn");
  lcd.setCursor(0, 1);
  lcd.print("to start!");
}

void saveScore(unsigned long score) {
  File myFile = SD.open("scores.txt", FILE_WRITE);
  if (myFile) {
    // myFile.println(score);
    myFile.close();
    Serial.println("Score saved.");
  } else {
    Serial.println("Error opening scores.txt for writing");
  }
}

int getScoreRanking(unsigned long myScore) {
  int rank = 0;
  File myFile = SD.open("scores.txt");
  if (myFile) {
    while (myFile.available()) {
      unsigned long score = myFile.parseInt();
      // Serial.print("Iterating through the SD card scores: ");
      // Serial.println(score);
      if (score < myScore) {
        rank++;
      }
    }
    myFile.close();
  } else {
    Serial.println("Error opening scores.txt for reading");
  }
  return rank + 1; // Rank is zero-based, so add 1
}
