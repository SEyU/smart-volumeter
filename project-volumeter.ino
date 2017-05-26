#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "ssid"
#define WLAN_PASS       "pass"

/************************* Information to Connect to Cloud MQTT *********************************/
/*  You will need and account - Its FREE  */

#define AIO_SERVER      "m13.cloudmqtt.com"  
#define AIO_SERVERPORT  15040                  
#define AIO_USERNAME    "egudeacq"
#define AIO_KEY         "OkFTZSCaav9U"

/************ Setting up your WiFi Client and MQTT Client ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.

WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Set Up a Feed to Publish To ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish myFirstValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/myFirstValue");


/********************* Values ******************************/
// Need a changing value to send.  We will get this value
// in the getVal function.  

double volumen=0;

/*************************** Sketch Code ***********************************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();


//Definimos las medidas de los ejes
double ancho=26;
double largo=29;
double alto=21;

double anchoObjeto=0;
double altoObjeto=0;
double largoObjeto=0;

#define TRIGGER_PIN  14 // D5
#define ECHO_PIN_1     12 //D6 
#define ECHO_PIN_2     13 //D7 
#define ECHO_PIN_3     15 //D8 

void setup() {
  Serial.begin (115200);
  pinMode(TRIGGER_PIN, OUTPUT);/*activación del pin D5 como salida: para el pulso ultrasónico*/
  pinMode(ECHO_PIN_1, INPUT); /*activación del pin D6 como entrada: tiempo del rebote del ultrasonido*/
  pinMode(ECHO_PIN_2, INPUT);/*activación del pin D7 como entrada: tiempo del rebote del ultrasonido*/
  pinMode(ECHO_PIN_3, INPUT);/*activación del pin D8 como entrada: tiempo del rebote del ultrasonido*/



 delay(10);

  Serial.println(F("Adafruit MQTT"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  
}

void loop() {
  double duration1, distance1, duration2,distance2,duration3,distance3;
  
  digitalWrite(TRIGGER_PIN, LOW);  // Get Start
 
  delayMicroseconds(2); // stable the line 
  digitalWrite(TRIGGER_PIN, HIGH); // sending 10 us pulse
 
  delayMicroseconds(10); // delay 


  digitalWrite(TRIGGER_PIN, LOW); // after sending pulse wating to receive signals 
 
  //EJE X
  duration1 = pulseIn(ECHO_PIN_1, HIGH); // calculating time 
  duration2 = pulseIn(ECHO_PIN_2, HIGH); // calculating time 
  duration3 = pulseIn(ECHO_PIN_3, HIGH); // calculating time 
  
  distance1 = (duration1/2) / 29.1; // single path 
  distance2 = (duration2/2) / 29.1; // single path 
  distance3 = (duration3/2) / 29.1; // single path 

  anchoObjeto=ancho-distance1;
 altoObjeto=alto-distance2;
 largoObjeto=largo-distance3;

   Serial.print("Ancho=");
  Serial.print(anchoObjeto);
  Serial.print(" ,Alto=");
  Serial.print(altoObjeto);
  Serial.print(" ,Largo=");
  Serial.print(largoObjeto);

  volumen=distance1*distance2*distance3;
   Serial.println("Volumen ");
    Serial.println(volumen);


  //conexion wifi

 // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  

  // Now we can publish stuff!
  Serial.print(F("\nSending volume "));
 Serial.print((volumen));
  Serial.print("...");
  if (! myFirstValue.publish(getVal())) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  //fin conexionwifi
  
  delay(2000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


//  Return the volume

double getVal(){

  return volumen;
  
}
