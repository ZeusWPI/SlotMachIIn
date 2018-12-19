
/*
  FUCKING FUCK THIS FUCKIN KAK SHIT
*/

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
#define TURN_SPEED 128

#define GETSER1 (digitalRead(SER1) == HIGH)
#define GETSER2 (digitalRead(SER2) == HIGH)

#define OPEN true
#define CLOSE false

// TODO, experiment!
#define CLOSE_THRESHHOLD 50
#define OPEN_THRESHHOLD 100
                                // 11         10              00              01
//const TurnTuple cwSequence[] = {{true, true}, {true, false}, {false, false}, {false, true}};
const int pulses_per_turn = 180; // http://trivox.tripod.com/lego-nxt-motor-input-output.html

volatile int current_turn_count = 0;

volatile bool prev_ser1, prev_ser2 = false;

bool is_open = false;

void init_motor_pinout() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(SER1, INPUT);
  pinMode(SER2, INPUT);
  pinMode(ROT_CLOCK, OUTPUT);
  pinMode(ROT_CCLOCK, OUTPUT);
}

int is_turning_clockwise(bool ser1, bool ser2) {
   if(ser1 == prev_ser1) {
    return (prev_ser2 && !ser2) || (!prev_ser2 && ser2);
   } else {
    return (prev_ser1 && !ser1) || (!prev_ser1 && ser1);
   }
}

void update_ser1(void) {
  current_turn_count += is_turning_clockwise(GETSER1, prev_ser2);
  prev_ser1 = GETSER1;
  Serial.print("Ser1");
  Serial.print(prev_ser1);

}

void update_ser2(void) {
  current_turn_count += is_turning_clockwise(prev_ser1, GETSER2);
  prev_ser1 = GETSER2;
  Serial.print("Ser2");
  Serial.print(prev_ser2);
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

void turn_total(bool dir) {
  int prev_state = current_turn_count;
  turn(dir);
  delay(100);
  
  while(1 || prev_state != current_turn_count) {
    prev_state = current_turn_count;
    delay(100);
  }
  
  stop_turn();
}

void determine_start(void) {
  turn_total(CLOSE);
  
  current_turn_count = 0;
  is_open = false;
}

void setup() {
  Serial.begin(9600);

  init_motor_pinout();
  pinMode(OPEN_PIN, INPUT);
  pinMode(CLOSE_PIN, INPUT);
  pinMode(IS_OPEN_PIN, OUTPUT);
  pinMode(IS_CLOSE_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(SER1), update_ser1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SER2), update_ser2, CHANGE);

  //determine_start();
}

void loop() {
  bool want_open = digitalRead(OPEN_PIN) == HIGH;
  bool want_closed = digitalRead(CLOSE_PIN) == HIGH;

  if (want_open != want_closed) {
    if (want_open && current_turn_count < OPEN_THRESHHOLD) {
      turn_total(OPEN);
    } 
    
    if (want_closed && current_turn_count > CLOSE_THRESHHOLD) {
       turn_total(CLOSE);
    }
  }

  delay(100);
}


