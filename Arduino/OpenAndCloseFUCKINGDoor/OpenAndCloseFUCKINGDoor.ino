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
#define TURN_SPEED 200

#define GETSER1 (digitalRead(SER1) == HIGH)
#define GETSER2 (digitalRead(SER2) == HIGH)

#define OPEN false
#define CLOSE true

// Delta equals 720? = 1 full turn, 2 interupts for 1 'tick'
#define CLOSE_THRESHHOLD -400
#define OPEN_THRESHHOLD -800
                                // 11         10              00              01
const int cwSequence[] = {3, 2, 0, 1};
// The inverse of cwSequence: cwSequence[reverseCwSequence[i]] = i
const int reverseCwSequence[] = {2, 3, 1, 0};
const int pulses_per_turn = 720; // http://trivox.tripod.com/lego-nxt-motor-input-output.html

volatile int current_turn_count = 0;
volatile int prev_cw_index = 0;

volatile int ser1 = 0;
volatile int ser2 = 0;

bool is_open = false;
bool is_closed = false;

int get_cw_index(bool ser1, bool ser2) {
  int out = 0;
  if (ser1) {
    out += 2;
  }
  if (ser2) {
    out += 1;
  }
  return reverseCwSequence[out];
}

void init_motor_pinout() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(SER1, INPUT);
  pinMode(SER2, INPUT);
  pinMode(ROT_CLOCK, OUTPUT);
  pinMode(ROT_CCLOCK, OUTPUT);
}

void update_openness() {

  if (is_open != current_turn_count < OPEN_THRESHHOLD) {
    is_open = current_turn_count < OPEN_THRESHHOLD;
    if (is_open) {
      digitalWrite(IS_OPEN_PIN, HIGH);
    } else {
      digitalWrite(IS_OPEN_PIN, LOW);
    }
  }

  if (is_closed != current_turn_count > CLOSE_THRESHHOLD) {
    is_closed = current_turn_count > CLOSE_THRESHHOLD;
    if (is_closed) {
      digitalWrite(IS_CLOSE_PIN, HIGH);
    } else {
      digitalWrite(IS_CLOSE_PIN, LOW);
    }
  }
}

void update_current_turn() {

  int index = get_cw_index(GETSER1, GETSER2);

  if ((index + 1) % 4 == prev_cw_index) {
   // current_turn_count ++;
  } else if (index == (prev_cw_index + 1) % 4) {
   // current_turn_count --;
  }

  prev_cw_index = index;

  Serial.print("turn count ");
  Serial.println(index);

  //update_openness();
}

void ser1_int() {
  if (ser1 == ser2) {
    current_turn_count ++;
    ser1 = ser1 + 1;
  } else {
    current_turn_count --;
    ser1 = ser1 - 1;
  }

}

void ser2_int() {
  if (ser1 == ser2){
    ser2 = ser2 - 1;
  } else {
    ser2 = ser2 + 1;
  }
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
void turn_total(bool dir, int deg) {
  Serial.print("In turn_total");
  Serial.print("Turning ");
  Serial.println(dir, BIN);

  int prev_state = current_turn_count;

  Serial.println(current_turn_count);

  turn(dir);
  delay(100);

  while(( (dir && deg < current_turn_count) || (!dir && deg > current_turn_count) )  && prev_state != current_turn_count) {
    prev_state = current_turn_count;
    delay(100);
  }

  Serial.println(current_turn_count);

  stop_turn();

  update_openness();
}

void determine_start(void) {
  turn_total(CLOSE, 7000);
  current_turn_count = 0;

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
  attachInterrupt(digitalPinToInterrupt(SER1), ser1_int, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SER2), ser2_int, CHANGE);


  ser1 = !GETSER1;
  ser2 = !GETSER2;
  
  determine_start();
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
      turn_total(CLOSE, -50);
    }

    if (want_open && !is_open) {
      turn_total(OPEN, -360);
    }
  }

  Serial.println(current_turn_count);
  Serial.print("Ser 1 ");
  Serial.println(ser1);
  Serial.print("Ser 2 ");
  Serial.println(ser2);

  delay(500);
}


