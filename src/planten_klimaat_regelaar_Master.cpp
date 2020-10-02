#include <Arduino.h>
#include <SD.h>
#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>  
#include <SPI.h>
#include <RtcDS3231.h>
#include <Wire.h> // must be included here so that Arduino library object file references work (clock)



#define MINPOTVOCHTIGHEID 0
#define DUURDAUW 1
#define DUURREGEN 2
#define DUURBEWOLKING 3
#define DUURDAG 4

#define DAGTEMPERATUUR 5
#define NACHTTEMPERATUUR 6
#define LUCHTVOCHTIGHEID 7
#define RESERVE1 8
#define RESERVE2 9
#define ISDAG 10
#define ISDAUW 11
#define ISBEWOLKING 12
#define ISREGEN 13
#define ISPOMP 14

#define TEMPERATUURNU 15
#define LUCHTVOCHTIGHEIDNU 16
#define POTVOCHTIGHEIDNU 17
#define LICHTNU 18
#define WATERSTANDNU 19
#define LAMPENAAN1 20
#define LAMPENAAN2 21
#define VENTILATORAAN 22
#define VERNEVELAARAAN 23
#define RESERVE3 24

#define PLANTENBAKNUMMER 25
#define SEIZOEN 26
#define JAAR 27
#define MAAND 28
#define DAG 29
#define UUR 30
#define MINUUT 31
#define HOOGSTEPOTVOCHTIGHEID 32
#define MEESTELICHT 33
#define WATERGEVEN 34
#define LAMPENVERVANGEN 35
#define ZONOP 36
#define ZONONDER 37

#define ZOMER 1
#define REGEN 2
#define WINTER 0

#define CHIANGMAI 0
#define MANAUS 1
#define SUMATRA 2

#define countof(a) (sizeof(a) / sizeof(a[0])) //deze regel is voor tijdweergave klok

extern uint8_t BigFont[];
extern uint8_t SmallFont[];
String datumTijd = "Nog niet";
int currentPage; //indicates the page that is active on touchscreen

int SS2 = 10; //Slave select pin voor SPI

byte defaultPlantenBakSettings[3][4][12] = {{{25, 14, 60, 20, 75, 3, 0, 0}, {35, 28, 55, 10, 75, 2, 0, 0}, {11, 22, 33, 44, 55, 66, 77, 88, 99}, {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0}}, {{8, 21, 20, 14, 4, 0, 70, 1}, {8, 22, 30, 20, 4, 1, 55, 1}, {8, 23, 35, 30, 4, 5, 85, 1}, {1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1}}, {{8, 21, 20, 14, 4, 0, 70, 2}, {8, 22, 30, 20, 4, 1, 55, 2}, {8, 23, 35, 30, 4, 5, 85, 2}, {2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2}}};
byte customPlantenBakSettings[3][4][12] = {{{11, 22, 33, 44, 55, 66, 77, 88, 99}, {35, 28, 55, 10, 75, 2, 0, 0}, {30, 25, 35, 80, 4, 5, 85, 4, 1,13, 8}, {0, 0, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0}}, {{8, 21, 20, 14, 4, 0, 70, 1, 20, 75}, {11, 22, 33, 44, 55, 66, 77, 88, 99}, {8, 23, 35, 30, 4, 5, 85, 1, 20, 75}, {1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1}}, {{8, 21, 20, 14, 4, 0, 70, 2, 20, 75}, {8, 22, 30, 20, 4, 1, 55, 2, 20, 75}, {11, 22, 33, 44, 55, 66, 77, 88, 99}, {2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2}}};
byte klimaatDataNu[3][38]= {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}};

class DefineSettingsNu {

    public:
    DefineSettingsNu() {}
    
    static void getSettingsNu() {
        Serial.print("in get settingsnu");
        for (int plantenBakNummer = 0; plantenBakNummer < 3; plantenBakNummer++){
            klimaatDataNu[plantenBakNummer][PLANTENBAKNUMMER] = plantenBakNummer;
            int seizoenNu = customPlantenBakSettings[plantenBakNummer][3][(klimaatDataNu[plantenBakNummer][MAAND])-1];
            klimaatDataNu[plantenBakNummer][SEIZOEN] = seizoenNu;
            int uurNu = klimaatDataNu[plantenBakNummer][UUR];
            int minuutNu = klimaatDataNu[plantenBakNummer][MINUUT];
            float uurMinuutNu = uurNu + (minuutNu / 60);
                        
            boolean isDag;
            boolean isDauw;
            boolean isRegen;
            boolean isBewolking;
            
            switch (seizoenNu) {
                case WINTER:
                    for (int i = 0; i < 10; i++) {
                        klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][WINTER][i];
                        Serial.print("Winter in get settings");
                    }
                    break;
                case ZOMER:
                    for (int i = 0; i < 10; i++) {
                        klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][ZOMER][i];
                        Serial.print("Zomer in get settings");
                }
                    break;
                case REGEN:
                    for (int i = 0; i < 10; i++) {
                        klimaatDataNu[plantenBakNummer][i] = customPlantenBakSettings[plantenBakNummer][REGEN][i];
                        Serial.print("Regen in get settings");
                    }
                    break;
            }       
            float eindDag = klimaatDataNu[plantenBakNummer][DUURDAG] + 8;
            //klimaatDataNu[plantenBakNummer][EINDDAG] = eindDag;
            if(uurMinuutNu >= 8 && uurMinuutNu <= eindDag) {
                isDag = true;
            }   else {
                    isDag = false;
                }
            klimaatDataNu[plantenBakNummer][ISDAG] = isDag;
            
            float startDauw = 8 - klimaatDataNu[plantenBakNummer][DUURDAUW];
            //klimaatDataNu[plantenBakNummer][STARTDAUW] = startDauw;
            if (uurMinuutNu >= startDauw && uurMinuutNu <= 8) {
                isDauw = true;
            }   else {
                    isDauw = false;
                }
            klimaatDataNu[plantenBakNummer][ISDAUW] = isDauw;

            float startRegen = 8 + ((klimaatDataNu[plantenBakNummer][DUURDAG] - klimaatDataNu[plantenBakNummer][DUURREGEN]) / 2);
            float eindRegen = startRegen + klimaatDataNu[plantenBakNummer][DUURREGEN];
            // klimaatDataNu[plantenBakNummer][STARTREGEN] = startRegen;
            // klimaatDataNu[plantenBakNummer][EINDREGEN] = eindRegen;
            if (uurMinuutNu >= startRegen && uurMinuutNu <= eindRegen) {
                isRegen = true;
            }   else {
                    isRegen = false;
                }
            klimaatDataNu[plantenBakNummer][ISREGEN] = isRegen;

            float startBewolking = 8 + ((klimaatDataNu[plantenBakNummer][DUURDAG] - klimaatDataNu[plantenBakNummer][DUURBEWOLKING]) / 2);
            float eindBewolking = startBewolking + klimaatDataNu[plantenBakNummer][DUURBEWOLKING];
            // klimaatDataNu[plantenBakNummer][STARTBEWOLKING] = startBewolking;
            // klimaatDataNu[plantenBakNummer][EINDBEWOLKING] = eindBewolking;
            if (uurMinuutNu >= startBewolking && uurMinuutNu <= eindBewolking) {
                isBewolking = true;
            }   else {
                    isBewolking = false;
                }
            //klimaatDataNu[plantenBakNummer][ISBEWOLKING] = isBewolking;
        }
        Serial.println("klimaatDataNu");
        for ( byte i = 0 ; i < 3 ; i++){
            for (byte j = 0 ; j < 33 ; j++){
                Serial.print(klimaatDataNu[i][j]);
            }
            Serial.println();
        }  

    }
};

class TouchScreen {
    
    UTFT myGLCD;
    URTouch myTouch;
    UTFT_Buttons myButtons;
    
    int x, y;
    char stCurrent[2]="";
    int stCurrentLen=0;
    char stLast[2]=""; 

    public:
    TouchScreen() :
    myGLCD(CTE32_R2,38,39,40,41),
    myTouch( 6, 5, 4, 3, 2),
    myButtons(&myGLCD, &myTouch)
    {}

    void setup() {
        
        myGLCD.InitLCD();
        myGLCD.clrScr();
        myGLCD.setFont(BigFont);
        myTouch.InitTouch();
        myTouch.setPrecision(PREC_MEDIUM);
        currentPage = 1; // Indicates wich screen is active
    }

    void toonStartScherm(String myDatumTijd) {
        Serial.println("begin toonstartscherm");
        DefineSettingsNu::getSettingsNu();
        currentPage = 1;
        Serial.print("currentPage = ");
        Serial.println(currentPage);
        myGLCD.clrScr();
        int seizoen;
        for (int i = 0; i < 3; i++) {
            switch (klimaatDataNu[i][SEIZOEN]) {
                case WINTER :
                    myGLCD.setColor(VGA_BLUE);
                    myGLCD.setBackColor(VGA_BLUE);
                    seizoen = WINTER;
                    break;
                case ZOMER :
                    myGLCD.setColor(VGA_RED);
                    myGLCD.setBackColor(VGA_RED);
                    seizoen = ZOMER;
                    break;
                case REGEN : 
                    myGLCD.setColor(VGA_GRAY);
                    myGLCD.setBackColor(VGA_GRAY);
                    seizoen = REGEN;
                    break;
            }
            Serial.print("het seizoen is:");
            Serial.println(seizoen);
            myGLCD.fillRoundRect(2, (i*71) + 2, 318, (i*71) + 71);
            myGLCD.setColor(VGA_WHITE);
            myGLCD.drawRoundRect(2, (i*71) + 2, 318, (i*71) + 71);
            myGLCD.setFont(SmallFont);
            myGLCD.print(String("Temperature"), 6, (i*71) +6);
            myGLCD.print(String("Humidity"), 6, (i*71) + 39);
            myGLCD.print(String("Soil"), 140, (i*71) + 6);
            myGLCD.print(String("Light"),140, (i*71) + 39);
            myGLCD.print(String(customPlantenBakSettings[i][seizoen][NACHTTEMPERATUUR]) + "/", 6, (i*71) + 21);
            myGLCD.print(String("/") + String(customPlantenBakSettings[i][seizoen][DAGTEMPERATUUR]), 76, (i*71) + 21);
            myGLCD.print(String("/") + String(customPlantenBakSettings[i][seizoen][LUCHTVOCHTIGHEID]), 53, (i*71) + 53);
            myGLCD.print(String("/") + String(customPlantenBakSettings[i][seizoen][WATERGEVEN]), 190, (i*71) + 21);
            myGLCD.print(String("/") + String(customPlantenBakSettings[i][seizoen][LAMPENVERVANGEN]), 190, (i*71) + 53);
            myGLCD.setColor(VGA_YELLOW);
            if(klimaatDataNu[i][ISDAUW] == 1) {
                myGLCD.print(String("Dew"), 265, (i*71) + 21);
            }
            if(klimaatDataNu[i][ISREGEN] == 1) {
                myGLCD.print(String("Rain"), 265, (i*71) + 39);
            }
            if(klimaatDataNu[i][ISDAG] == 1) {
                myGLCD.print(String("Day"), 265, (i*71) + 6);
            }   else {
                myGLCD.print(String("Night"), 265, (i*71) + 6);
            }
            if (klimaatDataNu[i][SEIZOEN] == WINTER) {
                myGLCD.print(String("Winter"), 265, (i*71) + 55);
            }   
            if (klimaatDataNu[i][SEIZOEN] == ZOMER) {
                myGLCD.print(String("Summer"), 265, (i*71) + 55);
            } 
            if (klimaatDataNu[i][SEIZOEN] == REGEN) {
                myGLCD.print(String("Wet"), 265, (i*71) + 55);
            }
            myGLCD.setFont(BigFont);
            myGLCD.setColor(VGA_YELLOW);
            myGLCD.print(String(klimaatDataNu[i][TEMPERATUURNU]), 28, (i*71) + 18);
            myGLCD.print(String("C"), 60, (i*71) + 18);
            myGLCD.fillCircle(61, (i*71) + 19, 2);
            myGLCD.print(String(klimaatDataNu[i][LUCHTVOCHTIGHEIDNU]) + "%", 5, (i*71) + 51);
            myGLCD.print(String(klimaatDataNu[i][MINPOTVOCHTIGHEID]) + "%", 140, (i*71) + 18);
            myGLCD.print(String(klimaatDataNu[i][LICHTNU]) + "%", 140, (i*71) + 51);
        }
        myGLCD.setColor(VGA_WHITE);
        myGLCD.setBackColor(VGA_BLACK);
        myGLCD.setFont(BigFont);
        myGLCD.print(myDatumTijd, CENTER, 220);
    }

    void tekenSettingsOverzicht(int bak) {
        myGLCD.clrScr();
        for (int i = 0; i < 3; i++) {
            if(i == 0) {
                myGLCD.setColor(VGA_WHITE);
                myGLCD.setFont(BigFont);
                myGLCD.print("WINTER", 205, 51 + (i*71));
                myGLCD.setColor(VGA_BLUE);
                myGLCD.setBackColor(VGA_BLUE);
            }  
            if(i == 1) { 
                myGLCD.setColor(VGA_WHITE);
                myGLCD.setFont(BigFont);
                myGLCD.print("SUMMER", 205, 51 + (i*71));
                myGLCD.setColor(VGA_RED);
                myGLCD.setBackColor(VGA_RED);
            }
            if(i == 2) { 
                myGLCD.setColor(VGA_WHITE);
                myGLCD.setFont(BigFont);
                myGLCD.print("RAIN", 221, 51 + (i*71));
                myGLCD.setColor(VGA_GRAY);
                myGLCD.setBackColor(VGA_GRAY);
            }
            myGLCD.fillRoundRect(1, (i*71) + 1, 318, (i*71) + 71);
            myGLCD.setColor(VGA_WHITE);
            myGLCD.drawRoundRect (1, (i*71) + 1, 318, (i*71) + 71);
            myGLCD.setFont(SmallFont);
            myGLCD.print(String("Temperature"), 6, (i*71) + 2);
            myGLCD.print(String("Humidity"), 6, (i*71) + 43);
            myGLCD.print(String("Soil"), 140, (i*71) + 2);
            myGLCD.print(String("Light"),140, (i*71) + 43);
            myGLCD.print(String("Daylight"), 200, (i*71) + 2);
            myGLCD.print(String("Dew"), 250, (i*71) + 43);
            myGLCD.print(String("Clouds"),190, (i*71) + 43);
            myGLCD.print(String("Rain"),280, (i*71) + 43);
            myGLCD.setFont(SmallFont);
            myGLCD.print(String("min"), 6, (i*71) + 16);
            myGLCD.print(String("max"), 63, (i*71) + 16);
            myGLCD.print(String("start"), 232, (i*71) + 16);
            myGLCD.print(String("average"), 6, (i*71) + 58);
            myGLCD.setFont(BigFont);
            myGLCD.setColor(VGA_YELLOW);
            myGLCD.print(String(customPlantenBakSettings[bak][i][NACHTTEMPERATUUR]), 29, (i*71) + 13);
            myGLCD.print(String(customPlantenBakSettings[bak][i][DAGTEMPERATUUR]), 85, (i*71) + 13);
            myGLCD.print(String(customPlantenBakSettings[bak][i][DUURDAG]), 200, (i*71) + 13);
            myGLCD.print(String(8)+ String("H"), 270, (i*71) + 13);
            myGLCD.print(String("C"), 115, (i*71) + 13);
            myGLCD.fillCircle(116, (i*71) + 15, 2);
            myGLCD.print(String(customPlantenBakSettings[bak][i][LUCHTVOCHTIGHEID]) + "%", 60, (i*71) + 55);
            myGLCD.print(String(customPlantenBakSettings[bak][i][WATERGEVEN]) + "%", 140, (i*71) + 13);
            myGLCD.print(String(customPlantenBakSettings[bak][i][LAMPENVERVANGEN]) + "%", 140, (i*71) + 55);
            myGLCD.print(String(customPlantenBakSettings[bak][i][DUURREGEN]) + "H", 280, (i*71) + 55);
            myGLCD.print(String(customPlantenBakSettings[bak][i][DUURDAUW]) + "H", 240, (i*71) + 55);
            myGLCD.print(String(customPlantenBakSettings[bak][i][DUURBEWOLKING]) + "H", 200, (i*71) + 55);
            myGLCD.setFont(SmallFont);
            for(int j = 0; j <12; j++) {
                myGLCD.setFont(SmallFont);
                switch (customPlantenBakSettings[bak][3][j]) {
                case WINTER :
                    myGLCD.setColor(0,0,225);
                    myGLCD.setBackColor(0,0,225);
                    myGLCD.fillRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
                    myGLCD.setColor(VGA_WHITE);
                    if(j== 0)
                        myGLCD.print(String("jan"), 6, (i*71) + 29);
                    if(j==1) 
                        myGLCD.print(String("feb"), 32, (i*71) + 29);
                    if(j==2) 
                        myGLCD.print(String("mar"), 58, (i*71) + 29);
                    if(j==3)
                        myGLCD.print(String("apr"), 84, (i*71) + 29);
                    if(j==4)
                        myGLCD.print(String("may"), 110, (i*71) + 29);
                    if(j==5)
                        myGLCD.print(String("jun"), 136, (i*71) + 29);
                    if(j==6)
                        myGLCD.print(String("jul"), 162, (i*71) + 29);
                    if(j==7)
                        myGLCD.print(String("aug"), 188, (i*71) + 29);
                    if(j==8)
                        myGLCD.print(String("sep"), 214, (i*71) + 29);
                    if(j==9)
                        myGLCD.print(String("oct"), 240, (i*71) + 29);
                    if(j==10)
                        myGLCD.print(String("nov"), 266, (i*71) + 29);
                    if(j==11)
                        myGLCD.print(String("dec"), 292, (i*71) + 29);
                    break;
                case ZOMER :
                    myGLCD.setColor(235,0,0);
                    myGLCD.setBackColor(235,0,0);
                    myGLCD.fillRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
                    myGLCD.setColor(VGA_WHITE);
                    if(j== 0) 
                        myGLCD.print("jan", 6, (i*71) + 29);
                    if(j==1)
                        myGLCD.print("feb", 32, (i*71) + 29);
                    if(j==2)
                        myGLCD.print("mar", 58, (i*71) + 29);
                    if(j==3)
                        myGLCD.print("apr", 84, (i*71) + 29);
                    if(j==4)
                        myGLCD.print("may", 110, (i*71) + 29);
                    if(j==5)
                        myGLCD.print("jun", 136, (i*71) + 29);
                    if(j==6)
                        myGLCD.print("jul", 162, (i*71) + 29);
                    if(j==7)
                        myGLCD.print("aug", 188, (i*71) + 29);
                    if(j==8)
                        myGLCD.print("sep", 214, (i*71) + 29);
                    if(j==9)
                        myGLCD.print("oct", 240, (i*71) + 29);
                    if(j==10)
                        myGLCD.print("nov", 266, (i*71) + 29);
                    if(j==11)
                        myGLCD.print("dec", 292, (i*71) + 29);
                    break;
                case REGEN : 
                    myGLCD.setColor(VGA_SILVER);
                    myGLCD.setBackColor(VGA_SILVER);
                    myGLCD.fillRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
                    myGLCD.setColor(VGA_WHITE);
                    if(j== 0)
                        myGLCD.print("jan", 6, (i*71) + 29);
                    if(j==1)
                        myGLCD.print("feb", 32, (i*71) + 29);
                    if(j==2)
                        myGLCD.print("mar", 58, (i*71) + 29);
                    if(j==3)
                        myGLCD.print("apr", 84, (i*71) + 29);
                    if(j==4)
                        myGLCD.print("may", 110, (i*71) + 29);
                    if(j==5)
                        myGLCD.print("jun", 136, (i*71) + 29);
                    if(j==6)
                        myGLCD.print("jul", 162, (i*71) + 29);
                    if(j==7)
                        myGLCD.print("aug", 188, (i*71) + 29);
                    if(j==8)
                        myGLCD.print("sep", 214, (i*71) + 29);
                    if(j==9)
                        myGLCD.print("oct", 240, (i*71) + 29);
                    if(j==10)
                        myGLCD.print("nov", 266, (i*71) + 29);
                    if(j==11)
                        myGLCD.print("dec", 292, (i*71) + 29);
                    break;
                }
                myGLCD.setColor(VGA_WHITE);
                myGLCD.drawRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
            }          
        } 
    }

    void tekenSettingsManipulatieScherm (int bak) {
        tekenSettingsOverzicht(bak);
        int variable;
        currentPage = 3;
        Serial.print(" currentPage = ");
        Serial.print(currentPage);
        myGLCD.setColor(VGA_BLACK);
        myGLCD.setBackColor(VGA_BLACK);
        myGLCD.fillRoundRect(2, 215, 80, 238);
        myGLCD.fillRoundRect(82, 215, 160, 238);
        myGLCD.fillRoundRect(162, 215, 240, 238);
        myGLCD.fillRoundRect(242, 215, 318, 238);
        myGLCD.setColor(VGA_WHITE);
        myGLCD.drawRoundRect(2, 215, 80, 238);
        myGLCD.drawRoundRect(82, 215, 160, 238);
        myGLCD.drawRoundRect(162, 215, 240, 238);
        myGLCD.drawRoundRect(242, 215, 318, 238);
        myGLCD.print("Back", 266, 221);
        myGLCD.print("Chiang", 5, 221);
        myGLCD.print("Mai", 58, 221);
        myGLCD.print("Manaus", 99, 221);
        myGLCD.print("Sumatra", 175, 221);
        while (currentPage == 3) {
            if (currentPage == 3 && myTouch.dataAvailable()) {
                myTouch.read();
                x=myTouch.getX();
                y=myTouch.getY();
                if ((y>=210) && (y<=240)) {
                    if ((x>=0) && (x<=80)) {
                        betast(2, 215, 80, 238);
                        for(int i=0; i<4; i++) {
                            for(int j=0; j<12; j++) {
                                customPlantenBakSettings[bak][i][j] = defaultPlantenBakSettings[CHIANGMAI][i][j];
                            }
                        }
                        tekenSettingsManipulatieScherm(bak);
                        //toonStartScherm("changmai");
                    }
                    if ((x>=80) && (x<=160)) {
                        betast(82, 215, 160, 238);
                        for(int i=0; i<3; i++) {
                            for(int j=0; j<12; j++) {
                                customPlantenBakSettings[bak][i][j] = defaultPlantenBakSettings[MANAUS][i][j];
                            }
                        }
                        //toonStartScherm("minaus");
                        tekenSettingsManipulatieScherm(bak);
                    }
                    if ((x>=160) && (x<=240)) {
                        betast(162, 215, 240, 238);
                        for(int i=0; i<3; i++) {
                            for(int j=0; j<12; j++) {
                                customPlantenBakSettings[bak][i][j] = defaultPlantenBakSettings[SUMATRA][i][j];
                            }
                        }
                        tekenSettingsManipulatieScherm(bak);
                    }
                    if ((x>=240) && (x<=320)) {
                        betast(242, 215, 318, 238);
                        String test = "back pressed";    //terugveranderen                        
                        //DefineSettingsNu::getSettingsNu();
                        Serial.println("BACK GEDRUKT IN SETTINGSSCHERM");
                        Serial.println("customPlantenBakSettings");
                        for ( int i = 0 ; i < 3 ; i++){
                            for(int k = 0; k < 4; k++){
                                for (byte j = 0 ; j < 12 ; j++){
                                    Serial.print(customPlantenBakSettings[i][k][j]);
                                    Serial.print("/");
                                }  
                            Serial.print("/////");      
                            }
                        Serial.println();
                        }
                        toonStartScherm(test);//VERVANGEN DOOR BREAK IN KIES PLANTENBAK??????
                        currentPage = 0;
                        break;
                    }
                }
                for(int i=0;i<3;i++) {
                    int seizoen = i;
                    if ((y<=(i*71) + 29) && (y>=(i*71) + 13) && (x>=29 && x<=61)) { 
                    drawButtons();
                    variable = NACHTTEMPERATUUR;
                    leesGetal(bak, seizoen, variable);
                    }
                    if ((y<=(i*71) + 29) && (y>=(i*71) + 13) && (x>=85 && x<=117)) { 
                    drawButtons();
                    variable = DAGTEMPERATUUR;
                    leesGetal(bak, seizoen, variable);
                    }
                    if ((y<=(i*71) + 29) && (y>=(i*71) + 13) && (x>=200 && x<=232)) { 
                    drawButtons();
                    variable = DUURDAG;
                    leesGetal(bak, seizoen, variable);
                    }
                    if ((y<=(i*71) + 29) && (y>=(i*71) + 13) && (x>=270 && x<=338)) { 
                    drawButtons();
                    //variable = STARTDAG;
                    leesGetal(bak, seizoen, variable);
                    }
                    if ((y<=(i*71) + 29) && (y>=(i*71) + 13) && (x>=140 && x<=172)) { 
                    drawButtons();
                    variable = WATERGEVEN;
                    leesGetal(bak, seizoen, variable);
                    }
                    if ((y<=(i*71) + 71) && (y>=(i*71) + 55) && (x>=60 && x<=92)) { 
                    drawButtons();
                    variable = LUCHTVOCHTIGHEID;
                    leesGetal(bak, seizoen, variable);
                    }
                }
                int k = 0;
                for(int j = 0; j <12; j++) {
                    if (y>=(k*71) + 29 && y<=(k*71) + 42 && x>=(j*26) + 4 && x<=(j*26) + 30) { 
                        Serial.print("maand geselecteerd");
                        betast((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                        customPlantenBakSettings[bak][3][j] = k;
                        myGLCD.setColor(VGA_BLUE);
                        myGLCD.setBackColor(VGA_BLUE);
                        for (int i = 0; i < 3; i++) {
                            myGLCD.fillRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
                            myGLCD.setColor(VGA_WHITE);
                            if(j== 0)
                                myGLCD.print("jan", 6, (i*71) + 29);
                            if(j==1) 
                                myGLCD.print("feb", 32, (i*71) + 29);
                            if(j==2) 
                                myGLCD.print("mar", 58, (i*71) + 29);
                            if(j==3)
                                myGLCD.print("apr", 84, (i*71) + 29);
                            if(j==4)
                                myGLCD.print("may", 110, (i*71) + 29);
                            if(j==5)
                                myGLCD.print("jun", 136, (i*71) + 29);
                            if(j==6)
                                myGLCD.print("jul", 162, (i*71) + 29);
                            if(j==7)
                                myGLCD.print("aug", 188, (i*71) + 29);
                            if(j==8)
                                myGLCD.print("sep", 214, (i*71) + 29);
                            if(j==9)
                                myGLCD.print("oct", 240, (i*71) + 29);
                            if(j==10)
                                myGLCD.print("nov", 266, (i*71) + 29);
                            if(j==11)
                                myGLCD.print("dec", 292, (i*71) + 29);
                        
                        }
                    }
               // myGLCD.setColor(VGA_WHITE);
               // myGLCD.drawRoundRect((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                }
                k = 1;
                for(int j = 0; j <12; j++) {
                    if (y>=(k*71) + 29 && y<=(k*71) + 42 && x>=(j*26) + 4 && x<=(j*26) + 30) { 
                        Serial.print("maand geselecteerd");
                        betast((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                        customPlantenBakSettings[bak][3][j] = k;
                        myGLCD.setColor(VGA_RED);
                        // myGLCD.setBackColor(235,0,0);
                        myGLCD.setBackColor(VGA_RED);
                        for (int i = 0; i < 3; i++) {
                            myGLCD.drawRoundRect((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                            myGLCD.setColor(VGA_WHITE);
                            if(j== 0) 
                                myGLCD.print("jan", 6, (i*71) + 29);
                            if(j==1)
                                myGLCD.print("feb", 32, (i*71) + 29);
                            if(j==2)
                                myGLCD.print("mar", 58, (i*71) + 29);
                            if(j==3)
                                myGLCD.print("apr", 84, (i*71) + 29);
                            if(j==4)
                                myGLCD.print("may", 110, (i*71) + 29);
                            if(j==5)
                                myGLCD.print("jun", 136, (i*71) + 29);
                            if(j==6)
                                myGLCD.print("jul", 162, (i*71) + 29);
                            if(j==7)
                                myGLCD.print("aug", 188, (i*71) + 29);
                            if(j==8)
                                myGLCD.print("sep", 214, (i*71) + 29);
                            if(j==9)
                                myGLCD.print("oct", 240, (i*71) + 29);
                            if(j==10)
                                myGLCD.print("nov", 266, (i*71) + 29);
                            if(j==11)
                                myGLCD.print("dec", 292, (i*71) + 29);
                        }
                    }
                // myGLCD.setColor(VGA_WHITE);
                // myGLCD.drawRoundRect((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                }
                k = 2;
                for(int j = 0; j <12; j++) {
                    if (y>=(k*71) + 29 && y<=(k*71) + 42 && x>=(j*26) + 4 && x<=(j*26) + 30) { 
                        Serial.print("maand geselecteerd");
                        betast((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                        customPlantenBakSettings[bak][3][j] = k;
                        myGLCD.setColor(VGA_GRAY);
                        myGLCD.setBackColor(VGA_SILVER);
                        for (int i = 0; i < 3; i++) {
                            myGLCD.fillRoundRect((j*26) + 4, (i*71) + 29, (j*26) + 30, (i*71) + 42);
                            myGLCD.setColor(VGA_WHITE);
                            if(j== 0)
                                myGLCD.print("jan", 6, (i*71) + 29);
                            if(j==1)
                                myGLCD.print("feb", 32, (i*71) + 29);
                            if(j==2)
                                myGLCD.print("mar", 58, (i*71) + 29);
                            if(j==3)
                                myGLCD.print("apr", 84, (i*71) + 29);
                            if(j==4)
                                myGLCD.print("may", 110, (i*71) + 29);
                            if(j==5)
                                myGLCD.print("jun", 136, (i*71) + 29);
                            if(j==6)
                                myGLCD.print("jul", 162, (i*71) + 29);
                            if(j==7)
                                myGLCD.print("aug", 188, (i*71) + 29);
                            if(j==8)
                                myGLCD.print("sep", 214, (i*71) + 29);
                            if(j==9)
                                myGLCD.print("oct", 240, (i*71) + 29);
                            if(j==10)
                                myGLCD.print("nov", 266, (i*71) + 29);
                            if(j==11)
                                myGLCD.print("dec", 292, (i*71) + 29);
                        }
                    }
                // myGLCD.setColor(VGA_WHITE);
                // myGLCD.drawRoundRect((j*26) + 4, (k*71) + 29, (j*26) + 30, (k*71) + 42);
                }
            }
        }        
    }       
    
    void tekenSettingsScherm(int bak) {
        tekenSettingsOverzicht(bak);
        Serial.println("begin tekenSettingsscherm");
        currentPage = 2;
        Serial.print("currentPage = ");
        Serial.println(currentPage);
            myGLCD.setColor(VGA_BLACK);
            myGLCD.setBackColor(VGA_BLACK);
            myGLCD.fillRoundRect(2, 215, 80, 238);
            myGLCD.fillRoundRect(82, 215, 160, 238);
            myGLCD.fillRoundRect(162, 215, 240, 238);
            myGLCD.fillRoundRect(242, 215, 318, 238);
            myGLCD.setColor(VGA_WHITE);
            myGLCD.drawRoundRect(2, 215, 80, 238);
            myGLCD.drawRoundRect(82, 215, 160, 238);
            myGLCD.drawRoundRect(162, 215, 240, 238);
            myGLCD.drawRoundRect(242, 215, 318, 238);
            myGLCD.print("Back", 266, 221);
            myGLCD.print("Settings", 10, 221);
            while (currentPage == 2) {
                String test = "opnieuw";
                if (currentPage ==2 && myTouch.dataAvailable()) {
                    myTouch.read();
                    x=myTouch.getX();
                    y=myTouch.getY();
                    if ((y>=210) && (y<=240)) { 
                        if ((x>=0) && (x<=80)) {
                            betast(2, 215, 80, 238);
                            tekenSettingsManipulatieScherm(bak);
                            Serial.println("TERUG UIT SETTINGSMANIPULATIESCHERM IN SETTINGS OVERZICHT");
                            //delay(5000);
                            break;
                        }
                        if ((x>=80) && (x<=160)) {
                            betast(82, 215, 160, 238);
                            break;
                        }
                        if ((x>=160) && (x<=240)) {
                            betast(162, 215, 240, 238);
                            break;
                        }
                        if ((x>=240) && (x<=320)) {
                            betast(242, 215, 318, 238);
                            Serial.print("Back gedrukt in settingsscherm");
                            toonStartScherm(test); //VERVANGEN DOOR BREAK IN KIES PLANTENBAK?????
                            currentPage = 0;
                            break;
                        }
                    }
                }
        }
    }

    void drawButtons() {
        currentPage = 4;
    Serial.println("begin drawbuttons");
    Serial.print("currentPage = ");
    Serial.println(currentPage);
        myGLCD.clrScr();
        myGLCD.setFont(BigFont);
        myGLCD.setBackColor(VGA_BLUE);
        for (x=0; x<5; x++) {// Draw the upper row of buttons
            myGLCD.setColor(0, 0, 255);
            myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
            myGLCD.setColor(255, 255, 255);
            myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
            myGLCD.printNumI(x+1, 27+(x*60), 27);
        }
   
        for (x=0; x<5; x++) {  // Draw the center row of buttons
            myGLCD.setColor(0, 0, 255);
            myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
            myGLCD.setColor(255, 255, 255);
            myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
            if (x<4)
            myGLCD.printNumI(x+6, 27+(x*60), 87);
        }
        myGLCD.print("0", 267, 87); // Draw the lower row of buttons
        myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect (10, 130, 150, 180);
        myGLCD.setColor(255, 255, 255);
        myGLCD.drawRoundRect (10, 130, 150, 180);
        myGLCD.print("Clear", 40, 147);
        myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect (160, 130, 300, 180);
        myGLCD.setColor(255, 255, 255);
        myGLCD.drawRoundRect (160, 130, 300, 180);
        myGLCD.print("Enter", 190, 147);
        myGLCD.setBackColor (0, 0, 0);
        myGLCD.setBackColor(VGA_BLACK);
        myGLCD.setColor(VGA_WHITE);
        myGLCD.drawRoundRect(242, 215, 318, 238);
        myGLCD.setFont(SmallFont);
        myGLCD.print("Back", 266, 221);
    }

    void updateStr(int val) {
    if (stCurrentLen<20) {
        stCurrent[stCurrentLen]=val;
        stCurrent[stCurrentLen+1]='\0';
        stCurrentLen++;
        myGLCD.setColor(0, 255, 0);
        myGLCD.print(stCurrent, LEFT, 224);
    }   else {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("BUFFER FULL!", CENTER, 192);
            delay(500);
            myGLCD.print("            ", CENTER, 192);
            delay(500);
            myGLCD.print("BUFFER FULL!", CENTER, 192);
            delay(500);
            myGLCD.print("            ", CENTER, 192);
            myGLCD.setColor(0, 255, 0);
        }
    }

    void leesGetal(int plantbak, int seizoen, int kolom) {
        Serial.println("begin leesgetal");
        while (currentPage == 4) {
            if (currentPage == 4 && myTouch.dataAvailable()) {
                myTouch.read();
                x=myTouch.getX();
                y=myTouch.getY();
                if ((y>=10) && (y<=60)) { // Upper row
                    if ((x>=10) && (x<=60))  { // Button: 1
                        betast(10, 10, 60, 60);
                        updateStr('1');
                    }
                    if ((x>=70) && (x<=120)) { // Button: 2
                        betast(70, 10, 120, 60);
                        updateStr('2');
                    }
                    if ((x>=130) && (x<=180))  // Button: 3
                    {
                    betast(130, 10, 180, 60);
                    updateStr('3');
                    }
                    if ((x>=190) && (x<=240))  // Button: 4
                    {
                    betast(190, 10, 240, 60);
                    updateStr('4');
                    }
                    if ((x>=250) && (x<=300))  // Button: 5
                    {
                    betast(250, 10, 300, 60);
                    updateStr('5');
                    }
                }

                if ((y>=70) && (y<=120)) { // Center row
                    if ((x>=10) && (x<=60))  {// Button: 6
                        betast(10, 70, 60, 120);
                        updateStr('6');
                    }
                    if ((x>=70) && (x<=120))  // Button: 7
                    {
                    betast(70, 70, 120, 120);
                    updateStr('7');
                    }
                    if ((x>=130) && (x<=180))  // Button: 8
                    {
                    betast(130, 70, 180, 120);
                    updateStr('8');
                    }
                    if ((x>=190) && (x<=240))  // Button: 9
                    {
                    betast(190, 70, 240, 120);
                    updateStr('9');
                    }
                    if ((x>=250) && (x<=300))  // Button: 0
                    {
                    betast(250, 70, 300, 120);
                    updateStr('0');
                    }
                }
                if ((y>=130) && (y<=180))  {// Upper row
                    if ((x>=10) && (x<=150))  {// Button: Clear
                        betast(10, 130, 150, 180);
                        stCurrent[0]='\0';
                        stCurrentLen=0;
                        myGLCD.setColor(0, 0, 0);
                        myGLCD.fillRect(0, 224, 319, 239);
                        }
                    if ((x>=160) && (x<=300)) { // Button: Enter
                        betast(160, 130, 300, 180);
                        if (stCurrentLen>0) {
                            for (x=0; x<stCurrentLen+1; x++){
                                stLast[x]=stCurrent[x];
                            }
                            stCurrent[0]='\0';
                            stCurrentLen=0;
                            myGLCD.setColor(0, 0, 0);
                            myGLCD.fillRect(0, 208, 319, 239);
                            myGLCD.setColor(0, 255, 0);
                            myGLCD.print(String(stLast), LEFT, 208);
                            int number = atoi(stLast);
                            customPlantenBakSettings[plantbak][seizoen][kolom] = number;
                            Serial.println("ENTER uit leesgetal gedrukt");
                            tekenSettingsManipulatieScherm(plantbak); 
                            break;
                        } else {
                            myGLCD.setColor(255, 0, 0);
                            myGLCD.print("BUFFER EMPTY", CENTER, 192);
                            delay(500);
                            myGLCD.print("            ", CENTER, 192);
                            delay(500);
                            myGLCD.print("BUFFER EMPTY", CENTER, 192);
                            delay(500);
                            myGLCD.print("            ", CENTER, 192);
                            myGLCD.setColor(0, 255, 0);
                        }
                    }
                    }
                }
                if ((y>=210) && (y<=240)) {
                    if ((x>=240) && (x<=320)) {
                        betast(242, 215, 318, 238);
                        tekenSettingsManipulatieScherm(plantbak);
                        Serial.println("Back in leesgetal gedrukt");
                        break;
                    }
                } 
        }
    }

    void betast(int x1, int y1, int x2, int y2) {
        Serial.println("begin betast");
        myGLCD.setColor(VGA_GREEN);
        myGLCD.drawRoundRect (x1, y1, x2, y2);
        while (myTouch.dataAvailable())
            myTouch.read();
    }

    void kiesPlantenBak() {

        int gekozenBak = 15; //15 is niet bestaandebestaande plantenbak
        //Serial.print("KIESBAK ");
        if (currentPage == 1 && myTouch.dataAvailable()) {
            Serial.println("aangeraakt in kiezen plantenbak = ");
            myTouch.read();
            x=myTouch.getX();
            y=myTouch.getY();
            if ((y>=10) && (y<=70)) { // bovenste bak
                gekozenBak = 0;
                betast(2, 5, 315, 70);
                tekenSettingsScherm(gekozenBak);
                //MOET HIER GEEN BREAK?????? of current page =0
            }
            if ((y>=73) && (y<=143)) {// middelste bak
                gekozenBak = 1;
                betast(2, 76, 315, 141);
                tekenSettingsScherm(gekozenBak);
                //MOET HIER GEEN BREAK??????
                
            }
            if ((y>=130) && (y<=180)) { // onderste bak
                gekozenBak = 2;
                betast(2, 147, 315, 212);
                tekenSettingsScherm(gekozenBak);
                //MOET HIER GEEN BREAK??????

            }
        }
    }
};

class DataUitwisselaarMaster {

    public:
    DataUitwisselaarMaster() {
        pinMode(SS2, OUTPUT);
        digitalWrite(SS2, HIGH);  // ensure SS stays high for now
        SPI.begin ();// Put SCK, MOSI, SS pins into output mode// also put SCK, MOSI into LOW state, and SS into HIGH state// Then put SPI hardware into Master mode and turn SPI on
        SPI.setClockDivider(SPI_CLOCK_DIV8); // Slow down the master a bit
    }  
    void zendOntvangData(){
        DefineSettingsNu::getSettingsNu();
        byte b;
        Serial.println("in zendontvangdata");
        digitalWrite(SS2, LOW);// enable Slave Select
        delayMicroseconds (200);
        //byte verzenden1 = 0xCD;
        byte z = SPI.transfer(0xCD); //0xCD = 205 Verzend startcode 0xCD voor Slave
        delayMicroseconds(20); //give the slave time to process
        // Serial.print("eerste verzonden byte is:");
        // Serial.println(verzenden1);
        Serial.print("eerste terugontvangen byte is: ");
        Serial.println(z);
        // byte verzenden2 = 0xF3;
        byte x = SPI.transfer(0xF3); //0xEF=243 is pumped to get response byte from slave
        // Serial.print("tweede verzonden byte is:");
        // Serial.println(0xF3);
        delayMicroseconds(50); //give the slave time to process
        Serial.print("tweede terugontvangen byte is: ");
        Serial.println(x);
        if (x == 0xEF){//0xEF=239
        Serial.println("verzonden data");
            for ( byte i = 0 ; i < 3 ; i++){
                Serial.println();
                for (byte j = 5 ; j < 15 ; j++){
                    b = (klimaatDataNu[i][j]);
                    byte y = SPI.transfer (b);
                    delayMicroseconds(20); //give the slave time to process
                    klimaatDataNu[i][j+10] = y;
                    delayMicroseconds(20); //give the slave time to process
                    Serial.print("verzonden byte is:");//deze print opdrachten zijn essientieel voor SPR
                    Serial.println(b);//deze print opdrachten zijn essientieel voor SPR
                    Serial.print("ontvangen byte is:");//deze print opdrachten zijn essientieel voor SPR
                    Serial.println(klimaatDataNu[i][j]);
                }
            }
        }
        digitalWrite(SS2, HIGH); // disable Slave Select
        DefineSettingsNu::getSettingsNu();
        Serial.println("=================");
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
        for (int baknummer = 0; baknummer < 3; baknummer++){
            klimaatDataNu[baknummer][JAAR] = (now.Year()- 2000); //Anders geen byte We leven voorlopig in de jaren 2000
            klimaatDataNu[baknummer][MAAND] = now.Month();
            klimaatDataNu[baknummer][DAG] = now.Day();
            klimaatDataNu[baknummer][UUR] = now.Hour();
            klimaatDataNu[baknummer][MINUUT] = now.Minute();
            //klimaatDataNu[baknummer][SECONDE] = now.Second();
        }
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


//DefineSettingsNu defineSettingsNu;
DataUitwisselaarMaster dataUitwisselaarMaster;
TouchScreen touchScreen;
Klok klok;

void setup (void){
    Serial.begin (9600);
    klok.setup();
    touchScreen.setup();
}

void loop(){
    Serial.println("nieuwe loop Master");
    RtcDateTime tijd = klok.getTime();
    RtcDateTime tijd2;
    //defineSettingsNu.getSettingsNu();
    
    dataUitwisselaarMaster.zendOntvangData();
    if(currentPage == 1 or currentPage == 0) { //0 wanneer terug uit settings schermen????????
        touchScreen.toonStartScherm(datumTijd);
    }
    int teller = tijd.Minute();
    int teller2 = teller;
    while ((currentPage == 1 or currentPage == 0) && teller == teller2) {//zonder die nul; hier alleen alleen 1; 0 wanneer terug uit settings schermen moet nieuwe loop beginnen
        touchScreen.kiesPlantenBak();
        tijd2 = klok.getTime();
        teller2 = tijd2.Minute();
    }
    Serial.println();
    Serial.println("einde loop Master");
}