#include <LiquidCrystal.h>
#include <SD.h>

const int ledPins[6] = {0, 1, 2, 3, 4, 5}; // Multiplexer channels for LEDs
const int buttonPins[6] = {8, 9, 10, 11, 12, 13}; // Multiplexer channels for buttons
const int muxS0 = 2;
const int muxS1 = 3;
const int muxS2 = 4;
const int muxS3 = 5;
const int ledComPin = 6;
const int buttonComPin = 7;
const int startButtonPin = A0; // Middle button to start/stop the round
const int whiteButtonPin = A1; // Button to save score
const int sdCardCS = 10;

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
File myFile;

unsigned long startTime;
unsigned long reactionTimes[100];
int totalPenalty = 0;
int pressCount = 0;
bool roundActive = false;
bool canSaveScore = false;
unsigned long totalTime = 0;

void setup() {
  pinMode(ledComPin, OUTPUT);
  pinMode(buttonComPin, INPUT);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(whiteButtonPin, INPUT_PULLUP);
  
  for (int i = 0; i < 4; i++) {
    pinMode(muxS0 + i, OUTPUT);
  }

  lcd.begin(16, 2);
  lcd.print("Press white button to start");

  if (!SD.begin(sdCardCS)) {
    lcd.setCursor(0, 1);
    lcd.print("SD init failed!");
    while (1);
  }
}

void loop() {
  if (digitalRead(startButtonPin) == LOW && !roundActive) {
    startRound();
  } else if (digitalRead(startButtonPin) == LOW && roundActive) {
    endRoundEarly();
  }

  if (roundActive) {
    playRound();
  }

  if (canSaveScore && digitalRead(whiteButtonPin) == LOW) {
    saveScore();
  }
}

void startRound() {
  roundActive = true;
  pressCount = 0;
  totalPenalty = 0;
  memset(reactionTimes, 0, sizeof(reactionTimes));
  lcd.clear();
  lcd.print("Round started");
  delay(1000);
}

void endRoundEarly() {
  roundActive = false;
  canSaveScore = false;
  lcd.clear();
  lcd.print("Round ended early");
  delay(2000);
  lcd.clear();
  lcd.print("Press white button to start");
}

void playRound() {
  int ledIndex = random(0, 6);
  activateLed(ledIndex);
  startTime = millis();
  
  bool buttonPressed = false;
  while (millis() - startTime < 500) {
    if (digitalReadButton(ledIndex) == LOW) {
      reactionTimes[pressCount] = millis() - startTime;
      buttonPressed = true;
      break;
    }
  }

  if (!buttonPressed) {
    totalPenalty += 1000; // 1 second penalty
  }

  deactivateLed(ledIndex);
  pressCount++;
  if (pressCount >= 100) {
    endRound();
  }
}

void endRound() {
  roundActive = false;
  canSaveScore = true;
  lcd.clear();
  lcd.print("Round ended");
  displayResults();
}

void activateLed(int index) {
  selectMuxChannel(ledPins[index]);
  digitalWrite(ledComPin, HIGH);
}

void deactivateLed(int index) {
  digitalWrite(ledComPin, LOW);
}

bool digitalReadButton(int index) {
  selectMuxChannel(buttonPins[index]);
  return digitalRead(buttonComPin);
}

void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxS0 + i, (channel >> i) & 0x01);
  }
}

void displayResults() {
  totalTime = 0;
  for (int i = 0; i < pressCount; i++) {
    totalTime += reactionTimes[i];
  }
  totalTime += totalPenalty;

  lcd.setCursor(0, 1);
  lcd.print("Total time: ");
  lcd.print(totalTime / 1000.0, 3);
  lcd.print(" s");
  delay(2000); // Display for 2 seconds

  // Calculate rank or percentile
  int rank = calculateRank(totalTime);
  lcd.clear();
  lcd.print("Rank: ");
  lcd.print(rank);
  lcd.print(" out of ");
  lcd.print(getTotalScores());
  delay(2000); // Display for 2 seconds

  lcd.clear();
  lcd.print("Press white button to save game");
}

void saveScore() {
  myFile = SD.open("scores.txt", FILE_WRITE);
  if (myFile) {
    myFile.print("Total time: ");
    myFile.print((float)totalTime / 1000.0, 3);
    myFile.println(" s");
    myFile.close();
    lcd.setCursor(0, 1);
    lcd.print("Score saved!");
    delay(2000);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Save failed");
    delay(2000);
  }
  canSaveScore = false;
  lcd.clear();
  lcd.print("Press white button to start");
}

int calculateRank(unsigned long newScore) {
  File myFile = SD.open("scores.txt");
  if (!myFile) {
    return 1; // If there are no previous scores, this is the first and best score
  }

  int rank = 1;
  unsigned long previousScore;
  while (myFile.available()) {
    String line = myFile.readStringUntil('\n');
    previousScore = (unsigned long)(line.substring(12).toFloat() * 1000);
    if (newScore > previousScore) {
      rank++;
    }
  }
  myFile.close();
  return rank;
}

int getTotalScores() {
  File myFile = SD.open("scores.txt");
  if (!myFile) {
    return 0; // If there are no previous scores
  }

  int totalScores = 0;
  while (myFile.available()) {
    myFile.readStringUntil('\n');
    totalScores++;
  }
  myFile.close();
  return totalScores;
}
