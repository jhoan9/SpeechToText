#include "Audio.h"
#include "CloudSpeechClient.h"
#include <ArduinoJson.h>

String palabras[] = {"levantar", "soy digno", "encender", "martillo", "paisanos"};

int led = 23;
int ledStart = 22;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(ledStart, OUTPUT);
  Serial.begin(115200);
  delay(500);
  Serial.println("\r\Graba un Audio!\r\n");
}

void loop() {
  delay(500);
  digitalWrite(ledStart, HIGH);
  delay(1500);
  digitalWrite(ledStart, LOW);
  Serial.println("\r\ ********* Graba un Audio! ***********\r\n");
  Audio* audio = new Audio(ICS43434);
  //Audio* audio = new Audio(M5STACKFIRE);
  audio->Record();
  Serial.println("Recording Completed. Now Processing...");
  CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
  String res = cloudSpeechClient->Transcribe(audio);
  Serial.println("This is a response");
  Serial.println(res);

  const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 90;

  StaticJsonDocument<bufferSize> doc;
  DeserializationError error = deserializeJson(doc, res);

  if (error) {
    Serial.println("Error al parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject results = doc["results"][0];
  JsonArray alternatives = results["alternatives"];

  JsonObject bestAlternative = alternatives[0];
  String transcript = bestAlternative["transcript"];

  Serial.println("Transcripción: ");
  Serial.println(transcript);

  delete cloudSpeechClient;
  delete audio;

   for (int i = 0; i < 5; i++) {
    Serial.print("----");
    Serial.println(palabras[i]);
    if(transcript == palabras[i]){
      digitalWrite(led, HIGH);
      delay(1500);
      digitalWrite(led, LOW);
      delay(1000);
      break;
    }
  }
}
