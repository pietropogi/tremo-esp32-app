#include <UrlEncode.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Firebase credentials
#define API_KEY "XXX"
#define DATABASE_URL "XXX"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signUpOK = false;
unsigned long sendDataPrevMillis = 0;

const char* ssid = "XXX";
const char* password = "XXX";

// Sensors and notifications
int sw_420 = 4;
int vibracall = 23;
String phoneNumber = "XXX";
String apiKey = "XXX";

// Send WhatsApp message
void sendMessage(String message) {

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Send HTTP POST request
  int httpResponseCode = http.POST("");
  if (httpResponseCode == 200) {
    Serial.print("Mensagem enviada com sucesso");
  } else {
    Serial.println("Erro no envio da mensagem");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(sw_420, INPUT);
  pinMode(vibracall, OUTPUT);

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi: " + WiFi.localIP());

  // Firebase configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  if (Firebase.signUp(&config, &auth, "", "")) {
    signUpOK = true;
    Serial.println("SignUp Ok");
  }
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int val = digitalRead(sw_420);


  
  if (val == HIGH) {
    Serial.println("Vibration Detected...");
    sendMessage("Vibrações anormais detectadas no dispositivo tremo-1.0.0.");
  } else {
    Serial.println("No vibration Detected...");
  }

  if (Firebase.ready() && signUpOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    if (Firebase.RTDB.setInt(&fbdo, "/vibration", val)) {
      Serial.println("Dados enviados ao Firebase com sucesso");
    } else {
      Serial.println("Falha ao enviar: " + fbdo.errorReason());
    }
    sendDataPrevMillis = millis();
  }

 
}

