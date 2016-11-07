// Import Libraries
#include "PietteTech_DHT/PietteTech_DHT.h"          // AM2302(DHT22) Sensor Library
#include "Adafruit_IO_Client.h"                     // Adafruit.io API client
#include "Adafruit_IO_Arduino.h"

// DHT parameters
#define DHTTYPE  DHT22                 // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   D3         	       // Digital pin for communications
#define DHT_SAMPLE_INTERVAL   60000    // Sample every sixty seconds

// Adafruit paramters
#define ADAFRUIT_API_KEY "*******************"
#define ADAFRUIT_FEED_TEMPERATURE "Particle-Temperature"
#define ADAFRUIT_FEED_HUMIDITY "Particle-Humidity"

// Variables
int temperature;
int humidity;
int blue = D7;
unsigned int DHTnextSampleTime;     // Next time we want to start sample
bool bDHTstarted;		            // flag to indicate we started acquisition
int n;                              // counter

// Declaration
void dht_wrapper();

// Initialize Libraries
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);       // DHT22 
TCPClient tcpClient;                                    // TCP Client to be used for Adafruit IO connection

// Adafruit IO Connections
Adafruit_IO_Client aio = Adafruit_IO_Client(tcpClient, ADAFRUIT_API_KEY);
Adafruit_IO_Feed aioFeedTemperature = aio.getFeed(ADAFRUIT_FEED_TEMPERATURE);
Adafruit_IO_Feed aioFeedHumidity = aio.getFeed(ADAFRUIT_FEED_HUMIDITY);

void setup() {
    pinMode(blue, OUTPUT);
    DHTnextSampleTime = 0;
    aio.begin();                // Adafruit IO Setup
}

void dht_wrapper() {
    DHT.isrCallback();
}

void loop()
{
  // Check if we need to start the next sample
  if (millis() > DHTnextSampleTime) {
	if (!bDHTstarted) {		// start the sample
	    DHT.acquire();
	    bDHTstarted = true;
	}

    if (!DHT.acquiring()) {		// has sample completed?

    // get DHT status
    int result = DHT.getStatus();
    int humidity = DHT.getHumidity();
    int tempc = DHT.getCelsius();

    //Turn the built in LED on to indicate publishing
    digitalWrite(blue, HIGH);

    // Publish readings to Particle Console
    Particle.publish("Humidity", String(humidity) + "%");
    Particle.publish("Temperature", String(tempc) + " °C");
    
    // Send readings to Adafruit IO
    aioFeedHumidity.send(humidity);
    aioFeedTemperature.send(tempc);

    //Turn the built in LED off indicate publishing finished
    delay(250);
    digitalWrite(blue, LOW);

    n++;  // increment counter
    bDHTstarted = false;  // reset the sample flag so we can take another
    DHTnextSampleTime = millis() + DHT_SAMPLE_INTERVAL;  // set the time for next sample
    
    }
  }
}
