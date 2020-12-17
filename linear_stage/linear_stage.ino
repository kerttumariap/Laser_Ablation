// Digital pins 5 and 6 are used for step and direction.
# define stp 5
# define dir 6
// Digital pins 2 and 3 are Arduino Uno attachInterrupt pins and are used for the two endstops.
#define end1 2
#define end2 3

float serial_read_delay = 500;
float serial_write_delay = 1000;
unsigned long serial_read_time = millis();
unsigned long serial_write_time = millis();
unsigned long loop_time = millis();

int event_code = 0;

unsigned long step_time = micros();
unsigned long velocity1 = 1000;
unsigned long velocity_delay_micros = velocity1;
int direction = 1;
long abs_pos = 0;
long steps_to_do = 0;



void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  Serial.begin(9600);
  reset_pins();
  attachInterrupt(digitalPinToInterrupt(end1), detect_endstop1, FALLING);
  attachInterrupt(digitalPinToInterrupt(end2), detect_endstop2, FALLING);
}

void loop() {
  loop_time = millis();
  serial_read();
  n_steps();
  serial_write();
}

// ---------------- Serial Functions ----------------

void serial_read(){
  String serial_string;
  if ((unsigned long)(loop_time - serial_read_time) >= serial_read_delay){
    serial_read_time = loop_time;
    if (Serial.available() > 0){
      serial_string = Serial.readString();
      if (serial_string.endsWith("r")) {
        categorize_cmd(serial_string);
      }
    }
  }
}


void serial_write(){
  if ((unsigned long)(loop_time - serial_write_time) >= serial_write_delay){
    serial_write_time = loop_time;
    Serial.print(loop_time);
    Serial.print(";");
    Serial.print(abs_pos);
    Serial.print(";");
    Serial.print(steps_to_do);
    Serial.print(";");
    Serial.print(velocity_delay_micros);
    Serial.print(";");
    Serial.print(direction);
    Serial.print(";");
    Serial.print(event_code);
  }
}


void categorize_cmd(String serial_string){
  int index_r = serial_string.indexOf("r\n");

  if (serial_string.startsWith("R")){
    reset_steps();
  }
  else if (serial_string.startsWith("E")){
    // Set event_code
    int serial_event = serial_string.substring(1, index_r).toInt();
    event_code = serial_event;
  }
  else if (serial_string.startsWith("D")){
    // Direction can't be changed while motor is moving.
    if (steps_to_do == 0){
      int serial_direction = serial_string.substring(1, index_r).toInt();
      set_direction(serial_direction);
    }
  }
  else if (serial_string.startsWith("V")){
    int serial_velocity = serial_string.substring(1, index_r).toInt();
    set_velocity(serial_velocity);
  }
  else if (serial_string.startsWith("S")){
    // Variable steps_to_do can't be changed while motor is moving.
    if (steps_to_do == 0) {
      long serial_steps = atol(serial_string.substring(1, index_r).c_str());
      set_steps_to_do(serial_steps);
    }
  }
  else if (serial_string.startsWith("A")){
    // Set absolute position
    long serial_abs_pos = atol(serial_string.substring(1, index_r).c_str());
    set_abs_pos(serial_abs_pos);
  }
}

// ---------------- Set Parameters ----------------

void set_direction(int new_direction){
  if (new_direction == 1){
    direction = new_direction;
    digitalWrite(dir, HIGH);
    //Serial.println("Direction was changed to 1");
  }
  else if (new_direction == -1){
    direction = new_direction;
    digitalWrite(dir, LOW);
    //Serial.println("Direction was changed to -1");
  }
}

void set_velocity(int new_velocity){
  velocity_delay_micros = new_velocity;
}

void set_steps_to_do(long new_steps){
  steps_to_do = new_steps;
}

void set_abs_pos(long new_abs_pos){
  abs_pos = new_abs_pos;
}


// ---------------- Step Functions ----------------

void n_steps() {
  if (steps_to_do > 0){
    if ((unsigned long) (micros() - step_time) >= velocity_delay_micros){
      step_time = micros();
      single_step();
      steps_to_do--;
      abs_pos += direction;
    }
  }
}

void single_step() {
  digitalWrite(stp, HIGH);
  delayMicroseconds(3);
  digitalWrite(stp, LOW);
}

// ---------------- Endstop Functions ----------------

void detect_endstop1() {
  reset_steps();
  event_code = 1;
}


void detect_endstop2() {
  reset_steps();
  event_code = 2;
}

// ---------------------- Reset -----------------------

void reset_pins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, HIGH);
}


void reset_steps(){
  set_steps_to_do(0);
}
