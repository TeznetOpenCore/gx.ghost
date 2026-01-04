#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_Keypad.h"

// ===============================
// OLED Definitions
// ===============================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===============================
// Keypad Definitions
// ===============================
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {23, 26, 25, 27};
byte colPins[COLS] = {32, 13, 12, 19};

Adafruit_Keypad keypad = Adafruit_Keypad(
  makeKeymap(keys), rowPins, colPins, ROWS, COLS
);

// ===============================
// Structs
// ===============================
struct Entity {
  int x;
  int y;
};

Entity player = {0, 0};

// ===============================
// Game Variables
// ===============================
int grid[4][4];          // Stores numbers in grid
bool selected[4][4];     // Tracks selected cells
int primeNumbers[16];    // Stores selected prime numbers
int primeCount = 0;      // Count of primes selected
unsigned long gameStartTime;
bool gameOver = false;

// Timer for slow updates
unsigned long lastUpdateTime = 0;  // Last grid update
int cellToUpdateX = random(0, 4); // Random cell to update
int cellToUpdateY = random(0, 4);

// ===============================
// Setup
// ===============================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Prime Picker");
  display.display();

  keypad.begin();
  randomSeed(analogRead(0));

  // Initialize grid with random numbers 1-100
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grid[i][j] = random(1, 101);
      selected[i][j] = false;
    }
  }

  gameStartTime = millis();
}

// ===============================
// Main Loop
// ===============================
void loop() {
  unsigned long now = millis();

  // Keypad input
  keypad.tick();
  while (keypad.available()) {
    keypadEvent e = keypad.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED && !gameOver) {
      handleInput((char)e.bit.KEY);
    }
  }

  // Slow grid update: only one cell every 10 seconds
  if (!gameOver && now - lastUpdateTime >= 2000) {
    grid[cellToUpdateX][cellToUpdateY] = random(1, 101); // Update cell
    lastUpdateTime = now;

    // Pick new random cell for next update
    cellToUpdateX = random(0, 4);
    cellToUpdateY = random(0, 4);
  }

  drawBoard();
  checkGameOver();

  delay(100); // Small delay to reduce flicker
}

// ===============================
// Handle Input
// ===============================
void handleInput(char key) {
  if (gameOver) return;

  int i, j;
  switch (key) {
    case '1': i=0;j=0; break; case '2': i=0;j=1; break; case '3': i=0;j=2; break; case 'A': i=0;j=3; break;
    case '4': i=1;j=0; break; case '5': i=1;j=1; break; case '6': i=1;j=2; break; case 'B': i=1;j=3; break;
    case '7': i=2;j=0; break; case '8': i=2;j=1; break; case '9': i=2;j=2; break; case 'C': i=2;j=3; break;
    case '*': i=3;j=0; break; case '0': i=3;j=1; break; case '#': i=3;j=2; break; case 'D': i=3;j=3; break;
    default: return; // Ignore invalid keys
  }

  // Toggle selection
  selected[i][j] = !selected[i][j];

  // If selected, check if prime
  if (selected[i][j]) {
    int number = grid[i][j];
    if (isPrime(number) && primeCount < 16) {
      primeNumbers[primeCount++] = number;
    }
  }
}

// ===============================
// Prime Check
// ===============================
bool isPrime(int num) {
  if (num <= 1) return false;
  for (int k = 2; k * k <= num; k++) {
    if (num % k == 0) return false;
  }
  return true;
}

// ===============================
// Check Game Over
// ===============================
void checkGameOver() {
  if (!gameOver && millis() - gameStartTime >= 60000) {
    gameOver = true;
  }
}

// ===============================
// Draw Board
// ===============================
void drawBoard() {
  display.clearDisplay();
  display.setTextSize(1);

  // Show selected primes at top
  display.setCursor(0, 0);
  display.print("Primes: ");
  for (int i = 0; i < primeCount; i++) {
    display.print(primeNumbers[i]);
    display.print(" ");
  }

  // Draw grid
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      display.setCursor(j * 30, i * 12 + 16);
      display.print(grid[i][j]);

      // Highlight selected cells
      if (selected[i][j]) {
        display.setCursor(j * 30 + 20, i * 12 + 16);
        display.print("<+");
      }
    }
  }

  display.display();
}

//1congratz...if u r unlucky... u caught m3
