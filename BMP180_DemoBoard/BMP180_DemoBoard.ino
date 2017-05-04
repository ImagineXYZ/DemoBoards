//=========================Includes==============================
#include "Arduino.h"
#include "ESP8266_XYZ_StandAlone.h"
#include <SFE_BMP180.h>
#include <Wire.h>

//==========================DefineForNetwork===============================

#define mqtt_server "m11.cloudmqtt.com"
#define mqtt_port 17984

#define mqtt_id "Nodo Prueba"
#define user "nodeThing"
#define pass "nodeThing"
#define test_topic "PruebaMQTTLib"

#define mqtt_retries_reconnect 5
#define mqtt_retries_delay_ms 500

const char* wifi_ssid = "Javo";
const char* wifi_pass = "123456789";

//==========================Objects===============================

ESP8266_XYZ esp;
SFE_BMP180 pressure;

//=====================Variables Globales========================

#define ID "Demo6"
#define retardo_ms 50
#define DEBUG 0

char status;
double T;
double P;
//int cont;

void BMP_init(){
	if (pressure.begin()) {
		if (DEBUG){
			Serial.println("BMP180 init success");
		}
	}

	else{
		if (DEBUG){
			Serial.println("BMP180 init fail\n\n");
		}
		while (1);
	}
}

void Presion_Temp() {
	status = pressure.startTemperature();
	if (status != 0){
		delay(status);
		status = pressure.getTemperature(T);
		if (status != 0){
			if (DEBUG){
				Serial.print("temperature: ");
				Serial.println(T, 2);
			}
			status = pressure.startPressure(3);// The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
			if (status != 0){
				delay(status);
				status = pressure.getPressure(P, T);
				if (status != 0){
					if (DEBUG){
						Serial.print("absolute pressure: ");
						Serial.print(P, 2);
						Serial.println(" mb");
					}
				}
				else {
					if (DEBUG){
						Serial.println("error retrieving pressure measurement\n");
					}
				}
			}
			else {
				if (DEBUG){
					Serial.println("error starting pressure measurement\n");
				}
			}
		}
		else {
			if (DEBUG){
				Serial.println("error retrieving temperature measurement\n");
			}
		}
	}
	else {
		if (DEBUG){
			Serial.println("error starting temperature measurement\n");
		}
	}
}

void enviarMensaje(){
	if(DEBUG){
		Serial.println("Enviando Mensaje");
	}
	esp.addToJson("ID", ID);
	esp.addToJson("Temp", String(T));
	esp.addToJson("P_Abs", String(P));
	//esp.addToJson("Conteo", String(cont));
	esp.MQTTPublish(test_topic);
	//cont++;
}

void setup(){
	if(DEBUG){
		Serial.begin(115200);
		Serial.println("Conectando Wifi...");
	}
	while(!esp.connectAP(wifi_ssid, wifi_pass)){
		delay(200);
		if(DEBUG){
			Serial.println("Conectando Wifi...");
		}
	}

	if(DEBUG){
		Serial.println("MQTT Config");
	}

	esp.MQTTTSetServer(mqtt_server, mqtt_port,user,pass);
	esp.MQTTConfig(mqtt_id, mqtt_retries_reconnect, mqtt_retries_delay_ms);

	esp.MQTTSetCallback(callbackTest);

	//esp.MQTTSubscribe(test_topic); //Para no recibir información del canal suscrito

	//Rutina para iniciar el BMP180

	Wire.begin(0, 2); //Wire.begin(int sda, int scl)
	BMP_init();
}

void loop(){
	esp.MQTTLoop();
	Presion_Temp();
	enviarMensaje();
	delay(retardo_ms);
}

void callbackTest(char* topic, byte* payload, unsigned int length) {
	if(DEBUG){
		Serial.print("Message arrived [");
		Serial.print(topic);
		Serial.print("] ");
		for (uint i = 0; i < length; i++) {
			Serial.print((char)payload[i]);
		}
		Serial.println();
	}
}
