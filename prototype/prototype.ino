/**
 * Analog input pin on Arduino board connected to voltage divider taking battery voltage.
 */
#define PIN_BATTERY A2
#define BATTERY_LOW_VOLTAGE 10500
#define BATTERY_HIGH_VOLTAGE 14000
#define ADC_MIN 0
#define ADC_MAX 1023
#define VOLTAGE_DIVIDER_12V_VOLTAGE_AT_MAX_ADC 16230

/**
 * Analog input pin on Arduino board connected to amplifier of water pressure sensor.
 */
#define PIN_WATER_PRESSURE A4
#define PRESSURE_SENSOR_LOW_VOLTAGE 596 // Pressure sensor at 0 cm overhead water
#define PRESSURE_SENSOR_HIGH_VOLTAGE 642
#define WATER_HEIGHT_MAX 90

/**
 * Digital output pin on Arduino board connected to relay controlling water pump.
 * 
 * Setting this pin to HIGH will fill the pipe with water.
 */
#define PIN_RELAY_PUMP 5

/**
 * Digital output pin on Arduino board connected to relay controlling solenoid valve.
 * 
 * Setting this pin to HIGH will empty the pipe of water.
 */
#define PIN_RELAY_VALVE 4

#define HYSTERIS 3 // Percentage hysteris
#define LOGGING true
#define WAIT_STATE_COUNT 500
#define MAIN_LOOP_WAIT 10

int batterySensor = 0;      // Value read from the voltage divider
int batteryVoltage = 0;     // Battery voltage in mV
int batteryPercentage = 0;  // Battery capacity in percentage

int pressureSensor = 0;     // value read from water pressure sensor. The sensor reads 0-5V for 0-16 ATM
int waterHeight = 0;        // Water height in cm
int waterPercentage = 0;    // Water height in percentage

const int STATE_INIT = 0;
const int STATE_MEASURE = 1;
const int STATE_ACT = 2;
const int STATE_WAIT = 3;

int s_state = STATE_INIT;
int s_pending_state;
int s_counter;
int s_init;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(PIN_RELAY_PUMP, OUTPUT);
  digitalWrite(PIN_RELAY_PUMP, LOW);
  pinMode(PIN_RELAY_VALVE, OUTPUT);
  digitalWrite(PIN_RELAY_VALVE, LOW);
}

/**
 * Main loop is to execute with a 10ms wait 
 */
void loop() {

  // Execute state
  switch(s_state) {
    case STATE_INIT:
      state_init();
      break;
    case STATE_MEASURE:
      state_measure();
      break;
    case STATE_ACT:
      state_act();
      break;
    case STATE_WAIT:
      state_wait();
      break;
  }

  // Change to pending state
  nextState();

  // Wait
  delay(MAIN_LOOP_WAIT);

/**

  if (batteryPercentage > waterPercentage + HYSTERIS) {
    digitalWrite(PIN_RELAY_PUMP, HIGH);
  } else {
    digitalWrite(PIN_RELAY_PUMP, LOW);
  }

  if (waterPercentage > batteryPercentage + HYSTERIS) {
    digitalWrite(PIN_RELAY_VALVE, HIGH);
  } else {
    digitalWrite(PIN_RELAY_VALVE, LOW);
  }
  
  delay(1000);

  **/
}

void state_init() {
  if (LOGGING) {
    logStart();
    Serial.println("\n\n\t Solar Power Well Meter - Prototype v1.0\n\n");

    Serial.print("\t BATTERY_LOW_VOLTAGE: ");
    Serial.println(BATTERY_LOW_VOLTAGE);
    Serial.print("\t BATTERY_HIGH_VOLTAGE: ");
    Serial.println(BATTERY_HIGH_VOLTAGE);
    Serial.print("\t VOLTAGE_DIVIDER_12V_VOLTAGE_AT_MAX_ADC: ");
    Serial.println(VOLTAGE_DIVIDER_12V_VOLTAGE_AT_MAX_ADC);

    Serial.print("\n\t PRESSURE_SENSOR_LOW_VOLTAGE: ");
    Serial.println(PRESSURE_SENSOR_LOW_VOLTAGE);
    Serial.print("\t PRESSURE_SENSOR_HIGH_VOLTAGE: ");
    Serial.println(PRESSURE_SENSOR_HIGH_VOLTAGE);
    Serial.print("\t WATER_HEIGHT_MAX: ");
    Serial.println(WATER_HEIGHT_MAX);

    Serial.print("\n\t MAIN_LOOP_WAIT: ");
    Serial.println(MAIN_LOOP_WAIT);
    Serial.print("\t HYSTERIS PERCENTAGE: ");
    Serial.println(HYSTERIS);
    logEnd();
  }
  s_pending_state = STATE_MEASURE;
}

void state_measure() {
  // read the analog values
  batterySensor = analogRead(PIN_BATTERY);
  batteryVoltage = convert12VBatterySensorToVoltage(batterySensor);
  batteryPercentage = convertBatteryVoltageToPercentage(batteryVoltage);
  if (batteryPercentage > 100) {
    batteryPercentage = 100;
  } else if (batteryPercentage < 0) {
    batteryPercentage = 0;
  }

  pressureSensor = analogRead(PIN_WATER_PRESSURE);
  waterHeight = map(pressureSensor, PRESSURE_SENSOR_LOW_VOLTAGE, PRESSURE_SENSOR_HIGH_VOLTAGE, 0, 90);
  waterPercentage = map(waterHeight, 0, WATER_HEIGHT_MAX, 0, 100);
  if (waterPercentage > 100) {
    waterPercentage = 100;
  } else if (waterPercentage < 0) {
    waterPercentage = 0;
  }

  if (LOGGING) {
    logStart();
    logBatteryToSerial();
    logWaterToSerial();
    logEnd();
  }

  s_pending_state = STATE_ACT;
}

void state_act() {
  if (s_init) {
    int error = batteryPercentage - waterPercentage;
    
    // If error > 0 it means that the battery has higher percentage charge than water is indicating
    if (error > HYSTERIS) {
      s_counter = 50;
      digitalWrite(PIN_RELAY_PUMP, HIGH);
    }

    if (error < -HYSTERIS) {
      s_counter = 10;
      digitalWrite(PIN_RELAY_VALVE, HIGH);
    }

    if (LOGGING) {
      logStart();
      Serial.print("\t Error: ");
      Serial.println(error);
      logEnd();
    }
  }
  
  s_counter = s_counter - 1;
  if (LOGGING) {
    if (s_counter % 80) {
      Serial.print("*");
    } else {
      Serial.println("*");
    }
  }
  if (s_counter <= 0) {
    digitalWrite(PIN_RELAY_PUMP, LOW);
    digitalWrite(PIN_RELAY_VALVE, LOW);
    if (LOGGING) {
      logEnd();
    }
    s_pending_state = STATE_WAIT;
  }
}

void state_wait() {
  if (s_init) {
    s_counter = WAIT_STATE_COUNT;
    if (LOGGING) {
      logStart();
    }
  }
  
  s_counter = s_counter - 1;
  if (LOGGING) {
    if (s_counter % 80) {
      Serial.print("*");
    } else {
      Serial.println("*");
    }
  }

  if (s_counter <= 0) {
    s_pending_state = STATE_MEASURE;
    if (LOGGING) {
      Serial.println("*");
      logEnd();
    }
  }
}

void nextState() {
  if (s_state == s_pending_state) {
    // No state change - early return
    s_init = false;;
    return;
  }
  
  if (LOGGING) {
    logStart();
    Serial.print("\t state change: ");
    Serial.print(s_state);
    Serial.print(" -> ");
    Serial.println(s_pending_state);
    logEnd();
  }
  s_init = true;
  s_state = s_pending_state;
}

int convert12VBatterySensorToVoltage(int analogValue) {
  // Each step corresponds to 15.8548 mV
  return map(analogValue, ADC_MIN, ADC_MAX, 0, VOLTAGE_DIVIDER_12V_VOLTAGE_AT_MAX_ADC);
}

int convertBatteryVoltageToPercentage(int voltage) {
  return map(voltage, BATTERY_LOW_VOLTAGE, BATTERY_HIGH_VOLTAGE, 0, 100);
}

void logBatteryToSerial() {
  Serial.print("\t batterySensor = ");
  Serial.print(batterySensor);
  Serial.print("\t battery voltage = ");
  Serial.print(batteryVoltage);
  Serial.print("\t battery percentage = ");
  Serial.println(batteryPercentage);
}

void logWaterToSerial() {
  Serial.print("\t pressureSensor = ");
  Serial.print(pressureSensor);
  Serial.print("\t water height = ");
  Serial.print(waterHeight);
  Serial.print("\t\t water percentage = ");
  Serial.println(waterPercentage);
}

void logStart() {
  Serial.println("<<<<<<");
}

void logEnd() {
  Serial.println(">>>>>>");
}
