#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work (clock)
#include <RtcDS3231.h>

#include <SD.h>

#include <DHT.h>
#include <DHT_U.h>

#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>   
#include <SPI.h>                             //Library for SPI                                                                

#define SEIZOEN 0
#define JAAR 1
#define MAAND 2
#define DAG 3
#define UUR 4
#define MINUUT 5
#define SECONDE 6
#define DAGTEMPERATUUR 7
#define NACHTTEMPERATUUR 8
#define LUCHTVOCHTIGHEID 9
#define POTVOCHTIGHEID 10
#define LAMPENVERVANGEN 11
#define DUURDAUW 12
#define DUURREGEN 13
#define DUURBEWOLKING 14
#define DUURDAG 15
#define STARTDAG 16
#define EINDDAG 17
#define STARTDAUW 18
//EINDDAUW
#define STARTREGEN 19
#define EINDREGEN 20
#define ISDAG 21
#define ISREGEN 22
#define ISDAUW 23

#define VENTILATORAAN 15
//AAN
#define VERNEVELAARAAN 16
//AAN

#define TEMPERATUURNU 17
#define LUCHTVOCHTIGHEIDNU 18
#define POTVOCHTIGHEIDNU 19
//POTVOCHTIGHEIDNU
#define LICHT 20 //NU
#define LAMPENAAN1 21
#define LAMPENAAN2 22

#define PLANTENBAKNUMMER 28
#define HOOGSTEPOTVOCHTIGHEID 29
#define MEESTELICHT 30


#define WINTER 0
#define ZOMER 1
#define REGEN 2

#define CHIANGMAI 0
#define MANAUS 1
#define SUMATRA 2

//volgende regel is voor tijdweergave klok
#define countof(a) (sizeof(a) / sizeof(a[0]))

extern uint8_t BigFont[];
extern uint8_t SmallFont[];

//const int defaultPlantenBakSettings[3][4][12] = {{{25, 14, 60, 20, 75, 3, 0, 0, 11, 8}, {35, 28, 55, 10, 75, 2, 0, 0, 12, 8}, {30, 25, 35, 80, 4, 5, 85, 4, 1,13, 8}, {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0}}, {{8, 21, 20, 14, 4, 0, 70, 1, 20, 75}, {8, 22, 30, 20, 4, 1, 55, 1, 20, 75}, {8, 23, 35, 30, 4, 5, 85, 1, 20, 75}, {1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1}}, {{8, 21, 20, 14, 4, 0, 70, 2, 20, 75}, {8, 22, 30, 20, 4, 1, 55, 2, 20, 75}, {8, 23, 35, 30, 4, 5, 85, 2, 20, 75}, {2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2}}};
byte customPlantenBakSettings[3][4][12] = {{{25, 14, 60, 20, 75, 3, 0, 0, 11, 8}, {35, 28, 55, 10, 75, 2, 0, 0, 12, 8}, {30, 25, 35, 80, 4, 5, 85, 4, 1,13, 8}, {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0}}, {{8, 21, 20, 14, 4, 0, 70, 1, 20, 75}, {8, 22, 30, 20, 4, 1, 55, 1, 20, 75}, {8, 23, 35, 30, 4, 5, 85, 1, 20, 75}, {1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1}}, {{8, 21, 20, 14, 4, 0, 70, 2, 20, 75}, {8, 22, 30, 20, 4, 1, 55, 2, 20, 75}, {8, 23, 35, 30, 4, 5, 85, 2, 20, 75}, {2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2}}};

//byte SlaveSelect = 10;

byte pinArray1[8] = {A0, 9, A1, 24, 25, 20, 8, 27}; 
// byte pinArray2[8] = {A0, 9, A1, 24, 25, 20, 8, 27};
// byte pinArray3[8] = {A0, 9, A1, 24, 25, 20, 8, 27};
byte pinArray2[8] = {A2, 11, A3, 24, 25, 20, 10, 27};
byte pinArray3[8] = {A4, 13, A5, 24, 25, 20, 12, 27};

        // soilHumiditySensor(myPins[0], myPins[1]),
        // lichtSensor(myPins[2]),
        // luchtVochtigheidTemperatuurSensor(myPins[6]),
        // klimaatRegelaar(myPins[3] , myPins[7], myPins[5], myPins[4], myPlantenBakNummer),
        // plantenBakNummer(myPlantenBakNummer)
//int currentPage= 1; //indicates the page that is active on touchscreen
//int klimaatDataNu[3][31]; //Veranderen naar byte ivm SPI
byte klimaatDataNu[3][31]= {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}};

class LichtSensor {

    int pin;
    float rawRange = 1024;
    float logRange = 5.0;

    public:
    LichtSensor(int myPin) {
        pin = myPin;
    }

    void initialisatie() {
        Serial.println("lichtsensor geinitialiseerd");
        Serial.print("lichtsensorpin = ");
        Serial.println(pin);
    }
    
    // read the raw value from the light sensor:
    float readRawValue() {
        float rawValue = analogRead(pin);
        delay(10);
        rawValue = analogRead(pin);
        return(rawValue);
    }

    // read the raw value from the light sensor:
    float readLogValue() {
        float rawValue = analogRead(pin);
        float logLux = rawValue * logRange / rawRange;
        float luxValue = pow(10, logLux);
        return(luxValue);
    }

};

class SoilHumiditySensor {

    byte pin;
    byte powerPin;

    public:
    SoilHumiditySensor(byte myPin, byte myPowerPin) {
        pin = myPin;
        powerPin = myPowerPin;
        pinMode(powerPin, OUTPUT);//Set D2 as an OUTPUT
        digitalWrite(powerPin, LOW);//Set to LOW so no power is flowing through the sensor
    }

    void initialisatie() {
        Serial.println("SoilHumiditySensor geinitialiseerd");
        Serial.print("SoilHumiditySensorPin = ");
        Serial.println(pin);
        Serial.print("SoilHumiditySensorPowerPin = ");
        Serial.println(powerPin);
    }
    
    float readValue() {
        digitalWrite(powerPin, HIGH);
        delay(10);
        float soilmoisture = analogRead(pin);
        delay(10);
        soilmoisture = analogRead(pin);
        digitalWrite(powerPin, LOW);
        return(soilmoisture);
    } 
};

class LuchtVochtigheidTemperatuurSensor {

    DHT dht;
    byte pin;

    public:
    LuchtVochtigheidTemperatuurSensor(byte myPin) :
        dht(myPin, DHT22) 
        {
            dht.begin();
            pin = myPin;
        }

    void initialisatie() {
        Serial.println("LuchtVochtigheidTemperatuurSensor geinitialiseerd");
        Serial.print("LuchtVochtigheidTemperatuurSensorPin = ");
        Serial.println(pin);
    }

    float readTempValue() {
        Serial.print("nieuwe temp meting");
        return(dht.readTemperature());
    }

    float readHumidityValue() {
        Serial.print("nieuwe humidity meting");
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
};

class KlimaatRegelaar {
    
    byte lampenPin1;
    byte lampenPin2;
    byte nevelPin;
    byte ventilatorPin;
    int plantenBakNummer;
    boolean ventilatorIsAan = false;
    boolean vernevelaarIsAan = false;
    boolean lampIsAan1 = false;
    boolean lampIsAan2 = false;
    boolean luchtIsDroog = false;
    boolean dag = false;
    boolean dauw = false;
    boolean regen = false;

    public:
    KlimaatRegelaar(byte myLampenPin1, byte myLampenPin2, byte myNevelPin, byte myVentilatorPin, int myPlantenBakNummer) {
        plantenBakNummer = myPlantenBakNummer;
        lampenPin1 = myLampenPin1;
        lampenPin2 = myLampenPin2;
        nevelPin = myNevelPin;
        ventilatorPin = myNevelPin;
        pinMode(lampenPin1, OUTPUT);
        digitalWrite(lampenPin1, LOW);
        pinMode(lampenPin2, OUTPUT);
        digitalWrite(lampenPin1, LOW);
        pinMode(nevelPin, OUTPUT);
        digitalWrite(nevelPin, LOW);
        pinMode(ventilatorPin, OUTPUT);
        digitalWrite(nevelPin, LOW);
        digitalWrite(ventilatorPin, LOW);
        }

    void initialisatie() {
        Serial.println("KlimaatRegelaar geinitialiseerd");
        Serial.print("lampenPin1 = ");
        Serial.println(lampenPin1);
        Serial.print("lampenPin1 = ");
        Serial.println(lampenPin2);
        Serial.print("nevelPin = ");
        Serial.println(nevelPin);
        Serial.print("ventilatorPin = ");
        Serial.println(ventilatorPin);
        Serial.println();
    }

    void doeJeKlimaatDing(RtcDateTime now) {
        Serial.print("plantenBakNummer = ");
        Serial.println(plantenBakNummer);
        getSettingsNu(now);
        regelLicht();
        regelDauw();
        regelRegenWolken();
        regelVochtigheid();
        regelTemperatuur();
        standen();
    }
    void getSettingsNu(RtcDateTime now) {
        int seizoenNu = customPlantenBakSettings[plantenBakNummer][3][(now.Month()-1)];
        klimaatDataNu[plantenBakNummer][JAAR] = now.Year();
        klimaatDataNu[plantenBakNummer][MAAND] = now.Month();
        klimaatDataNu[plantenBakNummer][DAG] = now.Day();
        klimaatDataNu[plantenBakNummer][UUR] = now.Hour();
        klimaatDataNu[plantenBakNummer][MINUUT] = now.Minute();
        klimaatDataNu[plantenBakNummer][PLANTENBAKNUMMER] = plantenBakNummer;
        int uurNu = now.Second();// terugveranderen naat hour()
        int minuutNu = now.Minute();
        float uurMinuutNu = uurNu + (minuutNu / 60);
        Serial.print("uurMinuutNu = ");
        Serial.println(uurMinuutNu);
        boolean isDag;
        boolean isDauw;
        boolean isRegen;
        //kijken naar variabelen isdag/dag, isdauw/dauw, isregen/regen
        switch (seizoenNu) {
            case WINTER:
                for (int i = 0; i < 8; i++) {
                    klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][WINTER][i];
                }
                klimaatDataNu[plantenBakNummer][SEIZOEN] = WINTER;
                break;
            case ZOMER:
                for (int i = 0; i < 8; i++) {
                    klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][ZOMER][i];
            }
                klimaatDataNu[plantenBakNummer][SEIZOEN] = ZOMER;
                break;
            case REGEN:
                for (int i = 0; i < 8; i++) {
                    klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][REGEN][i];
                }
                klimaatDataNu[plantenBakNummer][SEIZOEN] = REGEN;
                break;
        }       
        float eindDag = klimaatDataNu[plantenBakNummer][STARTDAG] + uurMinuutNu <= klimaatDataNu[plantenBakNummer][DUURDAG];
        klimaatDataNu[plantenBakNummer][EINDDAG] = eindDag;
        if (uurMinuutNu >= klimaatDataNu[plantenBakNummer][STARTDAG] && uurMinuutNu <= eindDag) {
            isDag = true;
        }   else {
                isDag = false;
            }
        klimaatDataNu[plantenBakNummer][ISDAG] = isDag;
        float startDauw = klimaatDataNu[plantenBakNummer][STARTDAG] - klimaatDataNu[plantenBakNummer][DUURDAUW];
        klimaatDataNu[plantenBakNummer][STARTDAUW] = startDauw;
        if (uurMinuutNu >= startDauw && uurMinuutNu <= klimaatDataNu[plantenBakNummer][STARTDAG]) {
            isDauw = true;
        }   else {
                isDauw = false;
            }
        klimaatDataNu[plantenBakNummer][ISDAUW] = isDauw;
        float startBewolking = klimaatDataNu[plantenBakNummer][STARTDAG] + ((klimaatDataNu[plantenBakNummer][DUURDAG] - klimaatDataNu[plantenBakNummer][DUURREGEN]) / 2);
        float eindBewolking = startBewolking + klimaatDataNu[plantenBakNummer][DUURREGEN];
        klimaatDataNu[plantenBakNummer][STARTREGEN] = startBewolking;
        klimaatDataNu[plantenBakNummer][EINDREGEN] = eindBewolking;
        
        if (uurMinuutNu >= startBewolking && uurMinuutNu <= eindBewolking) {
            isRegen = true;
        }   else {
                isRegen = false;
            }
        klimaatDataNu[plantenBakNummer][ISREGEN] = isRegen;
    }

    void regelLicht() {
        if (!dag && klimaatDataNu[plantenBakNummer][ISDAG]) {
            digitalWrite(lampenPin1, HIGH);
            digitalWrite(lampenPin2, HIGH);
            Serial.println("Lampen aangeschakeld");
            dag = true;
            lampIsAan2 = true;
            klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            lampIsAan1 = true;
            klimaatDataNu[plantenBakNummer][LAMPENAAN1] = lampIsAan1;
        }
        if (dag && !klimaatDataNu[plantenBakNummer][ISDAG]) {
            digitalWrite(lampenPin1, LOW); 
            digitalWrite(lampenPin2, LOW); 
            Serial.println("Lampen uitgeschakeld");
            dag = false;
            lampIsAan2 = false;
            klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            lampIsAan1 = false;
            klimaatDataNu[plantenBakNummer][LAMPENAAN1] = lampIsAan1;
        }
    }

    void regelRegenWolken() {
        // In volgende regel 100 aanpassen op basis van resultaten
        if (!regen && klimaatDataNu[plantenBakNummer][ISREGEN] && klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] < 100) {
            if (!vernevelaarIsAan && (klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR])) {
                digitalWrite(nevelPin, HIGH);
                Serial.println("vernevelaar aan (regenwolken)");
                vernevelaarIsAan = true;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
                regen = true;
            }    
            if (lampIsAan2) {
                digitalWrite(lampenPin2, LOW);
                Serial.println("lampen2 uit (regenwolken)");
                lampIsAan2 = false;
                klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            }
        }

        if (regen && (!klimaatDataNu[plantenBakNummer][ISREGEN]||klimaatDataNu[plantenBakNummer][TEMPERATUURNU] < klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR])) {
            regen = false;
            if (vernevelaarIsAan && klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] > klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID]) {
                digitalWrite(nevelPin, LOW); 
                Serial.println("vernevelaar uit (regenwolken)"); 
                regen = false;
                vernevelaarIsAan = false;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
            }  
            if (!lampIsAan2 && klimaatDataNu[plantenBakNummer][ISDAG]) {
                digitalWrite(lampenPin2, HIGH); 
                Serial.println("lampen2 aan (regenwolken)"); 
                lampIsAan2 = true;
                klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            }
        }
    }
    
    void regelDauw() {
        if (!dauw && klimaatDataNu[plantenBakNummer][ISDAUW]) {
            if (!vernevelaarIsAan && klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR]) {
                digitalWrite(nevelPin, HIGH);
                Serial.println("vernevelaar aan (dauw)");
                vernevelaarIsAan = true;
                dauw = true;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
                }    
            if (!ventilatorIsAan && klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR]) {
                digitalWrite(ventilatorPin, HIGH);
                Serial.println("ventilator aan (dauw)");
                ventilatorIsAan = true;
                klimaatDataNu[plantenBakNummer][VENTILATORAAN] = ventilatorIsAan;
                }
        }

        if (dauw && (klimaatDataNu[plantenBakNummer][ISDAUW]|| klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR])) {
            if (vernevelaarIsAan && klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] > klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID]) {
                digitalWrite(nevelPin, LOW); 
                Serial.println("vernevelaar uit (dauw)"); 
                vernevelaarIsAan = false;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
            }  
            if (ventilatorIsAan) {
                digitalWrite(ventilatorPin, LOW); 
                Serial.println("ventilator uit (dauw)"); 
                ventilatorIsAan = false;
                klimaatDataNu[plantenBakNummer][VENTILATORAAN] = ventilatorIsAan;
            }
        dauw = false;
        }
    }

    void regelVochtigheid() {

        if (!luchtIsDroog && klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] < klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID] && klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR]) {
            luchtIsDroog = true;
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.println("vernevelaar aan (luchtvochtigheid)");
                vernevelaarIsAan = true;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
            } 
        }
        if (vernevelaarIsAan && !dauw && !regen && (klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU]  > klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID] || klimaatDataNu[plantenBakNummer][TEMPERATUURNU] < klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR] || klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] > 100)) {
            luchtIsDroog = false;
            digitalWrite(nevelPin, LOW);
            Serial.print("vernevelaar uit (luchtvochtigheid)");
            vernevelaarIsAan = false;
            klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
        }
        if (luchtIsDroog && klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] > klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID] ) { 
            luchtIsDroog = false;    
        }
        if (klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] > klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEID] && klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][NACHTTEMPERATUUR]) {
            luchtIsDroog = false;
            if (!ventilatorIsAan) {
                digitalWrite(ventilatorPin, HIGH);
                Serial.println("ventilator aan (luchtvochtigheid)");
                ventilatorIsAan = true;
                klimaatDataNu[plantenBakNummer][VENTILATORAAN] = ventilatorIsAan;
            } 
        }
    }

    void regelTemperatuur(){

        if (klimaatDataNu[plantenBakNummer][TEMPERATUURNU] > klimaatDataNu[plantenBakNummer][DAGTEMPERATUUR]) {
            if (!ventilatorIsAan) {
                digitalWrite(ventilatorPin, HIGH);
                Serial.print("vernevelaar aan (temperatuur)");
                ventilatorIsAan = true;
                klimaatDataNu[plantenBakNummer][VENTILATORAAN] = ventilatorIsAan;
            }
            if (lampIsAan2) {
                digitalWrite(lampenPin2, LOW);
                Serial.println("lampen2 uit (temperatuur)");
                lampIsAan2 = false;
                klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            }
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.print("vernevelaar aan (temperatuur)");
                vernevelaarIsAan = true;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
            }
        }
        if (klimaatDataNu[plantenBakNummer][TEMPERATUURNU] < klimaatDataNu[plantenBakNummer][DAGTEMPERATUUR]) {
            if (ventilatorIsAan && !dauw) {
                digitalWrite(ventilatorPin, LOW);
                Serial.print("vernevelaar uit (temperatuur)");
                ventilatorIsAan = false;
                klimaatDataNu[plantenBakNummer][VENTILATORAAN] = ventilatorIsAan;
            }
            if (!lampIsAan2 && !regen && dag) {
                digitalWrite(lampenPin2, HIGH);
                Serial.println("lampen2 aan (temperatuur)");
                lampIsAan2 = true;
                klimaatDataNu[plantenBakNummer][LAMPENAAN2] = lampIsAan2;
            }
            if (vernevelaarIsAan && !dauw && !regen && !luchtIsDroog) {
                digitalWrite(nevelPin, LOW);
                Serial.print("vernevelaar uit (temperatuur)");
                vernevelaarIsAan = false;
                klimaatDataNu[plantenBakNummer][VERNEVELAARAAN] = vernevelaarIsAan;
            }
        }
    }

    void standen() {
        Serial.print("plantenBakNummer = " );
        Serial.println(plantenBakNummer);
        Serial.print("isDag = ");
        Serial.println(dag);
        Serial.print("vernevelaarIsAan = ");
        Serial.println(vernevelaarIsAan);
        Serial.print("isDauw = ");
        Serial.println(dauw);
        Serial.print("ventilatorIsAan = ");
        Serial.println(ventilatorIsAan);
        Serial.print("isRegenWolk = ");
        Serial.println(regen);
        Serial.print("lampenAan2 = ");
        Serial.println(lampIsAan2);
        Serial.print("luchtIsDroog = ");
        Serial.println(luchtIsDroog);
        
    }
};


class Klok {
    
    RtcDS3231<TwoWire> Rtc;
        
    public:
    Klok() : 
        Rtc(Wire)
    {}

    void setup () {
        
        Serial.print("compiled: ");
        Serial.println(__DATE__);
        Serial.println(__TIME__);
        

        //--------RTC SETUP ------------
        // if you are using ESP-01 then uncomment the line below to reset the pins to
        // the available pins for SDA, SCL
        // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
        
        Rtc.Begin();

        RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
        Serial.print("Dit is de gecompileerde tijd: ");
        printDateTime(compiled);
        Serial.println();

        if (!Rtc.IsDateTimeValid())
        {
            if (Rtc.LastError() != 0)
            {
                // we have a communications error
                // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
                // what the number means
                Serial.print("RTC communications error = ");
                Serial.println(Rtc.LastError());
            }
            else
            {
                // Common Causes:
                //    1) first time you ran and the device wasn't running yet
                //    2) the battery on the device is low or even missing

                Serial.println("RTC lost confidence in the DateTime!");

                // following line sets the RTC to the date & time this sketch was compiled
                // it will also reset the valid flag internally unless the Rtc device is
                // having an issue

                Rtc.SetDateTime(compiled);
            }
        }

        if (!Rtc.GetIsRunning())
        {
            Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }

        RtcDateTime now = Rtc.GetDateTime();
        if (now < compiled) 
        {
            Serial.println("RTC is older than compile time!  (Updating DateTime)");
            Rtc.SetDateTime(compiled);
        }
        else if (now > compiled) 
        {
            Serial.println("RTC is newer than compile time. (this is expected)");
        }
        else if (now == compiled) 
        {
            Serial.println("RTC is the same as compile time! (not expected but all is fine)");
        }

        // never assume the Rtc was last configured by you, so
        // just clear them to your needed state
        Rtc.Enable32kHzPin(false);
        Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
        Serial.println("Klok geinitieerd");
        //String dateTime = geefDatumString(now);
        //return dateTime;
    }

    RtcDateTime getTime(){
        if (!Rtc.IsDateTimeValid()) 
        {
            if (Rtc.LastError() != 0) {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
            }   else {
                // Common Causes:
                //    1) the battery on the device is low or even missing and the power line was disconnected
                Serial.println("RTC lost confidence in the DateTime! Check battery");
            }
        }
        RtcDateTime now = Rtc.GetDateTime();
        return(now);
    }
    
    void printDateTime(const RtcDateTime& dt) {
        char datestring[20];

        snprintf_P(datestring, 
        countof(datestring),
        PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
        dt.Day(),
        dt.Month(),
        dt.Year(),
        dt.Hour(),
        dt.Minute(),
        dt.Second() );
        Serial.println(datestring);
    }
};


class Plantenbak {

    KlimaatRegelaar klimaatRegelaar;
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;
    boolean ventilatorIsUit = true;
    boolean vernevelaarIsUit = true;
    int plantenBakNummer;
    int maxPotVocht; //gelijkstellen aan getlal gemeten na ingebruikname
    int maxLicht; //gelijkstellen aan getlal gemeten na ingebruikname
    
    public:
    Plantenbak(byte (&myPins)[8], int myPlantenBakNummer) :
        soilHumiditySensor(myPins[0], myPins[1]),
        lichtSensor(myPins[2]),
        luchtVochtigheidTemperatuurSensor(myPins[6]),
        klimaatRegelaar(myPins[3] , myPins[7], myPins[5], myPins[4], myPlantenBakNummer),
        plantenBakNummer(myPlantenBakNummer)
    {
    }
    //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1

    void setup() {

        Serial.print("plantenBakNummer = ");
        Serial.println(plantenBakNummer);
        lichtSensor.initialisatie();
        soilHumiditySensor.initialisatie();
        luchtVochtigheidTemperatuurSensor.initialisatie();
        klimaatRegelaar.initialisatie();
        Serial.println("plantenbak geinitialseerd");
    }

    void regelKlimaat(RtcDateTime myTime, int plantenBakNummer) {
        Serial.print("plantenBakNummer in plantenBak regelKlimaat = ");
        Serial.println(plantenBakNummer);
        float luchtVochtigheid = luchtVochtigheidTemperatuurSensor.readHumidityValue();
        klimaatDataNu[plantenBakNummer][LUCHTVOCHTIGHEIDNU] = luchtVochtigheid;
        Serial.print("Humidity = ");
        Serial.println(luchtVochtigheid);
        Serial.println(luchtVochtigheidTemperatuurSensor.readHumidityValue());
        float temperatuur = luchtVochtigheidTemperatuurSensor.readTempValue();
        Serial.print(" %, Temp  ");
        klimaatDataNu[plantenBakNummer][TEMPERATUURNU] = temperatuur;
        //Serial.print(" %, Temp  ");
        Serial.print(temperatuur);
        Serial.println(" Celsius");
        if (isnan(luchtVochtigheid) || isnan(temperatuur)) {
            Serial.println(F("Failed to read from DHT sensor!"));
            return;
        }


        Serial.println(luchtVochtigheidTemperatuurSensor.readTempValue());
        int potVochtigheid = soilHumiditySensor.readValue();
        if(potVochtigheid > maxPotVocht) {
            maxPotVocht = potVochtigheid;
            klimaatDataNu[plantenBakNummer][HOOGSTEPOTVOCHTIGHEID] = potVochtigheid;
        }
        klimaatDataNu[plantenBakNummer][POTVOCHTIGHEIDNU] = potVochtigheid;
        //klimaatDataNu[plantenBakNummer][POTVOCHTIGHEIDNU] = (potVochtigheid/maxPotVocht)*99; //100% past niet op display
        Serial.print("Soil Moisture = ");  
        Serial.println(potVochtigheid);
        Serial.print("Soil Moisture = ");  
        Serial.println((potVochtigheid/maxPotVocht)*99);//100% past niet op display
        Serial.print("in klimaatdatanu ");
        Serial.println(klimaatDataNu[plantenBakNummer][POTVOCHTIGHEID]);
        int licht = lichtSensor.readLogValue();
        if(licht > maxLicht) {
            maxLicht = licht;
            klimaatDataNu[plantenBakNummer][MEESTELICHT] = licht;
        }
        //klimaatDataNu[plantenBakNummer][LICHT] = (licht/maxLicht)*99;//100% past niet op display
        klimaatDataNu[plantenBakNummer][LICHT] = licht;
        Serial.print("Licht = ");
        Serial.println(licht);
        Serial.print("in klimaatdatanu");
        Serial.println(klimaatDataNu[plantenBakNummer][LICHT]);
        Serial.println((licht/maxLicht)*99); //100% past niet op display
        klimaatRegelaar.doeJeKlimaatDing(myTime);
    }  
};


class DataUitwisselaarMaster {
  
  public:
  DataUitwisselaarMaster() {
    digitalWrite(SS, HIGH);  // ensure SS stays high for now
    SPI.begin ();// Put SCK, MOSI, SS pins into output mode// also put SCK, MOSI into LOW state, and SS into HIGH state// Then put SPI hardware into Master mode and turn SPI on
    SPI.setClockDivider(SPI_CLOCK_DIV8); // Slow down the master a bit
  }  
  void zendOntvangData(){
    byte b;
    digitalWrite(SS, LOW);// enable Slave Select
    delayMicroseconds (200);
    byte z = SPI.transfer (0xCD); //Verzend startcode 0xCD voor Slave
    delayMicroseconds(20); //give the slave time to process
    byte x = SPI.transfer (0xF3); //0xEF is pumped to get response byte from slave
    delayMicroseconds(20); //give the slave time to process
      if (x == 0xEF){
        for ( byte i = 0 ; i < 3 ; i++){
          for (byte j = 0 ; j < 31 ; j++){
            b = (klimaatDataNu[i][j]);
            byte y = SPI.transfer (b);
            delayMicroseconds(20); //give the slave time to process
            klimaatDataNu[i][j] = y;
            Serial.print(klimaatDataNu[i][j]);
          }
          Serial.println();
        }
      }
  
    digitalWrite(SS, HIGH); // disable Slave Select
    Serial.println();
    Serial.println("================= end transfer");
  }
};

Klok klok;
DataUitwisselaarMaster dataUitwisselaarMaster;

int bakNummer1 = 0;
Plantenbak plantenbak1(pinArray1, bakNummer1);
int bakNummer2 = 1;
Plantenbak plantenbak2(pinArray2, bakNummer2);
int bakNummer3 = 2;
Plantenbak plantenbak3(pinArray3, bakNummer3);

void setup() {
    Serial.begin(9600);
    analogReference(EXTERNAL); 
    klok.setup();
    // touchScreen.setup();
    // klimaatDataLogger.setup();
    plantenbak1.setup();
    plantenbak2.setup();
    plantenbak3.setup();
    SPI.begin();                            //Begins the SPI commnuication
    SPI.setClockDivider(SPI_CLOCK_DIV8);    //Sets clock for SPI communication at 8 (16/8=2Mhz)
    digitalWrite(SS,HIGH);                  // Setting SlaveSelect as HIGH (So master doesnt connnect with slave)
}

void loop() {
    
    Serial.println("begin main");
    
    RtcDateTime tijd = klok.getTime();
    plantenbak1.regelKlimaat(tijd, bakNummer1);
    plantenbak2.regelKlimaat(tijd, bakNummer2);
    plantenbak3.regelKlimaat(tijd, bakNummer3);
    dataUitwisselaarMaster.zendOntvangData();
    delay(2000);////DEZE MOET ERUIT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Serial.println();
    Serial.print("end of loop Master");   
}
