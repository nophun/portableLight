#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "html.h"
#include "rotary.h"
#include "ledCtrl.h"

String host = "esp32";
String ssid = "ESP_AP";
String password = "winteriscoming";

unsigned int led_A_pin = 3U;
unsigned int led_B_pin = 10U;
unsigned int button_pin = 5U;
unsigned int rot_dat_pin = 6U;
unsigned int rot_clk_pin = 7U;

static constexpr unsigned int led_ch = 0U;
static constexpr unsigned int led_freq = 20000U;
static constexpr unsigned int led_pwm_bits = 8U;
unsigned int brightness = {1 << (led_pwm_bits / 2)};
bool update_mode = {false};

LedCtrl LED{led_ch, led_pwm_bits, brightness, 0.1F};
RotaryEncoder rotary{rot_clk_pin, rot_dat_pin, 10, RotaryMode::FULL_STEP};
WebServer server{80};

void setupWIFI(void) {
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupServer(void) {
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void setupHW(void) {
  ledcSetup(led_ch, led_freq, led_pwm_bits);
  ledcAttachPin(led_A_pin, led_ch);
  ledcAttachPin(led_B_pin, led_ch);
  pinMode(rot_clk_pin, INPUT_PULLUP);
  pinMode(rot_dat_pin, INPUT_PULLUP);
  pinMode(button_pin, INPUT_PULLUP);
  update_mode = !digitalRead(button_pin);
}

void setup(void) {
  Serial.begin(115200);

  setupHW();
  if (update_mode) {
    setupWIFI();
    setupServer();
  }
  LED.init();
}

void loop(void) {
  if (update_mode) {
    server.handleClient();
  }

  uint8_t rotary_movement = rotary.update();
  if (rotary_movement == DIR_CW) {
    if (brightness < LED.get_last_step()) {
      brightness <<= 1;
      LED.request_brightness(brightness);
      Serial.println(brightness);
    }
  } else if (rotary_movement == DIR_CCW) {
    if (brightness > LED.get_first_step()) {
      brightness >>= 1;
      LED.request_brightness(brightness);
      Serial.println(brightness);
    }
  }

  LED.update();
  delay(1);
}
