#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define SDA 13 //Define SDA pins
#define SCL 14 //Define SCL pins
LiquidCrystal_I2C lcd(0x27,16,2); 
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TI-84 Plus CE";
const char* password = "that's hot(spot)";
const char* mqttServer = "nickhoang.duckdns.org";
const int mqttPort = 1883;
const char* subTopic = "random_numbers";
const char* pubTopic = "game_results";
char board[3][3]; // 3x3 tic-tac-toe board
bool gameOver = false; // Flag to check if the game is over
int wins = 0;
int losses = 0;
int draws = 0;
int turn = 0;
String gameResult = "S";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char player = 'X';
  turn++;
  Serial.println("Turn: ");
  Serial.print(turn);
  if (turn % 9 == 1) {
    gameOver = false;
  }
  if (turn % 2 == 0) {
     player = 'X';
   }
  else {
    player = 'O';
  }
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  //Serial.println(message);

  int number = atoi(message);
  if (gameOver == false) {
   if (number == 1){
            playerMove(player, 0, 0);
   }
   else if (number == 2) {
            playerMove(player, 0, 1);
   }
   else if (number == 3) {
            playerMove(player, 0, 2);
   }
   else if (number == 4) {
            playerMove(player, 1, 0);
   }
   else if (number == 5) {
            playerMove(player, 1, 1);
   }
   else if (number == 6) {
            playerMove(player, 1, 2);
   }
   else if (number == 7){
            playerMove(player, 2, 0);
   }
   else if (number == 8) {
            playerMove(player, 2, 1);
   }
   else if(number == 9) {
            playerMove(player, 2, 2);
   }
   printGrid();
   isGameOver();
  }
  if (turn % 9 == 0) {
    delay(250);
    if (gameResult.equals("W")) {
      Serial.println("Publishing W");
      client.publish(pubTopic, "W");
    }
      
    else if (gameResult.equals("L")) {
      Serial.println("Publishing L");
      client.publish(pubTopic, "L");
    }
    else if (gameResult.equals("D")) {
      Serial.println("Publishing D");
      client.publish(pubTopic, "D");
    }
    else {
      Serial.println("Error!");
      client.publish(pubTopic, "E");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
      client.subscribe(subTopic);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
 Wire.begin(SDA, SCL); // attach the IIC pin
 Serial.begin(115200); // Start serial communication at 115200 baud rate
 setup_wifi();
 client.setServer(mqttServer, mqttPort);
 client.setCallback(callback);
 if (!i2CAddrTest(0x27)) {
 lcd = LiquidCrystal_I2C(0x3F, 16, 2);
 }
 lcd.init(); // LCD driver initialization
 lcd.backlight(); // Open the backlight
 lcd.setCursor(0,0); // Move the cursor to row 0, column 0
 lcd.print("W   L   D"); // The print content is displayed on the LCD
 lcd.setCursor(0,1);
 lcd.print("0   0   0");

 for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      board[row][col] = ' ';
    }
  }
}

void playerMove(char player, int row, int col) {
  board[row][col] = player; // Place the player's letter on the board
}

void clearBoard() {
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      board[row][col] = ' ';
    }
  }
}

void printGrid() {
  for (int i = 0; i < 3; ++i) {
    if (i != 0) {
      //Serial.println("-----");
    }

    for (int j = 0; j < 3; ++j) {
      if (j != 0) {
        //Serial.print("|");
      }
      //Serial.print(" ");
      //Serial.print(board[i][j]);
      //Serial.print(" ");
    }
    //Serial.println();
  }
}
bool isGameOver() {
  // Check rows, columns, and diagonals for a winning condition or a full board
  for (int i = 0; i < 3; ++i) {
    if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') ||
        (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')) {

      if (board[i][i] == 'X') {
        wins++;
        
        //client.publish(pubTopic, "W");
        gameResult = "W";
        Serial.println("X WINS!");
      }
      else {
        losses++;
        
        //client.publish(pubTopic, "L");
        gameResult = "L";
        Serial.println("O WINS!");
      }

      gameOver = true;
      clearBoard();
      lcd.setCursor(0,1);
      lcd.print(wins);
      if (wins >= 10)
        lcd.print("  ");
      else
        lcd.print("   ");
      lcd.print(losses);
      if (losses >= 10)
        lcd.print("  ");
      else
        lcd.print("   ");
      lcd.print(draws);
      return true;
    }
  }

  if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') ||
      (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')) {
      if (board[1][1] == 'X') {
        wins++;
        gameResult = "W";
        Serial.println("X WINS!");
        //client.publish(pubTopic, "W");
      }
      else {
        losses++;
        gameResult = "L";
        Serial.println("O WINS!");
        //client.publish(pubTopic, "L");
        
      }
      gameOver = true;
      clearBoard();
      
      lcd.setCursor(0,1);
      lcd.print(wins);
      if (wins >= 10)
        lcd.print("  ");
      else
        lcd.print("   ");
      lcd.print(losses);
      if (losses >= 10)
        lcd.print("  ");
      else
        lcd.print("   ");
      lcd.print(draws);
      return true;
  }

  // Check if the board is full
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      if (board[row][col] == ' ') {
        return false; // Game is not over yet
      }
    }
  }

  draws++;
  gameOver = true;
  clearBoard();

  lcd.setCursor(0,1);
  lcd.print(wins);
  if (wins >= 10)
    lcd.print("  ");
  else
    lcd.print("   ");
  lcd.print(losses);
  if (losses >= 10)
    lcd.print("  ");
  else
     lcd.print("   ");
  lcd.print(draws);  
  gameResult = "D";
  Serial.println("DRAW!");
  return true;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

bool i2CAddrTest(uint8_t addr) {
 Wire.begin();
 Wire.beginTransmission(addr);
 if (Wire.endTransmission() == 0) {
 return true;
 }
 return false;
}

