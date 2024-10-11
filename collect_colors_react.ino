#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN 9  // pin de la RGB
#define NUMPIXELS 2 // nombre de RGB connecté
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Déclaration des lignes et colonnes de la matrice (utiliser les numéros de broches directement)
const int ROW_PINS[] = {7, 8, 1};  // Modifier selon vos connexions
const int COL_PINS[] = {20, 10, 0};  // Modifier selon vos connexions

const char* ssid = ""; // ajouter le nom du wifi
const char* password = ""; //  ajouter le mdp

WebSocketsServer webSocket = WebSocketsServer(80);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint32_t selectedColor = pixels.Color(150, 150, 150);  
int selectedOption = 2;// l'état de l'affichage  dans l'ordre (oui, non , en attente)

void setup() {
  Serial.begin(115200);
    // Initialisation NeoPixel
  pixels.begin();
  pixels.clear();
  pixels.show();

  // initialisation écran
  Wire.begin(5, 6);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
  updateDisplayState();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("Starting WebSocket server...");
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  // Configurer les broches lignes comme sorties et les colonnes comme entrées
  for (int row = 0; row < 3; row++) {
    pinMode(ROW_PINS[row], OUTPUT);
    digitalWrite(ROW_PINS[row], HIGH); // Initialise les lignes à HIGH
  }
  
  for (int col = 0; col < 3; col++) {
    pinMode(COL_PINS[col], INPUT_PULLUP); // Utiliser les résistances de pull-up internes pour les colonnes
  }
}

void loop() {
  webSocket.loop();
  handleButtonPress();
}

// void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
//   if (type == WStype_TEXT) {
//     String message = (char*)payload;
//     Serial.println("Message received: " + message);

//     if (message.startsWith("RGB:")) {
//       Serial.println("Color detected: " + message);
//       int r, g, b;
//       sscanf(message.c_str(), "RGB:%d,%d,%d", &r, &g, &b);

//       selectedColor = pixels.Color(r, g, b);
//       Serial.println("Couleur saisie " + selectedColor);
//       delay(5);
//       updatePixels();
//     }
//   }
// }

unsigned long lastUpdateTime = 0; // Enregistre le dernier temps de mise à jour
const unsigned long UPDATE_INTERVAL = 100; // Intervalle minimal en millisecondes

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = (char*)payload;
    unsigned long currentTime = millis();

    // Vérifier si l'intervalle minimal est respecté
    if (currentTime - lastUpdateTime < UPDATE_INTERVAL) {
      Serial.println("Skipping update to avoid overload.");
      return;
    }

    Serial.println("Message received: " + message);

    if (message.startsWith("RGB:")) {
      int r, g, b;
      sscanf(message.c_str(), "RGB:%d,%d,%d", &r, &g, &b);
      //selectedColor = pixels.Color(r, g, b);
      //Serial.println("color detected: " + String(r) + "," + String(g) + "," + String(b));
      // Utilisation de la méthode fill() pour allumer toutes les LEDs avec la même couleur
      pixels.fill(pixels.Color(r, g, b));
      Serial.println("color sent: " + String(r) + "," + String(g) + "," + String(b));
      pixels.show(); // Applique la couleur aux LEDs


      //updatePixels();
      lastUpdateTime = currentTime; // Mettre à jour l'heure de la dernière mise à jour
    }
  }
}


// void updatePixels() {

//   // méthodé pour pouvoir régler la couleur de rgb individuel
//   // for (int i = 0; i < NUMPIXELS; i++) {
//   //   pixels.setPixelColor(i, selectedColor);
//   // }

//   // fill permet 
//   pixels.fill()
//   pixels.show();
// }

void displayMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print(msg);
  display.display();
}

void handleButtonPress() {
  // Scanner la matrice de boutons
  for (int row = 0; row < 3; row++) {
    // Activer la ligne courante en la mettant à LOW
    digitalWrite(ROW_PINS[row], LOW);
    
    for (int col = 0; col < 3; col++) {
      if (digitalRead(COL_PINS[col]) == LOW) {  // Le bouton est pressé
        if (row == 0 && col == 0) {  // Bouton "Up"
          selectedOption = (selectedOption + 1) % 3;
          updateDisplayState();
        } else if (row == 2 && col == 0) {  // Bouton "Down"
          selectedOption = (selectedOption + 2) % 3;
          updateDisplayState();
        } else if (row == 0 && col == 2) {  // Bouton "A" pour valider
          if (selectedOption == 0) {
            webSocket.broadcastTXT("Satisfaction: Yes");
          } else if (selectedOption == 1) {
            webSocket.broadcastTXT("Satisfaction: No");
          }
          selectedOption= 2;
          updateDisplayState();
        }
        delay(200);  // Petit délai pour le debounce
      }
    }
    
    // Désactiver la ligne courante en la remettant à HIGH
    digitalWrite(ROW_PINS[row], HIGH);
  }
}

void updateDisplayState() {
  switch (selectedOption) {
    case 0:
      displayMessage("Oui");
      break;
    case 1:
      displayMessage("Non");
      break;
    case 2:
      displayMessage("En attente");
      break;
  }
}