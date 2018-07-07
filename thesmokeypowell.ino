// Hello, Have a good look around before you burn something down or electrocute yourself
// I tried my best to explain what I was doing to help stupid poeple like me figure it all out
// This unit is not designed to be plug and play, but if you understand what you are doing and 
// like to have control of the fine tuning you will enjoy The Smokey Powell.
// This program is not perfect by any means, mostly because I am still very new to this.
// So if you find it to be a little clunky, keep in mind it's main interface was intended to be your phone running blynk


// A couple of notes. There is a section for adjustable values below. Use this to set fan or electric
// some LCD I2c controllers have diffrent addressess. you need to enter yours below if not working
// This controller has two pins one for Main heat, one for smoke control
// This controller con run a timed pulse mode for solid state relays
// or it can run a pulse width modulated mode for a fan *the smoke control will have no function in this mode*
// some fans do not like pwm for short pulses there is a way to fix this below.
// you can use the bbq guru fan with this controller. It Does Not like to be use with pwm


#include <blynk.h>          // speaking of stupid people, these are the included libraries, I'm not smart enough to write all this from scratch
                                // Thanks to all that contributed to the Libraries and thanks to Adafuit for the thermistor function
#include <LiquidCrystal_I2C_Spark.h>

#include <pid.h>

#include "math.h"



//blynk auth code goes here


char auth[] = " put your auth code here";       // You need the blynk auth code to make it work with blynk. find it on your blynk app


//******************************
// uncomment the selectAntenna out if you plan to use an external one.
//STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));      // the antenna defualt is internal. the range may be very poor with external selected and no antenna
SYSTEM_MODE(SEMI_AUTOMATIC);                    // a particle thing



//***************************************************************
//PID

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
double aggKp=15, aggKi=0.02, aggKd=40;       //when input is below setpoint
double consKp=15, consKi=0.1, consKd=40;     // when input is above setpoint. in this case P does nothing..... that is unless you change it

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, PID::DIRECT);


//*********************************************************
//lcd

LiquidCrystal_I2C *lcd;

int lastSecond = 0;

//************************************************************
// thermistor 
//NOTE i was reading 9 degrees high at 77f I could not correct for the curve with this version
// It read great +- 3 from 150f to 450f. +2 from 120f to 150f. +3to5 from 100f to 120 something to keep in mind
// I might fix this later

#define THERMISTORNOMINAL 100000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 79      //79 higher increases results
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 10
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 4870           //4870  higher number narrow results between two temps
// the value of the 'other' resistor
#define SERIESRESISTOR 100000
float fahrenheit;
int samples[NUMSAMPLES];
int THERMISTORPIN;

unsigned long temptimer; //used for temp sampling timer
int meat1;      // temp values of the meat probes
int meat2;      
int meat3;      
int probestate1;    // on off states of the meat probes
int probestate2;
int probestate3;
int alarm1; // alarm states used for each probe
int alarm2;
int alarm3;
int pidsample; // used to make the pid sample time adjustable
int buttonone;  // a filtered conditioned button state used for direct control
int buttontwo;
int buttonthree;
int buttonfour;
int onestate; // shows the current state of the button
int twostate;
int threestate;
int fourstate;
int onelaststate = 0;  // used to save the last state of the button
int twolaststate = 0;
int threelaststate = 0;
int fourlaststate = 0;
unsigned long onefast = 0;  // used with millis to determine time held down of the button
unsigned long twofast = 0;
unsigned long threefast = 0;
unsigned long fourfast = 0;
int buttondelay; // how long button is held before fast indexing.
int screen = 0; // represents the screen number that is desired
int alarmtemp; // the alarm temp. temp at which the alarm turns on
int alarmtempmin;   // lets you set the minimum limit of alarmtemp setting
int alarmtempmax;   // lets you set the maximum limit of alarmtemp setting
int alarm;  //  the alarm state 
unsigned long alarmblink = 0; // the alarm blink timer
int alarmon;// used for send a high or low output when on
int alarmoff;// used to send a high or low output when off
int maxsetpoint; //maximum allowable setpoint
int minsetpoint; // minimum allowable setpoint
unsigned long prepulse; // counter for pulse interval
int outputperc; // used as an interface between pid and non pid use to simplify the sketch. converts to percentage
int inputdis; // used to display the input without decimals
int setpointdis; // same as inputdis except
unsigned long blynker;  // a timer used to send values to blynk cloud
int verify;     // used to monitor a virtual pin on blynk and turn on an output to verify cloud connection
int lastverify; // the saved last state
int notify = 0; // this is used to send a notification to the phone through blynk
int blynkup;    // used with button presses to decide when to update to blynk
int connect = 0;        // used to set the cloud connection on and off and reconnect if necessary 
unsigned long notifier = 0;     //a timer that used used to resend notifications
int outputtype;         // used to decide which output type you want. electric or fan.
int prange;     // the percentage at which the fan switches from timed pulsing to pwm
int outpulse = 0; //used with a timer to pulse fan on and off
unsigned long smoketimer; // used for timing the smoke element intervals
int smokeperc; // the percentage the smoke element will run
int smokeduty; // a duty cycle for the smoke element, give more control in case of over powered elements
int tempalarm; // an on or off state of the over temp. used as an alarm
int overtemp; // a value in degrees f that the temp can be above the setpoint before going into an alarm state
int heatcontrol=1; // used to control the output mode, 0 = off, 1 = auto, 2 = auto off (off at alarm temp)
int manualperc=0; // a value in percent to control the main heat in manual


// pin assignment incase you make your own board and want to move stuff around.
int alarmpin = A4;
int buttonpin1 = D4;
int buttonpin2 = D5;
int buttonpin3 = D6;
int buttonpin4 = D7;
int mainpin = D2;
int smokepin = D3;
int pitprobepin = A3;
int meatprobepin1 = A2;
int meatprobepin2 = A1;
int meatprobepin3 = A0;




/* for reference Blynk app V numbers

V0  Setpoint    the temp setpoint *****Notice we send values to Setpoint, but read them from setpointdis**** this why it we dont see extra zeros
V1  smokeperc   the smoke percent
V2  alarmtemp   the alarm temp
V3  Input       what the pit probe is reading for temp
V4  meat1       what the meat probes are reading
V5  meat2
V6  meat3
V7  verify          a signal sent from app to confirm connection
V8  probestate1     and on or off state of the probes using 1 and 0... i think
V9  probestate2
V10 probestate3
V11 outputperc      the percentage that the output is running
V12 has no name, used for a return verifiy signal from particle to app.... that way I don't have to get off my lazy ass to see the there is a connection
V13 Heatcontrol     used to off, manual, auto, and auto off control
v14 manualperc      a simple and lazy way of controlling the output in manual mode
*/

BLYNK_WRITE(V0)         // these are all blynk write commands. they allow the blynk app to send data to the hardware
{
  Setpoint = param.asInt();}

BLYNK_WRITE(V1)
{
  smokeperc = param.asInt();}
  
BLYNK_WRITE(V2)
{
  alarmtemp = param.asInt();}
  
BLYNK_WRITE(V7)
{
  verify = param.asInt();}
  
BLYNK_WRITE(V8)
{
  probestate1 = param.asInt();}

BLYNK_WRITE(V9)
{
  probestate2 = param.asInt();}
  
BLYNK_WRITE(V10)
{
  probestate3 = param.asInt();}
  
BLYNK_WRITE(V13){
  switch (param.asInt())
{
    case 1:
    heatcontrol = 1;//off
    break;
    
    case 2:
    heatcontrol = 2;//manual
    break;
    
    case 3:
    heatcontrol = 3;//auto
    break;
    
    case 4:
    heatcontrol = 4;//auto off
    break;
    
    default:
    heatcontrol = 1;//off
}}

BLYNK_WRITE(V14)
{
  manualperc = param.asInt();}



void setup() {
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////

Blynk.config(auth);
Blynk.disconnect();     // we call this to keep blynk from trying to connect


  //tell the PID the range between 0 and the full window size
 // myPID.SetOutputLimits(0, 255);
//  myPID.SetSampleTime(pidsample);

  

    
    
    
  Serial.begin(9600);
  lcd = new LiquidCrystal_I2C(0x27, 20, 4);     // some liquid c displays have a different address that goes here. mine is 0x27
  lcd->init();
  lcd->backlight();
  lcd->clear();
  lcd->print("**The Smoky Powell**");  // you can type anything you want here.
  delay(1000);
  lcd->clear();                     
///////////////////////////////////////////////////




    pinMode(alarmpin, OUTPUT);
    pinMode(mainpin, OUTPUT);
    pinMode(smokepin, OUTPUT);
    pinMode(buttonpin1, INPUT_PULLDOWN);
    pinMode(buttonpin2, INPUT_PULLDOWN);
    pinMode(buttonpin3, INPUT_PULLDOWN);
    pinMode(buttonpin4, INPUT_PULLDOWN);
    
    
    
    
    
    
      Output = 0; //had a problem i could not solve, without the program making a cycle the Output would always start at 45.6
  ///////////////////////////////////////////////////////////////////////////////////////
 
  // adjustable values all in one place for convenience
  // Keep in mind the blynk app sets limits on the vitrual pins. You need to set them to match
  // I might change this later so that the hardware can adjust them, but I need a break from this project
  
  screen = 0;// preferred starting screen
  Setpoint = 225; // sets the initial setpoint temp of the chamber
  maxsetpoint = 450; //maximum allowible setpoint
  minsetpoint = 100; // minimum allowable setpoint
  pidsample = 20000; // used to make the pid sample time adjustable in millis
  buttondelay = 1000;// how long button is held before fast indexing.
  alarmtemp = 175;// the starting temp for the meat probe alarm to be set at. tells you when it's done cooking
  alarmon = HIGH;// used for send a HIGH or LOW output when on
  alarmoff = LOW;// used to send a HIGH or LOW output when off
  alarmtempmin = 100; // the minimum aloud alarm temp
  alarmtempmax = 400; // the max
  outputtype = 0;       // enter 0 for electric, 1 for pwm fan output. Note smoke will still be in the menu, but does nothing
                        //Adding the fan was an after thought for this controller
  prange = 30;  // percentage the output must be to switch from pulsing to pwm fan control. some fans are different.
  smokeduty=50; // 100 is max. Duty cycle for smoke element this will trim back the output duration time. 
  overtemp=50; // degrees in f of overtemp before going into alarm




lcd->print("Connecting WIFI"); // here we offer the chance to set the particle to listen mode (for wifi setup) or cancel (when there is no wifi)
lcd->setCursor(9, 1);           // this lcd stuff is fun and simple look it up if you have questions. it's a little different then arduino
lcd->print("5");                //putting something between "" prints exactly that. in this case a 5
lcd->setCursor(0, 2);
lcd->print(" Hold listen mode>");
lcd->setCursor(0, 3);
lcd->print(" Hold to cancel>>>");
delay(1000);
lcd->setCursor(9, 1);
lcd->print("4");
delay(1000);
lcd->setCursor(9, 1);
lcd->print("3");
delay(1000);
lcd->setCursor(9, 1);
lcd->print("2");
delay(1000);
lcd->setCursor(9, 1);
lcd->print("1");
delay(1000);
setpointdis=Setpoint;
if(digitalRead(buttonpin3)==HIGH){  // reading a button
    WiFi.listen();}                 // sets wifi to listen

if(digitalRead(buttonpin4)==LOW){   // here I'm being lazy again, which could cause problems if you hold both buttons.... Maybe
    lcd->clear();                   // anyway we look to see if the cancel button is NOT being held. if not, then we connect. 
connect=1;
 Particle.connect();
 Blynk.connect();
Blynk.virtualWrite(V0, setpointdis);  
Blynk.virtualWrite(V1, smokeperc);
Blynk.virtualWrite(V2, alarmtemp);
}

}

void loop() {
    ///////////////////////////////////////////////////////////////// finally the loop 
    ////////////////////////////////////////////////////////////////
 // 
      
  //////////////////////some blynk stuff      
      if(blynkup==1){ // this gets called when a button gets released. too keep from sending data too often, but also get updates fast when needed
          
        Blynk.virtualWrite(V0, setpointdis);  
        Blynk.virtualWrite(V1, smokeperc);
        Blynk.virtualWrite(V2, alarmtemp);
       
   }
blynkup=0;  //resets after writing to blynk


if((millis()-blynker)>5000){    //this timer sends updates to blynk every 10 seconds
        Blynk.virtualWrite(V3, Input);// this set at the 5 sec mark to split up the data
        Blynk.virtualWrite(V4, meat1);
        Blynk.virtualWrite(V5, meat2);
        Blynk.virtualWrite(V6, meat3);
        Blynk.virtualWrite(V8, probestate1);
        }
        
if((millis()-blynker)>10000){ // this section at the 10 sec mark
        Blynk.virtualWrite(V9, probestate2);
        Blynk.virtualWrite(V10, probestate3);
        Blynk.virtualWrite(V11, outputperc);
        Blynk.virtualWrite(V13, heatcontrol);
        Blynk.virtualWrite(V14, manualperc);
        if(connect==1){                         //connect sets whether or not to connect to the cloud
            if(Blynk.connected()==false){       // we check to see if blynk is connected
                Blynk.connect();}}          //if not connected. Blynk.connect boggs down the controller. so only uses it if needed
blynker=millis();                       // noctice we put the above in the timer. we reconnect because every once in a while
// blynk disconnects. it seems to happen when pressing buttons too fast. blynk says no more than 10 writes per second.

}
    
   if (Blynk.connected()==true){    //if connected run blynk every cycle. this is needed for blynk to work
        Blynk.run();
  }
  
  
  if(verify==1&&alarm==0&&tempalarm==0){    // verify gets set high from blynk app turns on the alarm output
            digitalWrite(alarmpin, alarmon);}   // this only works when not in an alarm state since controlling the output
      else {digitalWrite(alarmpin, alarmoff);}  // from two places was getting conflicts.
      
  if(verify!=lastverify){                           // we look for a change and then save it this way we only send a blynk signal one time
      if(verify==1){Blynk.virtualWrite(V12, 255);}  // this v12 i have set up as an led on the phone
      if(verify==0){Blynk.virtualWrite(V12, 0);}
  }  
    lastverify=verify;  // after the task is complete we save the last state of verify. 
    
      
    
    ///////Notice below sometimes I use AND "&&" or OR "||" and sometimes I stack "if" and "else" statements. both doing the same thing
    // Stacking "if" statements allows to mix in other commands in one bunch
    if(notify==0&&tempalarm==1){                                  // Notify of high temp, stuck relay, over fire ect.
            Blynk.notify("Warning High Temp!");
            Blynk.virtualWrite(V13, heatcontrol);
            notify=1;
        }
        
    if(alarm==1&&notify==0){                            // notify is used to keep from sending a notification too often
            Blynk.notify("Meat Probes are up to temp!"); /// notice how if hightemp sends a notification this section will never be checked
            Blynk.virtualWrite(V13, heatcontrol);
            notify=1;
    }    
        
    
    if(Input - overtemp>Setpoint){tempalarm=1;}  // set temp alarm high if over shoot
        else{tempalarm=0;}
   
   if(notify==1){ 
        if((millis()-notifier)>120000){   // every two minutes we reset notify allowing it to resend the notification
            notify=0;
   }}
   
    if(notify==0){                      // this is a little messy, but it allows use to start counting when notify goes high
        notifier=millis();              // to delay it an exact amount of time
    }
    
  ////////////////////////////////////////////////////////////
  //Temp stuff as well as some random, but needed things


if((millis()-temptimer) > 5000){  // I like to think of this as a one shot timer. it's very simple
                                  //after it is triggered it immediately resets also reading the temperatures one time
     Input=tempRead(pitprobepin);  // reading the temp of the chamber/pit probe
     inputdis=Input;        // converts to a different type to better display on the lcd
     if (probestate1==1){    // the following is looking to see if we have the meat probes turned on using the probestate
        meat1=tempRead(meatprobepin1);   // as our indicator. if so we read the temp if not, we set them to 0
      }                         //tempRead is the function at the bottom.
      else{meat1=0;alarm1=0;}

      
     if (probestate2==1){
        meat2=tempRead(meatprobepin2);
     }
      else{meat2=0;alarm2=0;}       // same for the other probes

      
     if (probestate3==1){
        meat3=tempRead(meatprobepin3);
      }
      else{meat3=0;alarm3=0;}
     temptimer=millis(); //reset the timer
     lcd->clear();       //update the screen with the new temps
  }


Setpoint=constrain(Setpoint,minsetpoint,maxsetpoint);   //constrains the Setpoint to the min and max settings
setpointdis=Setpoint;// again better for lcd display


        if(probestate1==1){     // this section handles the alarm
            if(meat1>alarmtemp){    // by now you should be picking up on the names pretty quickly
                alarm1=1;           // and understanding the simple stuff like this.... I'm just saying this because I need to go finish the chicken coop
            }
            else{alarm1=0;}
        }
        
        
        if(probestate2==1){
            if(meat2>alarmtemp){
                alarm2=1;
            }
            else{alarm2=0;}
        }
        
        
        if(probestate3==1){
            if(meat3>alarmtemp){
                alarm3=1;
            }
            else{alarm3=0;}
        }

  if(probestate1+probestate2+probestate3>0){            // here we check to see if the probes are on, if not alarm off
        if(probestate1+probestate2+probestate3==alarm1+alarm2+alarm3){// with a little math we can decide if we should turn the alarm on
            alarm=1;                            // notice we only turn the alarm on if all the probes are up to temp
        }                               
        else{alarm=0;}          // If you are new to this, notice how I have the else statment inside the brackets of one of the if's
  }
  else{alarm=0;}
  
//// here we blink an output when the alarm is on
  if(alarm==1||tempalarm==1){                   // again if you are new. find a nice arduino cheat sheet and print it out and laminate it
      if ((millis() - alarmblink) < 1000){ // this value is the on time. you can change this to what you want
            digitalWrite(alarmpin, alarmon);}
      else {digitalWrite(alarmpin, alarmoff);}}

    
  if ((millis() - alarmblink) > 10000){  // this is the off time. you can change this too
    alarmblink = millis();} 

   //////////////////PID stuff/////////////////////////////////////
   //Notice here due to the nature of a smoker. We use the aggresive settings for when the temp is over the setpoint
   // this way we can dial down the integral a bit faster when it over shoots. 
   // I like to think of a smoker PID being like a car with no brakes. we are only forcing it's position one direction
   // I have noticed the I gains a lot of value as it is heating up. So naturally I would turn it down, but no matter
   // what, it takes the same amount of time to count down to 0 after it passes the setting. good old fashioned PID tunning 
   // can control this just fine, but this will help get up to heat faster and avoid over heating.... hopefully
  
  if(Input>Setpoint)
  {  //we are over the setpoint set to normal
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we are not over, so we must be below. set to aggresive
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  
  myPID.Compute();
  myPID.SetOutputLimits(0,255);
  myPID.SetSampleTime(pidsample); // takes the sample time from our pidsample
  
  //Output=constrain(Output,0,255);
  
        
        
  
      //////////////////////////////////////////////////////////////////
  //debounce for button one
  // pulses at the rate of the loop after the button is held for buttondelay amount of time
  // the debounce portion of this is simplified. It does not have a debounce timer. Instead works within the 10ms delay in the loop
                            
                            
  onestate = digitalRead(buttonpin1);
  
    if (onestate != onelaststate) {        // looking for a change in the button
      if(onestate == HIGH) {                // if the button is HIGH (in our case on)
          buttonone = 1;}
      if(onestate==LOW){                // when the button is release blynkup goes high
              blynkup=1;
          }
      }                    //it allows it to index. gets set back to 0 at end of sketch
      
  if (onestate == LOW) {                  //resets the counter saying the button is not held
    onefast = millis();} 
       
   if (onestate == HIGH) {    
        if ((millis() - onefast) > buttondelay) { //if millis exceeds button delay
    buttonone = 1;}}     // fast indexes
    
    
    
    
   //////////////////////////////////////////////////////////////////
  //debounce for button two
 twostate = digitalRead(buttonpin2);
  
    if (twostate != twolaststate) {    
      if(twostate == HIGH) {          
          buttontwo = 1;}
          if(twostate==LOW){
              blynkup=1;
          }
          }                
      
  if (twostate == LOW) {                
    twofast = millis();} 
       
   if (twostate == HIGH) {    
        if ((millis() - twofast) > buttondelay) { 
    buttontwo = 1;}}   
    
    
    
   //////////////////////////////////////////////////////////////////
  //debounce for button three
  threestate = digitalRead(buttonpin3);
  
    if (threestate != threelaststate) {      
      if(threestate == HIGH) {              
          buttonthree = 1;}
          if(threestate==LOW){
              blynkup=1;
          }
          }                
      
  if (threestate == LOW) {            
    threefast = millis();} 
       
   if (threestate == HIGH) {    
        if ((millis() - threefast) > buttondelay) { 
    buttonthree = 1;}}   
    
    
    
   //////////////////////////////////////////////////////////////////
  //debounce for button four
fourstate = digitalRead(buttonpin4);
  
    if (fourstate != fourlaststate) {    
      if(fourstate == HIGH){      
          buttonfour = 1;}
          if(fourstate==LOW){
              blynkup=1;
          }
          } 
      
  if (fourstate == LOW) {  
    fourfast = millis();} 
       
   if (fourstate == HIGH) {    
        if ((millis() - fourfast) > buttondelay) {
    buttonfour = 1;}}

   
  

 ////////////////////////////////////////////////////////////////
 //LCD screens. menus work out of the tens place and other options use the ones place
 
 
 ///////Main Screen/////////////////////////////////////////////
 if (screen == 0){
    lcd->setCursor(0, 0);
    lcd->print("Pit");
    lcd->setCursor(4, 0);
    lcd->print(inputdis);
    
    lcd->setCursor(8, 0);
    lcd->print("Out ");
    lcd->setCursor(11, 0);
    lcd->print(outputperc);
    
    lcd->setCursor(0, 1);
    lcd->print("Smoke");
    lcd->setCursor(6, 1);
    lcd->print(smokeperc);
    
    lcd->setCursor(10, 1);
    if(heatcontrol==1){
      lcd->print("Off     ");}// we add spaces to remove words after changing selection
    if(heatcontrol==2){
      lcd->print("Manual  ");}
    if(heatcontrol==3){
      lcd->print("Auto    ");}
    if(heatcontrol==4){
      lcd->print("Auto Off");}
    lcd->setCursor(19, 1);
    lcd->print(">");
    
    lcd->setCursor(0, 2);
    lcd->print("SetP ");
    lcd->print(setpointdis);
    
    lcd->print(" AlarmSP ");
    lcd->print(alarmtemp);
    
     lcd->setCursor(0, 3);
    lcd->print("1# ");
    if(probestate1 == 0){
        lcd->print("OFF");
        }
    else{lcd->print(meat1);
        }
        lcd->print(" 2# ");
    if(probestate2 == 0){
        lcd->print("OFF");
        }
    else{lcd->print(meat2);
        }
        lcd->print(" 3# ");
    if(probestate3 == 0){
        lcd->print("OFF");
        }
    else{
        lcd->print(meat3);
        }
    lcd->setCursor(15, 0);
    lcd->print("Menu>");
     
     if(buttonone == 1){
         screen = 1;buttonone = 0; lcd->clear();
        }
    
     if(buttontwo == 1){
         heatcontrol ++;buttontwo = 0;      // increase heat control by one when pressing button
         if (heatcontrol==5){heatcontrol=1;} // here we set heatcontrol back to the starting point
     }
    
    }
    
 
 ////////// Main Menu Screen //////////////////////////////////////   
if (screen == 1){   
    lcd->setCursor(8, 0);
    lcd->print("Main Screen>");
    lcd->setCursor(8, 1);
    lcd->print("Meat Probes>");
    lcd->setCursor(10, 2);
    lcd->print("Setpoints>");
    lcd->setCursor(11, 3);
    lcd->print("Settings>");
    
    if(buttonone == 1){
         screen = 0;buttonone = 0;lcd->clear();
        }
    if(buttontwo == 1){
         screen = 10;buttontwo = 0; lcd->clear();
        }
    if(buttonthree == 1){
         screen = 20;buttonthree = 0; lcd->clear();
        }
    if(buttonfour == 1){
         screen = 2;buttonfour = 0; lcd->clear();
        }
    }
    
   ///////Settings Menu///////////////////////////////////// 
if (screen == 2){   
    lcd->setCursor(16, 0);
    lcd->print("PID>");
    lcd->setCursor(15, 1);
    lcd->print("WIFI>");
    lcd->setCursor(15, 3);
    lcd->print("Back>");
    
    if(buttonone == 1){
         screen = 30;buttonone = 0;lcd->clear();
        }
    if(buttontwo == 1){
         screen = 3;buttontwo = 0; lcd->clear();
        }
    if(buttonfour == 1){
         screen = 1;buttonfour = 0; lcd->clear();
        }
    }
      
    
  ///////////WIFI//////////////////////////////////
  if (screen == 3){   
    lcd->setCursor(7, 0);
    lcd->print("Connect WIFI>");
    lcd->setCursor(4, 1);
    lcd->print("Disconnect WIFI>");
    lcd->setCursor(0, 2);
    if (Particle.connected()) {
       lcd->print("WIFI  ON"); }
       
       else{lcd->print("WIFI OFF"); }
       
    lcd->setCursor(0, 3);
        if (Blynk.connected()){
            lcd->print("App Connected"); }
            
        else{lcd->print("Not connected"); }

    lcd->setCursor(15, 3);
    lcd->print("Back>");
    
    if(buttonone == 1){
         buttonone =0;lcd->clear();
         if (Particle.connected() == false) {
                Particle.connect();}
        connect=1;
  
  }
  
    if(buttontwo==1){
        buttontwo=0;lcd->clear();WiFi.off();Blynk.disconnect();connect=0;
    }
    
    if(buttonfour == 1){
         screen = 2;buttonfour = 0; lcd->clear();
        }
    }
    
 ////////Meat Probes Menu///////////////////////////////////////////////////   
if (screen > 9 && screen < 20){
    
    lcd->setCursor(0, 0);
    lcd->print("Meat Probes");
    if(buttonone==1){
        screen++;lcd->clear();
    }
    if(screen==10){
        lcd->setCursor(0, 1);
        lcd->print(">");
        if(buttontwo==1){
            probestate1=1;lcd->clear();
        }
        if(buttonthree==1){
            probestate1=0;lcd->clear();
        }
    }
    
    if(screen==11){
        lcd->setCursor(0, 2);
        lcd->print(">");
        if(buttontwo==1){
            probestate2=1;lcd->clear();
        }
        if(buttonthree==1){
            probestate2=0;lcd->clear();
        }
    }
    
    if(screen==12){
        lcd->setCursor(0, 3);
        lcd->print(">");
        if(buttontwo==1){
            probestate3=1;lcd->clear();
        }
        if(buttonthree==1){
            probestate3=0;lcd->clear();
        }
    }
    lcd->setCursor(1,1);
    lcd->print("Probe1");
    lcd->setCursor(8,1);
    if(probestate1 == 0){
        lcd->print("OFF");
        }
    else{lcd->print("ON ");
        lcd->print(meat1);
        }
    
    lcd->setCursor(1,2);
    lcd->print("Probe2");
    lcd->setCursor(8,2);
    if(probestate2 == 0){
        lcd->print("OFF");
        }
    else{lcd->print("ON ");
        lcd->print(meat2);
        }
        
    lcd->setCursor(1,3);
    lcd->print("Probe3");
    lcd->setCursor(8,3);
    if(probestate3 == 0){
        lcd->print("OFF");
        }
    else{lcd->print("ON ");
        lcd->print(meat3);
        }
     
     
    lcd->setCursor(15,0);
    lcd->print("Next>");    
    lcd->setCursor(17,1);
    lcd->print("ON>");
    lcd->setCursor(16,2);
    lcd->print("OFF>");
    lcd->setCursor(15, 3);
    lcd->print("Back>");
    
    if(screen >12){
        screen=10;
    }
    if(buttonfour == 1){
         screen = 1;buttonfour = 0; lcd->clear();
        }
    }
    
  ///////Setpoints Menu//////////////////////////////////////////////   
if (screen > 19 && screen < 30){
    
    
    if(buttonone==1){
        screen++;lcd->clear();
    }

    if(screen==20){
        lcd->setCursor(0, 0);
        lcd->print(">");
        if(buttontwo==1){
            manualperc+=5;
        }
        if(buttonthree==1){
            manualperc-=5;
        }
    }
    
    
    if(screen==21){
        lcd->setCursor(0, 1);
        lcd->print(">");
        if(buttontwo==1){
            Setpoint++;
        }
        if(buttonthree==1){
            Setpoint--;
        }
    }
    
    if(screen==22){
        lcd->setCursor(0, 2);
        lcd->print(">");
        if(buttontwo==1){
            smokeperc+=5;
        }
        if(buttonthree==1){
            smokeperc-=5;
        }
    }
    
    if(screen==23){
        lcd->setCursor(0, 3);
        lcd->print(">");
        if(buttontwo==1){
            alarmtemp++;
        }
        if(buttonthree==1){
            alarmtemp--;
        }
    }
                 // we constrain them here. works best with the flow of the program
    alarmtemp=constrain(alarmtemp,alarmtempmin,alarmtempmax);
    smokeperc=constrain(smokeperc,0,100);
    manualperc=constrain(manualperc,0,100);
    lcd->setCursor(1,0);
    lcd->print("Manual % ");
    lcd->print(manualperc);
    
    lcd->setCursor(1,1);
    lcd->print("MainHeat SP ");
    lcd->print(setpointdis);
    
    lcd->setCursor(1,2);
    lcd->print("Smoke ");
    lcd->print(smokeperc);
        
    lcd->setCursor(1,3);
    lcd->print("AlarmTemp ");
    lcd->print(alarmtemp);
     
    lcd->setCursor(17,0);
    lcd->print("N >");    
    lcd->setCursor(17,1);
    lcd->print("+ >");
    lcd->setCursor(17,2);
    lcd->print("- >");
    lcd->setCursor(17, 3);
    lcd->print("B >");
    
    if(screen >23){
        screen=20;
    }
    if(buttonfour == 1){
         screen = 1;buttonfour = 0; lcd->clear();
        }
    
}
 
    
  ///////PID Settings screen/////////////////////////////////////////////////   
if (screen > 29 && screen < 40){
    lcd->setCursor(0, 0);
    
    if(buttonone==1){
        screen++;lcd->clear();
    }
    
    if(screen==30){
        lcd->setCursor(0, 0);
        lcd->print(">");
        if(buttontwo==1){
            consKp+=.1;
        }
        if(buttonthree==1){
            consKp-=.1;
        }
    }
    
    if(screen==31){
        lcd->setCursor(0, 1);
        lcd->print(">");
        if(buttontwo==1){
            consKi+=.01;
        }
        if(buttonthree==1){
            consKi-=.01;
        }
    }
    
    if(screen==32){
        lcd->setCursor(0, 2);
        lcd->print(">");
        if(buttontwo==1){
            consKd+=.05;
        }
        if(buttonthree==1){
            consKd-=.05;
        }
    }
    
     if(screen==33){
        lcd->setCursor(0, 3);
        lcd->print(">");
        if(buttontwo==1){
            pidsample+=100;
        }
        if(buttonthree==1){
            pidsample-=100;
        }
    }
    
       if(screen==34){
        lcd->setCursor(7, 0);
        lcd->print(">");
        if(buttontwo==1){
            aggKp+=.1;
        }
        if(buttonthree==1){
            aggKp-=.1;
        }
    }
    
    if(screen==35){
        lcd->setCursor(7, 1);
        lcd->print(">");
        if(buttontwo==1){
            aggKi+=.01;
        }
        if(buttonthree==1){
            aggKi-=.01;
        }
    }
    
    if(screen==36){
        lcd->setCursor(7, 2);
        lcd->print(">");
        if(buttontwo==1){
            aggKd+=.05;
        }
        if(buttonthree==1){
            aggKd-=.05;
        }
    }
    
 
    
    
    lcd->setCursor(1,0);
    lcd->print("P");
    lcd->print(consKp);
    
    lcd->setCursor(1,1);
    lcd->print("I");
    lcd->print(consKi);
        
    lcd->setCursor(1,2);
    lcd->print("D");
    lcd->print(consKd);
    
    lcd->setCursor(1,3);
    lcd->print("SampleTime ");
    lcd->print(pidsample);
    
    lcd->setCursor(8,0);
    lcd->print("aggP");
    lcd->print(aggKp);
    
    lcd->setCursor(8,1);
    lcd->print("aggI");
    lcd->print(aggKi);
        
    lcd->setCursor(8,2);
    lcd->print("aggD");
    lcd->print(aggKd);
     
    lcd->setCursor(18,0);
    lcd->print("N>");    
    lcd->setCursor(18,1);
    lcd->print("+>");
    lcd->setCursor(18,2);
    lcd->print("->");
    lcd->setCursor(18, 3);
    lcd->print("B>");
    
    if(screen >36){
        screen=30;
    }
    if(buttonfour == 1){
         screen = 2;buttonfour = 0; lcd->clear();
        }
    
}




//////////////////////////////////////////////////////////
  // Output stuff And Heatcontrol
  outputperc=(Output/2.55); // makes a percentage to read on the lcd
 
  if(alarm==1){             // turn the heat control off, when set to auto off and the alarm goes high
      if(heatcontrol>=4){
          heatcontrol = 1;
        }
    }
  
     if(tempalarm==1){
         if(heatcontrol>=3){// if in auto turn the heat off if it goes over the temp alarm
        heatcontrol = 1;
        }
     }
        
      
  if(heatcontrol<=2){       // when set in off 1 or manual 2 the PID switches to manual mode
       myPID.SetMode(PID::MANUAL);
       Output=manualperc*2.55;      //And the Output is set off of the manual setting that is converted to an 8 bit signal
    }
  else{myPID.SetMode(PID::AUTOMATIC);}
  
    if(heatcontrol<=1){     // here we take advantage of the manual mode in the pid and turn everything off
        Output=0;           
        smokeperc=0;
        digitalWrite(mainpin, LOW); // we set the pins low to quickly shut them off
        digitalWrite(smokepin, LOW);
        }
        
    
        
        
//fan output///////////////////////////////////////////////////////////////////////////
if(heatcontrol!=1){// we allow this block to fuction as long as heatcontrol is not off
if(outputtype==1){// fan output is type 1
if ((millis() - prepulse) <= (Output*100)){ // a timer that pulses "on" based
    outpulse=1;}                         // on the output
    else {outpulse=0;}
    
  if ((millis() - prepulse) >25500){  // here, said timers off time is the remaining
    prepulse = millis();}             // amount withing the 255 digit range.
  
  
  if (outputperc>prange){analogWrite(mainpin ,Output);}    //if it's fast enough to pwm the fan, do it 
  
  if (outputperc<=prange){
      if(outpulse==1){digitalWrite(mainpin ,LOW);}  // if it's too slow, pulse it
  else{digitalWrite(mainpin, HIGH);}
 }}}



// electric main element///////////////////////////////////////////////////////////////

if(outputtype==0){  //electric output is type 0
    if(heatcontrol!=1){
        if (outputperc>=100){digitalWrite(mainpin,HIGH);}// a strange problem here. this is to keep
        else{       // the relays from chattering when the timer reset
            if ((millis() - prepulse) < (outputperc*100)){ // a timer that pulses "on" based
                digitalWrite(mainpin ,HIGH);}                         // on the output
                else {digitalWrite(mainpin, LOW);}}

    
  if ((millis() - prepulse) >= 10000){  // here, said timer's off time is the remaining amount
    prepulse = millis();}             
   }
//smoke element timer and output//////////////////////////////////////////////////////
    if(heatcontrol!=1){
        if ((millis() - smoketimer) < smokeperc*smokeduty){ // a timer that pulses "on" based
            digitalWrite(smokepin , HIGH);}                         // on the smoke setting
        else {digitalWrite(smokepin , LOW);}

    
  if ((millis() - smoketimer) >= 10000){  // here, said timer's off time is the remaining amount
    smoketimer = millis();}     

}}

///////////////////////////////////////////////////////////////
//end of line resets
    onelaststate = onestate;  // sometimes the end of a program is not the best place for things ;)
    twolaststate = twostate;
    threelaststate = threestate;
    fourlaststate = fourstate;
    buttonone = 0;
    buttontwo = 0;
    buttonthree = 0;
    buttonfour = 0;
  delay(10);







}










/**********************************************************
 * tempRead thermistor function
 * call tempRead(analogpin) to return temp in f */


int tempRead(int THERMISTORPIN){




 uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }



  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;


  // convert the value to resistance
  average = 4095 / average - 1; //4095
  average = SERIESRESISTOR / average;


  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  fahrenheit =((steinhart*1.8)+32);
return fahrenheit;
}
