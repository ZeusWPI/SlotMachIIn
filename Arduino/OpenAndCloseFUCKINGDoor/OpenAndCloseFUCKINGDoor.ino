// ananogWrite value between 0 and 255 (8 bit value)

// Connections to motor controls
#define SER1 2
#define SER2 3

// Connections to H-bridge
#define ROT_CCLOCK 4
#define ROT_CLOCK 5
#define MOTOR_PWM 6


// Connections with the PI
#define OPEN_PIN 8
#define CLOSE_PIN 9

#define IS_OPEN_PIN 10
#define IS_CLOSE_PIN 11

// Half the max speed
//#define TURN_SPEED 175
#define TURN_SPEED 80

#define OPEN true
#define CLOSE false

// Delta equals 720? = 1 full turn, 2 interupts for 1 'tick'
#define CLOSE_THRESHHOLD -400
#define OPEN_THRESHHOLD -800

#include <Encoder.h>

bool is_open = false;
bool is_closed = false;
bool result = false;

Encoder turnCount(SER1, SER2);

void init_motor_pinout() {
  pinMode(MOTOR_PWM, OUTPUT);
//  pinMode(SER1, INPUT);
//  pinMode(SER2, INPUT);
  
  pinMode(ROT_CLOCK, OUTPUT);
  pinMode(ROT_CCLOCK, OUTPUT);
}

void turn(bool clockwise) {
  digitalWrite(ROT_CLOCK, HIGH);
  digitalWrite(ROT_CCLOCK, HIGH);

  if(clockwise) {
    digitalWrite(ROT_CLOCK, LOW);
  } else {
    digitalWrite(ROT_CCLOCK, LOW);
  }

  analogWrite(MOTOR_PWM, TURN_SPEED);
}

void stop_turn(void) {
   analogWrite(MOTOR_PWM, 0);
}

// Turn to the desired degrees
void turn_total(bool dir, long deg) {
  long prev_state, current_state;
  prev_state = turnCount.read();
  current_state = prev_state; 

  Serial.println(dir, BIN);
  if ( (!dir && ( deg > current_state)) || (dir && (deg < current_state)) ) {
    Serial.print("FUck you ");
    Serial.println(current_state);
    return;
  }
  
  Serial.print("In turn_total ");
  Serial.print("Turning ");
  Serial.println(dir, BIN);

  Serial.print(prev_state);

  turn(dir);
  delay(100);

  // Changing the lt en gt constantly because of increasing or decreasing current_turn_count on cw of ccw changing
  /**
   * 
   * This is really getting non deterministic, it is pretty bad
   * Sometimes whether or not it turns cw or ccw current_turn_count always assends or always decends which you don't want.
   * 
   * Excpected behavior is that cw and ccw at least cancel each other out.
   * Then you can say whether or not the wanted amount of degrees is bigger or smaller.
   * 
   * Idealiter we want that for example cw always goes assending, and ccw always decends. How does one do this?
   * 
   * Another challenging point is that everything shouldn't break when one interupt is missed (can be ignore with lower turn speeds.
   * 
   * 
   * Let's do a library lol
   */
  
   
  while(( (dir && (deg > current_state)) || (!dir && (deg < current_state)) )  && prev_state != current_state) {
    prev_state = current_state;
    current_state = turnCount.read();
    delay(100);
  }

  Serial.print(" -> ");
  Serial.println(turnCount.read());

  stop_turn();

//  update_openness();
}

void determine_start(void) {
  turn(CLOSE);
  
  turn_total(CLOSE, 7000);
  turnCount.write(0);

  turn_total(OPEN, -20);
}

void stop() {
  stop_turn();
  exit(0);
}

void setup() {
//  stop();
  Serial.begin(9600);

  init_motor_pinout();
  pinMode(OPEN_PIN, INPUT);
  digitalWrite(OPEN_PIN, HIGH);
  pinMode(CLOSE_PIN, INPUT);
  digitalWrite(CLOSE_PIN, HIGH);

  pinMode(IS_OPEN_PIN, OUTPUT);
  pinMode(IS_CLOSE_PIN, OUTPUT);
//  attachInterrupt(digitalPinToInterrupt(SER1), ser1_int, CHANGE);
//  attachInterrupt(digitalPinToInterrupt(SER2), ser2_int, CHANGE);

  pinMode(LED_BUILTIN, OUTPUT); //Tentative

  //determine_start();

/*  ser1 = !GETSER1;
//  ser2 = ser1;

  Serial.print("Ser 1 ");
  Serial.println(ser1);
  Serial.print("Ser 2 ");
  Serial.println(ser2);  
  */
  
}


// Turn to the desired degrees
// Maxi's function rewrite
bool better_turn_total(bool dir, long deg) {
  Serial.println("Start better_turn_total ");
  long prev_state, current_state;
  prev_state = turnCount.read();
  current_state = prev_state;

  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Starting from state ");
  Serial.println(prev_state);
  Serial.print("Desired state: ");
  Serial.println(deg);
  Serial.print("Turning direction: ");
  Serial.println(dir, BIN);

  /*
  if ( (!dir && ( deg < current_state)) || (dir && (deg > current_state)) ) {
    Serial.print("Requested degrees cannot be obtained in this direction. Current state is: ");
    Serial.println(current_state);
    return false;
  }
  */
  
  //Serial.print("Turning ");
  //Serial.println(dir, BIN);
  //Serial.print(prev_state);

  //Tentative
  if(deg > prev_state) dir = CLOSE;
  else if(deg <= prev_state) dir = OPEN;
  else Serial.print("You dun goofed");
  //End tentative

  turn(dir);
  delay(20);
  if(dir){
    Serial.println("Dir is TRUE");
    while(deg < current_state){
      Serial.println("Inside the WHILE");
      Serial.print("Deg is: ");
      Serial.println(deg);
      Serial.print("current_state is: ");
      Serial.println(current_state);
      delay(20);
      current_state = turnCount.read();
    }
  }
  if(!dir){
    Serial.println("Dir is FALSE");
    while(deg > current_state){
      Serial.println("Inside the WHILE");
      Serial.print("Deg is: ");
      Serial.println(deg);
      Serial.print("current_state is: ");
      Serial.println(current_state);
      delay(20);
      current_state = turnCount.read();
    }
  }
  /*
  while(( (dir && (deg > current_state)) || (!dir && (deg < current_state)) )) {
    //  && prev_state != current_state
//    prev_state = current_state;
    current_state = turnCount.read();
    delay(100);
  }
  */
  stop_turn();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  Serial.print(prev_state);
  Serial.print(" -> ");
  Serial.println(turnCount.read());

  Serial.println("Better_turn_count return complete");
  digitalWrite(LED_BUILTIN, LOW);
  return true;

//  update_openness();
}

void loop() {
  bool want_open = digitalRead(OPEN_PIN) == HIGH;
  bool want_closed = digitalRead(CLOSE_PIN) == HIGH;

  if (want_open != want_closed) {
    Serial.print("Want open ");
    Serial.print(want_open, BIN);
    Serial.print(" is open ");
    Serial.println(is_open, BIN);
    Serial.print("Want close ");
    Serial.print(want_closed, BIN);
    Serial.print(" is closed ");
    Serial.println(is_closed, BIN);

    if (want_closed && !is_closed) {
      Serial.println("Closing");
      result = better_turn_total(CLOSE, 180);
      if(result){
        is_closed = true;
        is_open = false;
      }
    }

    if (want_open && !is_open) {
      Serial.println("Opening");
      result = better_turn_total(OPEN, 40);
      if(result){
        is_closed = false;
        is_open = true;
      }
    }
    
  }

    Serial.print("Want open ");
    Serial.print(want_open, BIN);
    Serial.print(" is open ");
    Serial.println(is_open, BIN);
    Serial.print("Want close ");
    Serial.print(want_closed, BIN);
    Serial.print(" is closed ");
    Serial.println(is_closed, BIN);
  Serial.println(turnCount.read());

  delay(500);
}
