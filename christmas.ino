
#include <Adafruit_NeoPixel.h>   
#include <WiFi.h>
#include <MQTT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


//------------------------Defines for Neo-Pixel ring ------------------------------
#define LED_PIN   5  // Which pin on the ESP32 is connected to the NeoPixels? 
#define LED_COUNT 64 // How many NeoPixels are attached to the ESP32?
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//---------------------------Defines Wifi and broker Connection ------------------------------
const char ssid[] = "MSI 9165";
const char pass[] = "abcd7654";
const char MQTT_BROKER_ADDRESS[] = "192.168.137.1";

bool light_1 = false;
bool light_2 = false;
bool light_3 = false;
bool light_4 = false;
bool light_5 = false;

WiFiClient net;
MQTTClient client(50000); //sets maximum message-size to ~25kB

//-------------------------------Defines MQTT topics------------------------------
const char TOPIC_PIXEL[] = "christmas";

//-----------------------------Function Prototypes----------------------------------
//Function to establish connection to WiFi and MQTT
void connect();
void lightsOff();
void theaterChase(uint32_t color);
void rainbow();
void theaterChaseRainbow();

void messageReceived(String &topic, String &input);

//-----------------------------Display-Helper-Functions----------------------------------
void log(String message)
{
  Serial.println(message.c_str());
}


void setup() 
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  Serial.begin(500000);
  
  //------------------------Neo Pixel-------------------------------
  strip.begin();
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  for(int i=0; i<LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0,0,0));
    strip.show();
  }
  
  //--------------------WIFI & MQTT Connection----------------------------
  
  client.begin(MQTT_BROKER_ADDRESS, net); //Set the IP address directly.
  client.onMessage(messageReceived); //Set wich function to call on receiving a MQTT message
  connect(); //connect to WiFi and MQTT
}

void loop() 
{
  client.loop(); // function to check for new message arrivals

  if (!client.connected()) 
  { 
    connect(); // in case of connection loss, the client reconnects
  }

  if ((light_1 == false) && (light_2 == false) && (light_3 == false)&& (light_4 == false)&& (light_5 == false))
  {
    lightsOff(); // go to lightshow1 function when other flags are false
  }
  
  if ((light_1 == true) && (light_2 == false) && (light_3 == false)&& (light_4 == false)&& (light_5 == false))
  {
    theaterChase(strip.Color(127, 127, 127), 50); // go to lightshow1 function when other flags are false
  }

  if ((light_2 == true) && (light_1 == false) && (light_3 == false)&& (light_4 == false)&& (light_5 == false))
  {
    theaterChase(strip.Color(127,   0,   0), 50); // go to lightshow1 function when other flags are false
  }

  if ((light_3 == true) && (light_1 == false) && (light_2 == false)&& (light_4 == false)&& (light_5 == false))
  {
    theaterChase(strip.Color(  0,   0, 127), 50); // go to lightshow1 function when other flags are false
  }

  if ((light_4 == true) && (light_1 == false) && (light_2 == false)&& (light_3 == false)&& (light_5 == false))
  {
    rainbow(5); // go to lightshow1 function when other flags are false
  }

  else if ((light_5 == true) && (light_1 == false) && (light_2 == false)&& (light_4 == false)&& (light_3 == false))
  {
    theaterChaseRainbow(50); // go to lightshow1 function when other flags are false
  }
}

void messageReceived(String &topic, String &input) 
{
  Serial.print("\nMessage received: ");
  Serial.print(input);
  if (topic == TOPIC_PIXEL)
  {
    if (input == "0")
    {
      light_1 = false;
      light_2 = false;
      light_3 = false;
      light_4 = false;
      light_5 = false;
    }
    if (input == "1")
    {
      light_1 = true;
      light_2 = false;
      light_3 = false;
      light_4 = false;
      light_5 = false;
    }
    
    else if (input == "2")
    {
      light_1 = false;
      light_2 = true;
      light_3 = false;
      light_4 = false;
      light_5 = false;
    }

    else if (input == "3")
    {
      light_1 = false;
      light_2 = false;
      light_3 = true;
      light_4 = false;
      light_5 = false;
    }

    else if (input == "4")
    {
      light_1 = false;
      light_2 = false;
      light_3 = false;
      light_4 = true;
      light_5 = false;
    }

    else if (input == "5")
    {
      light_1 = false;
      light_2 = false;
      light_3 = false;
      light_4 = false;
      light_5 = true;
    }
  }
}

void lightsOff(){
  for(int i=0; i<LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0,0,0));
    strip.show();
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<10; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void connect() 
{
  //--------------------Connection to WiFi---------------
  log("Checking wifi");
 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("."); 
  }
  
  Serial.print("connected");
  log("\nConnected!");
  delay(2000);
  
  //--------------------Connection to Broker---------------
  log("\nConnecting to Broker");
  String clientId = "user1";
  while (!client.connect(WiFi.macAddress().c_str(), "try", "try")) 
  {
    Serial.print(".");
  }
  
  log("\nConnected!");
  delay(2000);
  
  //---------------Subscribe to Topics--------------------
  client.subscribe(TOPIC_PIXEL);
}
