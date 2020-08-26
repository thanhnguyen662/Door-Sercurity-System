#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Keypad.h>
#include <Servo.h>
#include <string.h>

// Create LCD Widget object on Blynk
WidgetLCD lcd(V1);

//Connect to Blynk App
char auth[] = "G6j2Kra8yTkQ2QsRdLfNrDw5nkVu3976";
char ssid[] = "Gau 2.4GHz";
char pass[] = "0762634797";

// Define some constants and variables
const byte rows = 4;
const byte columns = 3;
int holdDelay = 700;
int n =3;
int state = 0;
char key = 0;

// Create an instance for servo motor
Servo servo_d8;

//Default password is 0000
String default_passwd = "0000";
//variable to store the user input for password
String input_passwd = "";

//Define keys function on Keypad
char lock_key = '*';
char unlock_key = '#';
char change_pass_key = '-'; //Press 3000ms

//Define characters matrix on Keypad
char keys[rows][columns] = 
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};

//Row of keypad
byte rowPins[rows] = {D0, D1, D2, D3};
//Column of keypad
byte columnPins[columns] = {D4, D5, D6};
// Instance for our keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);


void setup()
{
  // Debug console
  Serial.begin(9600);
  
  //Inititate Blynk
  Blynk.begin(auth, ssid, pass);
  
  //Setup LED
  pinMode(D7, OUTPUT);
  //Connect Servo on D8
  servo_d8.attach(D8);
}


//Setup for Blynk
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();

  if(pinValue == 1)
  {
    digitalWrite(D7, HIGH);
  servo_d8.write(0);
  lcd.clear(); 
  lcd.print(0, 0, "Hi Sir ^_^");
  lcd.print(0, 1, "Status: Unlocked");
  }
  else{ 
    digitalWrite(D7, LOW);
  servo_d8.write(90);
  lcd.clear(); 
  lcd.print(0, 0, "Hi Sir ^_^");
  lcd.print(0, 1, "Status: Locked");
  }
}

void loop()
{
  Blynk.run();
  Key_pressed();
}
// Define Unlock() funtion
void Unlock()
{
  digitalWrite(D7, HIGH); 
  lcd.clear(); 
  // Servo = 0
  servo_d8.write(0);
  // wait 15 ms for servo to reach the position
  delay(15);
  //Print Status to LCD Widget on Blynk App
  lcd.clear(); 
  lcd.print(0, 0, "Press * to Lock");
  lcd.print(0, 1, "Status: Unlocked");   
}

// Define Lock() funtion
void Lock(){
  // Servo = 90
  servo_d8.write(90);
  // wait 15 ms for servo to reach the position
  delay(15);
  //Print Status to LCD Widget on Blynk App
  lcd.clear(); 
  lcd.print(0, 0, "Hi Sir ^_^");
  lcd.print(0, 1, "Status: Locked");
  digitalWrite(D7, LOW);
}

// Keypad_press
void Key_pressed()
{
  key = function_key(n);
  
  if (key == unlock_key) 
  {
  Serial.print("Input password: ");
  lcd.print(0, 1, "Input Password: ");
  input_passwd = input_password(4);
  if (input_passwd == default_passwd)
  {
    //Unlock by servo_d8 and send alert to LCD Widget
    lcd.clear();
    lcd.print(0, 0, "Unlocking...");
    delay(1000);
    Unlock();
  }
  else 
  {
    //Send alert to LCD Widget
    Serial.println("Password Incorrect"); 
    lcd.clear(); 
    lcd.print(0, 0, "Incorret Press #");
    lcd.print(0, 1, "to input again");
    for(int i=1; i<11; i++){
       digitalWrite(D7, HIGH);
       delay(100);
       digitalWrite(D7, LOW);
       delay(100);
    }
  }
  //Reset input_passwd
  input_passwd = "";
    key = 0;
  }

  if (key == lock_key)
  {
    Lock();
    delay(500);
  }
      
  if (key == change_pass_key)
  {
    lcd.print(0, 1, "Change Password. ");
    for(int i=1; i<3; i++){
       digitalWrite(D7, HIGH);
       delay(1000);
       digitalWrite(D7, LOW);
       delay(1000);
    } 
  default_passwd = change_password(4, default_passwd);
    delay(2000);
    key =0;
  }
}
// Define function for key
char function_key(int n)
{
  char temp = keypad.getKey();
  if ((int)keypad.getState() == PRESSED)
  {
    if (temp != 0) {key = temp;}    
  }
  if ((int)keypad.getState() == HOLD)
  {
    state++;
    state = constrain(state, 1, n);
    delay(holdDelay);
  }  
  if ((int)keypad.getState() == RELEASED)
  {
    key += state;
    state = 0;  
  }  
  delay(100);
  Serial.println(key);
  return key;
}
// Define function input_password
String input_password(int num_char)
{
  String passwd = "";
  do 
  {
    char temp = keypad.getKey();
  if (temp != 0) {
    Serial.print(temp); passwd += temp; 
    lcd.clear();
    lcd.print(0, 0, "Input Password: ");
    lcd.print(0, 1, passwd);
    }
    delay(100);
  }
  while (passwd.length() < num_char);
  Serial.println();
  return passwd;
}
// Define function change_password
String change_password(int num_char, String current_passwd)
{
  String passwd = "";
  //Authenticate the old password:
  Serial.print("Input old password: ");
  lcd.clear();
  lcd.print(0, 0, "Input Old Password");
  delay(2000);
  lcd.print(0, 1, passwd);
  String old_passwd = input_password(num_char);
  if (old_passwd != current_passwd)
  {
    Serial.println("Password does not match! Nothing changes");
    for(int i=1; i<11; i++){
       digitalWrite(D7, HIGH);
       delay(100);
       digitalWrite(D7, LOW);
       delay(100);
    }
  return current_passwd; 
  }
  //New password 
  Serial.print("Input new password: ");
  lcd.clear();
  lcd.print(0, 0, "Input New Password 2 times");
  for(int i=1; i<3; i++){
       digitalWrite(D7, HIGH);
       delay(500);
       digitalWrite(D7, LOW);
       delay(500);
    }
  String new_passwd = input_password(num_char);
  //Confirm passwd
  Serial.print("Input new password again: ");
  for(int i=1; i<3; i++){
       digitalWrite(D7, HIGH);
       delay(500);
       digitalWrite(D7, LOW);
       delay(500);
    }
  String confirm_passwd = input_password(num_char);
  
  if (confirm_passwd == new_passwd)
  {
  Serial.println("Password has changed!!!");
  lcd.clear();
  lcd.print(0, 0, "Password has");
  lcd.print(0, 1, "changed!!!");
  for(int i=1; i<3; i++){
       digitalWrite(D7, HIGH);
       delay(500);
       digitalWrite(D7, LOW);
       delay(500);
    }
  return confirm_passwd;
  }
  else
  {
  Serial.println("Password does not match! Nothing changes");
  lcd.clear();
  lcd.print(0, 0, "Incorrect, nothing changed");
  for(int i=1; i<11; i++){
       digitalWrite(D7, HIGH);
       delay(100);
       digitalWrite(D7, LOW);
       delay(100);
    }
  return current_passwd;
  }
}
