/* DESCRIPTION
  ====================
  controling Dust collector and air Pleasure for 5 tools

  Commands to Raspi --->
  xBeeName - from xBee (=Ident) [max 4 caracter including numbers]
  'POR'    - power on reset (Ident;por)
   Acknolage for commands:
  'dcok'  - dust collector ok (started)
  'apok'  - air preasure ok (started)

  Commands from Raspi
  'time'  - format time33.33.33 33:33:33
  'dcon'   - set dust collector on
  'dcof'   - set dust collector off
  'apon'   - set air preasure on
  'apof'   - set air preasure off

  last change: 03.01.2022 by Michael Muehl
  changed: include commands for controller of dust and air
	*/
#define Version "0.9.x" // (Test =0.9.0 ==> 0.1.0)
#define xBeeName "K200"	// Gerätename für xBee

// ---------------------
#include <Arduino.h>
#include <TaskScheduler.h>

// PIN Assignments

#define DustColl A2 // dust collector on / off
#define AirPreas A3 // air preasure on / off

#define xBeError  8 // xBee and Bus error (13)

// DEFINES
#define porTime   5 // wait seconds for sending Ident + POR
#define repMES    1 // repeat commands
#define divTime   4 // divider for time

// CREATE OBJECTS
Scheduler runner;

// Callback methods prototypes
void checkXbee();        // Task connect to xBee Server
void BlinkCallback();    // Task to let LED blink - added by D. Haude 08.03.2017
void CheckEvent();       // Task to event checker
// TASKS
Task tM(TASK_SECOND / 2, TASK_FOREVER, &checkXbee);	    // 500ms main task
Task tB(TASK_SECOND * 5, TASK_FOREVER, &BlinkCallback); // 5000ms added M. Muehl

// VARIABLES
boolean dcOK = false;  
boolean apOK = false;

// Serial with xBee
String inStr = "";      // a string to hold incoming data
String IDENT = "";      // identifier for remote access control
byte co_ok = 0;         // count ok after +++ control AT sequenz
byte getTime = porTime;

// ======>  SET UP AREA <=====
void setup()
{
  //init Serial port
  Serial.begin(57600);  // Serial
  inStr.reserve(40);    // reserve for instr serial input
  IDENT.reserve(5);     // reserve for IDENT serial output

  // initialize:
  // IO MODES
  pinMode(xBeError, OUTPUT);

  pinMode(DustColl, OUTPUT);
  pinMode(AirPreas, OUTPUT);

  // Set default values
  digitalWrite(xBeError, HIGH); // turn the LED ON (init start)

  digitalWrite(DustColl, LOW);  // turn off dust collector
  digitalWrite(AirPreas, LOW);  // turn off air preasure

  runner.init();
  runner.addTask(tM);
  runner.addTask(tB);

  tM.enable();         // xBee check
  Serial.print("+++"); //Starting the request of IDENT
}
// Setup End -----------------------------------

// TASK (Functions) ----------------------------
void checkXbee()
{
  if (IDENT.startsWith(xBeeName) && co_ok == 2)
  {
    ++co_ok;
    tB.setCallback(retryPOR);
    tB.enable();
    tM.disable();
    digitalWrite(xBeError, LOW); // turn the LED off (Programm start)
  }
}

void retryPOR() {
  digitalWrite(xBeError, !digitalRead(xBeError));
  if (getTime < porTime * 5)
  {
    tB.setInterval(TASK_SECOND * getTime);
    ++getTime;
    Serial.println(String(IDENT) + ";POR;V" + String(Version));
  }
  else if (getTime == 255)
  {
    tB.disable();
    digitalWrite(xBeError, LOW);
    tM.setCallback(checkCMDO);
    tM.enable();
  }
  else
  {
    tB.setCallback(BlinkCallback);
    tB.setInterval(TASK_SECOND / 2);
    tM.disable();
  }
}

void checkCMDO()  // check for sending OK cmd
{
  if (dcOK)
  {
    Serial.println("DCOK");
    dcOK = false;
  }

  if (apOK)
  {
    Serial.println("APOK");
    apOK = false;
  }

}
void BlinkCallback()
{
  // --Blink if BUS Error
  digitalWrite(xBeError, !digitalRead(xBeError));
}

// END OF TASKS ---------------------------------

// FUNCTIONS ------------------------------------
// End Funktions --------------------------------

// Funktions Serial Input (Event) ---------------
void evalSerialData()
{
  inStr.toUpperCase();
  if (inStr.startsWith("OK"))
  {
    if (co_ok == 0)
    {
      Serial.println("ATNI");
      ++co_ok;
    }
    else
    {
      ++co_ok;
    }
  }

  if (co_ok ==1  && inStr.length() == 4)
  {
    IDENT = inStr;
    Serial.println("ATCN");
  }

  if (inStr.startsWith("TIME"))
  {
    getTime = 255;
  }

  // command 2
  if (inStr.startsWith("DCON")  && inStr.length() == 4)
  {
    digitalWrite(DustColl, HIGH);
    dcOK = true;
  }
  
  if (inStr.startsWith("DCOF")  && inStr.length() == 4)
  {
    digitalWrite(DustColl, LOW);
    dcOK = true;
  }
  
  if (inStr.startsWith("APON")  && inStr.length() == 4)
  {
    digitalWrite(AirPreas, HIGH);
    apOK = true;
  }
  
  if (inStr.startsWith("APOF")  && inStr.length() == 4)
  {
    digitalWrite(AirPreas, LOW);
    apOK = true;
  }
}

/* SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent()
 {
  char inChar = (char)Serial.read();
  if (inChar == '\x0d')
  {
    evalSerialData();
    inStr = "";
  }
  else if (inChar != '\x0a')
  {
    inStr += inChar;
  }
}
// End Funktions Serial Input -------------------

// PROGRAM LOOP AREA ----------------------------
void loop() {
  runner.execute();
}
