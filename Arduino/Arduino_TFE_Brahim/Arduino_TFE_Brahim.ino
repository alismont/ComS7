/*----------------------------------------------------------------------
  DB Get Demo

  BRAHIM

  ------------------------------------------------------------------------
  This demo shows how to upload an entire DB from the PLC without
  knowing its size in advance.
  A DB1 with not more than 1024 byte into the PLC is needed.

  - During the loop, try to disconnect the ethernet cable.
  The system will report the error and will reconnect automatically
  when you re-plug the cable.
  - For safety, this demo *doesn't write* data into the PLC, try
  yourself to change ReadArea with WriteArea.
  - This demo uses ConnectTo() with Rack=0 and Slot=2 (S7300)
  - If you want to connect to S71200/S71500 change them to Rack=0, Slot=0.
  - If you want to connect to S7400 see your hardware configuration.
  - If you want to work with a LOGO 0BA7 or S7200 please refer to the
   documentation and change
   Client.ConnectTo(<IP>, <Rack>, <Slot>);
   with the couple
   Client.SetConnectionParams(<IP>, <LocalTSAP>, <Remote TSAP>);
   Client.Connect();
  ----------------------------------------------------------------------*/
#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"
#include <SD.h>

#include <Timer.h>
#include "TimerOne.h"

File myFile;
int i = 0;
int val = 0, valMemo = 0, valTest = 0, valEcritureMemo = 0;
bool Oneshot1 = false;
int DBs;
int Buf;
bool B3[100];
int FunctRT;
int IndFile;
char octetReception;
char caractereReception;
char octetReceptionProc;
char caractereReceptionProc;
String chaineReception, Tram;
String chaineReceptionProc, TramProc;
uint16_t NumeroDB = 1;
char octetReceptionTBL;
char caractereReceptionTBL;
char octetReceptionProcTBL;
String chaineReceptionTBL;
String TramTBL = "";
int MotClignotant = 0;

String NumDB;
char charVal[10];

int T[100], CptT[100];
bool TDN[100];

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0E, 0x05, 0xBD
};

IPAddress Local(192, 168, 1, 20); // Local Address
IPAddress PLC(192, 168, 1, 10); // PLC Address
int SQLecture = 0;
int SQEcriture = 0;
int NBRsSQ;
byte Buffer[1024];
int TblDBs[200], Index, PasApas = 0;
int PasApasEcr = 0;
int pin2 = 2;
int pin13 = 13;
int pin52 = 52;

int SYNCHRO = 0;
int LU = 0;
int NUM = 0;
int FirstScan = 1;
int MessIn = 0;
int MessOut = 0;

S7Client Client;
unsigned long Elapsed; // To calc the execution time


//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------
void setup() {

  pinMode(pin2, INPUT);           // set pin to input
  digitalWrite(pin2, HIGH);
  pinMode(pin13, OUTPUT);

  pinMode(pin52, INPUT_PULLUP);
  digitalWrite(pin52, HIGH);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial3.begin(9600);

  Timer1.initialize(100000);         // initialize timer1
  Timer1.attachInterrupt(callback);


  // Start the Ethernet Library
  Ethernet.begin(mac, Local);
  // Setup Time, someone said me to leave 2000 because some
  //rubbish compatible boards are a bit deaf.
  delay(2000);
  Serial.print(Ethernet.localIP());
  Serial.println("/");


  //CARTE SD
  Serial.print("Initializing SD card.../");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!/");
    return;
  }
  Serial.println("initialization done./");

}
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Connect()
{
  int Result = Client.ConnectTo(PLC,
                                0,  // Rack (see the doc.)
                                2); // Slot (see the doc.)
  Serial.print("Connecting to "); Serial.println(PLC);
  if (Result == 0)
  {
    Serial.print("Connected ! PDU Length = "); Serial.println(Client.GetPDULength());
  }
  else
    Serial.println("Connection error");
  return Result == 0;
}



//----------------------------------------------------------------------
// Buffer VERS plc
//----------------------------------------------------------------------
void DumpVersPLC(void *Buffer, int Length)
{


}

//----------------------------------------------------------------------
// Dumps a buffer
//----------------------------------------------------------------------
void Dump(void *Buffer, int Length)
{
  int i, cnt = 0;
  pbyte buf;

  if (Buffer != NULL)
    buf = pbyte(Buffer);
  else
    buf = pbyte(&PDU.DATA[0]);

  Serial.print("[ Dumping "); Serial.print(Length);
  Serial.println(" bytes ]==========================");
  IndFile = Length;
  for (i = 0; i < Length; i++)
  {
    cnt++;
    if (buf[i] < 0x10)
      Serial.print("0");
    Serial.print(buf[i], HEX);
    Buf = buf[i];
    Serial.print(" ");
    if (cnt == 16)
    {
      cnt = 0;
      Serial.println();
    }
  }
  Serial.println("===============================================");
}
//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    Serial.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect();
  }
}
//----------------------
// Profiling routines
//
void MarkTime()
{
  Elapsed = millis();
  // Calcs the time
  Elapsed = millis() - Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);
}



//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop()
{
  //OneShot
  if (valMemo == 1) {
    if (!B3[6]) {
      B3[6] = 1;
      //valMemo=0;
    }
  }
  else {
    B3[6] = 0;
  }

  valTest = digitalRead(pin52);

  //Tempo TON
  if (!digitalRead(pin2)) {
    CptT[2] = true;
  }
  else {
    T[2] =  0.0;
    TDN[2] =  0.0;
  }
  if (T[2] >= 100) {
    TDN[2] =  1.0;
    T[2] = 100;
    CptT[2] = false;
    //valMemo = 1;
  }



  if (!digitalRead(pin2) && MotClignotant == 0 ) {
    CptT[3] = true;
  }
  else {
    T[3] =  0.0;
    TDN[3] =  0.0;
    T[3]  = 0.0;
  }
  if (T[3] >= 5) {
    TDN[3] =  1.0;
    T[3] = 5;
    CptT[3] = false;
    MotClignotant = 1;
    valMemo = 1;
  }

  if (!digitalRead(pin2) && MotClignotant == 1 ) {
    CptT[4] = true;
  }
  else {
    T[4] =  0.0;
    TDN[4] =  0.0;
    T[4]  = 0.0;
  }
  if (T[4] >= 10) {
    TDN[4] =  1.0;
    T[4] = 10;
    CptT[4] = false;
    MotClignotant = 0;
    valMemo = 0;
  }

  if (!digitalRead(pin2) && MotClignotant == 0) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }


  // gestion comm RS232
  while (Serial.available() > 0) { // si un caractère en réception

    octetReceptionProc = Serial.read(); // lit le 1er octet de la file d'attente

    if (octetReceptionProc == '/') {

      if (chaineReceptionProc.substring(0, 5) == "OKFIN")  {
        Serial.println("OKFIN/");
        Index = 0;
        SDWrite();
        PasApas = 0;
      }

      if (PasApas = 1)  {
        TblDBs[Index] = (chaineReceptionProc.toInt());
        dtostrf( TblDBs[Index], 4, 0, charVal);
        Tram = Tram + ";" +  charVal;
        Index++;
      }

      if (chaineReceptionProc.substring(0, 6) == "OKPROC")  {
        delay(500);
        Serial.println("OKPROC/");
        PasApas = 1;
        Tram = "";
        Index = 0;
      }


      if (chaineReceptionProc.substring(0, 5) == "TOPLC")  {
        delay(500);
        Serial.println("okokok/");
        valEcritureMemo = 1;

        //Buffer[0] = 04;
        //Buffer[1] = 15;
        ////int retour=Client.WriteArea(S7AreaDB, 1, 0, 2, &Buffer);
        //Client.WriteArea(S7AreaDB, 1, 0, 2, &Buffer);
        //Serial.println("retour");
      }

      //Serial.println(chaineReceptionProc);
      chaineReceptionProc = "";
    }
    else {
      caractereReceptionProc = char(octetReceptionProc);
      chaineReceptionProc = chaineReceptionProc + caractereReceptionProc;

    }
  }

  //demande Ecriture
  if (digitalRead(pin2) and valEcritureMemo == 1) {

    //Pas A pas Ecriture
    if (PasApasEcr == 0) {
      if (valEcritureMemo == 1 ) {
        SDRead();

        switch (SQEcriture) {
          case 0:
            if (TblDBs[0] > 0) {
              NBRsSQ = 1;
              SQEcriture = 1;
              Serial.print("case 0:...Ecriture");
              Serial.print(TblDBs[0]);
              delay(1);
            }
            break;
          case 1:
            DBs = TblDBs[NBRsSQ];
            SQEcriture = 2;
            Serial.print("case 1:...Ecriture.");
            Serial.print(DBs);
            delay(1);
            //SenttiminoEcriture();
            break;
          case 2:
            NBRsSQ++;
            if (NBRsSQ > TblDBs[0]) {
              SQEcriture = 0;
              valEcritureMemo = 0;
              Serial.println("case 2 Fin:.....");
            }
            else {
              SQEcriture = 1;
              Serial.print("case 2 else:.....");
              delay(1);
            }
            break;
        }
      }
    }
  }
  //-------------------------------------

  //demande lecture
  if (!digitalRead(pin2) and valMemo == 0 and SQLecture == 0) {
    //valMemo = 1;
    digitalWrite(pin13, HIGH);
  }
  if (digitalRead(pin2) and valMemo == 0 and SQLecture == 0) {
    digitalWrite(pin13, LOW);
  }


  //Pas A pas
  if (PasApas == 0) {
    if (valMemo == 1 ) {
      SDRead();

      switch (SQLecture) {
        case 0:
          if (TblDBs[0] > 0) {
            NBRsSQ = 1;
            SQLecture = 1;
            Serial.print("case 0:...");
            Serial.print(TblDBs[0]);
            delay(1);
          }

          break;
        case 1:
          DBs = TblDBs[NBRsSQ];
          SQLecture = 2;
          Serial.print("case 1:....");
          Serial.print(DBs);
          delay(1);
          Senttimino();
          break;
        case 2:
          NBRsSQ++;
          if (NBRsSQ > TblDBs[0]) {
            SQLecture = 0;
            valMemo = 0;
            Serial.println("case 2 Fin:.....");
            delay(1);
            //digitalWrite(pin13, LOW);
            delay(1);
            //digitalWrite(pin13, HIGH);
          }
          else {
            SQLecture = 1;
            Serial.print("case 2 else:.....");
            delay(1);
          }
          break;
      }


    }
  }
  FirstScan = 0;

  Ecriture ();// tablette vers arduino
  Lecture (); // arduino vers tablette
}

//------------------------------------------------------
//void SenttiminoEcriture() {
//
//
//  uint16_t Size;
//  int Result;
//  Size = sizeof(Buffer);
//
//  // Connection
//  while (!Client.Connected)
//  {
//    if (!Connect())
//      delay(500);
//  }
//
//  Serial.print("Uploading DB:");
//  Serial.println(DBs);
//  // Get the current tick
//  MarkTime();
//  Result = Client.DBGet(DBs,     // DB Number = 1
//                        &Buffer, // Our Buffer
//                        &Size);  // In input contains our buffer size
//  // As output contains the data read
//  if (Result == 0)
//  {
//    ShowTime();
//    Dump(&Buffer, Size);
//
//    SDWriteFile();
//  }
//  else {
//    CheckError(Result);
//  }
//}


//------------------------------------------------------
void Senttimino() {


  uint16_t Size;
  int Result;
  Size = sizeof(Buffer);

  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }

  Serial.print("Uploading DB:");
  Serial.println(DBs);
  // Get the current tick
  MarkTime();
  Result = Client.DBGet(DBs,     // DB Number = 1
                        &Buffer, // Our Buffer
                        &Size);  // In input contains our buffer size
  // As output contains the data read
  if (Result == 0)
  {
    //ShowTime();
    Dump(&Buffer, Size);

    SDWriteFile();
  }
  else {
    CheckError(Result);
  }
}
//------------------------------------------------------
void SDWrite() {
  boolean RemoveFile;
  RemoveFile = SD.exists("DBs.txt");

  if (RemoveFile) {
    SD.remove("DBs.txt");
  }
  myFile = SD.open("DBs.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {

    for (int i = 0; i <= TblDBs[0]; i++) {
      myFile.println(TblDBs[i]);
    }
    myFile.close();
  }
  else {
    // if the file didn't open, print an error:
    //Serial.println("error opening test.txt/");
  }
  PasApas = 0;
}
//-----------------------------------------------
//Lecture DBs avec Nbrs de DB & diff numero de DB
void SDRead()
{
  myFile = SD.open("DBs.txt");
  Index = 0;
  String inString;
  int inChar;
  if (myFile) {
    //Serial.println("Debut:");

    while (myFile.available()) {
      int inChar = myFile.read();

      if (inChar == 10) {

        TblDBs[Index] = (inString.toInt());
        // Serial.println(TblDBs[Index]);
        inString = "";
        Index++;

      }
      else {
        inString += (char)inChar;
      }

    }
    // close the file:
    myFile.close();
  } else {
  }
}

//------------------------------------------------------
void SDWriteFile() {
  NumDB = "DB" + String(DBs, HEX) + ".TXT";

  myFile = SD.open(NumDB, FILE_WRITE);
  myFile.seek(0);
  // if the file opened okay, write to it:
  if (myFile) {

    for (int i = 0; i <= IndFile; i++) {

      myFile.println(Buffer[i]);
    }
    myFile.close();
  }
  else {
    // if the file didn't open, print an error:
    //Serial.println("error opening NumDB.txt/");
  }
  PasApas = 0;
}

//------------------------------------------------------
//void SDReadFile() {
//  NumDB = "DB" + String(DBs, HEX) + ".TXT";
//
//  myFile = SD.open(NumDB, FILE_WRITE);
//  myFile.seek(0);
//  // if the file opened okay, write to it:
//  if (myFile) {
//
//    for (int i = 0; i <= IndFile; i++) {
//
//      myFile.println(Buffer[i]);
//      myFile.read(Buffer[i]);
//    }
//    myFile.close();
//  }
//  else {
//    // if the file didn't open, print an error:
//    //Serial.println("error opening NumDB.txt/");
//  }
//  PasApas = 0;
//}

//-------------------------------------------------------
void callback() {
  if (CptT[2]) {
    T[2] =  T[2] + 1.0;
  }
  if (CptT[3]) {
    T[3] =  T[3] + 1.0;
  }
  if (CptT[4]) {
    T[4] =  T[4] + 1.0;
  }
}

//-------------------------------------------------------
void Ecriture() {

  while (Serial3.available() > 0) { // si un caractère en réception

    octetReceptionTBL = Serial3.read(); // lit le 1er octet de la file d'attente

    if (octetReceptionTBL == 10) { // si Octet reçu est le saut de ligne

      //PWM4
      if (chaineReceptionTBL == "PWM4=ON") {
        // digitalWrite(LED4, HIGH);
        //digitalWrite(LED13, HIGH);
        //MessIn = 1;
      }
      if (chaineReceptionTBL.substring(0, 2) == "OK")  {
        //        String InString;
        //        InString = chaineReception.substring(3);
        //        Value = InString.toFloat();
        //        F8[IndexDebug] = Value;
        Serial.println(octetReceptionTBL);
      }

      //synchro
      if (chaineReceptionTBL == "SYNCHRO")  {
        SYNCHRO = 1;
        LU = 1;
      }


      //synchro off
      if (chaineReceptionTBL == "SYNCHROFF")  {
        SYNCHRO = 0;
      }



      if (chaineReceptionTBL == "MESSDIT")  {
        MessOut = 0;
      }

      //LU
      if (chaineReceptionTBL == "LU")  {
        LU = 1;
      }
      //Serial.println(chaineReceptionTBL.substring(0, 9));
      //Serial.println (chaineReceptionTBL);
      //      Serial.print("SYNCHRO:");
      //      Serial.println(SYNCHRO);
      //      Serial.print("LU:");
      //      Serial.println(LU);
      chaineReceptionTBL = ""; //RAZ le String de réception

      break;
    }
    else { // si le caractère reçu n'est pas un saut de ligne
      caractereReceptionTBL = char(octetReceptionTBL); // récupere le caractere à partir du code Ascii
      chaineReceptionTBL = chaineReceptionTBL + caractereReceptionTBL; // ajoute la caractère au String
      //Serial.println (chaineReception);
      //delay(1); // laisse le temps au caractères d'arriver
    }
  }
}

//----------------------------
void Lecture() {


  if ((LU == 1) and (SYNCHRO == 1)) {
    LU = 0;
    TramTBL = "";

    dtostrf( SQLecture, 1, 0, charVal);
    TramTBL = TramTBL + "D01" +  charVal;
    TramTBL = TramTBL + "F";

    dtostrf( valMemo, 1, 0, charVal);
    TramTBL = TramTBL + "D02" +  charVal;
    TramTBL = TramTBL + "F";

    dtostrf( valTest, 1, 0, charVal);
    TramTBL = TramTBL + "D03" + charVal;
    TramTBL = TramTBL + "F";

    Serial3.print(TramTBL);
    //Serial.println(TramTBL);

  }
  //delay(1);
}//-------------------------------------------------


