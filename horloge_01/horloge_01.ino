/*
 Name:		horloge_01.ino
 Created:	01/04/2019 19:30:07
 Author:	thomas
*/

#include <iostream>
using namespace std;
#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000


#include <WiFiUdp.h>
const char *ssid = "V20_1763";
const char *password = "01234567";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);



#define HEURE true //affichage des heures
#define MINUTE true //affichage des minutes
#define SECONDE true //affichage des secondes

bool internet = true; //si on trouve pas internet

//Pin connected to ST_CP of 74HC595
int const latchPin = D0;
//Pin connected to SH_CP of 74HC595
int const clockPin = D1;
////Pin connected to DS of 74HC595
int const dataPin = D2;

int heures = 0;
int minutes = 0;
int secondes = 0;

//variables temporaires pour les calculs
int decimale = 0;
int reste = 0;

int refresh = 0;

//variable affich�e sur les nixies quand pas d'internet
int affPasInternet = 0; 
byte affPasInternet_converties = 0;


//variables pour envoyer aux nixies
byte heures_converties = 0;
byte minutes_converties = 0;
byte secondes_converties = 0;

// the setup function runs once when you press reset or power the board
void setup() 
{
	//set pins to output so you can control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	//le debug, c'est ma passion
	Serial.begin(9600);

	WiFi.begin(ssid, password);

	byte timeout_wifi = 0;

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		timeout_wifi++;
		if (timeout_wifi>=59)
		{
			internet = false;
			break;
		}
	}
	Serial.println();

	timeClient.begin();
	timeClient.setTimeOffset(7200); //changement de zone horaire, unit� bizarre pour 2h

}

// the loop function runs over and over again until power down or reset
void loop() {
	//count up routine
	if (internet)
	{
		refresh = 500;
		timeClient.update();

		heures = timeClient.getHours();
		decimale = heures / 10;
		reste = heures % 10;
		heures_converties = decimale << 4 | reste;

		Serial.println("affichage de l'heure");
		Serial.println(heures);
		Serial.println(heures_converties, BIN);


		minutes = timeClient.getMinutes();
		decimale = minutes / 10;
		reste = minutes % 10;
		minutes_converties = decimale << 4 | reste;

		Serial.println("affichage des minutes");
		Serial.println(minutes);
		Serial.println(minutes_converties, BIN);


		secondes = timeClient.getSeconds();
		decimale = secondes / 10;
		reste = secondes % 10;
		secondes_converties = decimale << 4 | reste;

		Serial.println("affichage des secondes");
		Serial.println(secondes);
		Serial.println(secondes_converties, BIN);
	}

	else //pas d'internet, on fait d�filer les chiffres toutes les secondes
	{
		refresh = 1000;
		if (affPasInternet<9)
		{
			affPasInternet++;

			decimale = affPasInternet / 10;
			reste = affPasInternet % 10;

			affPasInternet_converties = heures_converties 
				= minutes_converties = secondes_converties 
				= decimale << 4 | reste;

			Serial.println("affichage de affPasInternet");
			Serial.println(affPasInternet);
			Serial.println(affPasInternet_converties, BIN);

		}
		else
		{
			affPasInternet = 0;
		}
	}
	

	//on pr�vient qu'on va envoyer la pur�e
	digitalWrite(latchPin, 0);

	if (HEURE) //si on veut afficher l'heure
	{
		shiftOut(dataPin, clockPin, MSBFIRST, heures_converties);
	}
	if (MINUTE) //si on veut afficher les minutes
	{
		shiftOut(dataPin, clockPin, MSBFIRST, minutes_converties);
	}
	if (SECONDE) //si on veut afficher les secondes
	{
		shiftOut(dataPin, clockPin, MSBFIRST, secondes_converties);
	}
	//on arr�te d'envoyer la pur�e
	digitalWrite(latchPin, 1);
	delay(refresh);
}

