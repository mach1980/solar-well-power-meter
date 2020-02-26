# solar-well-power-meter
The Solar Well Power Meter is a functional art piece made for Torpet. A description of Torpet:

"In a dream of a not so far away future, exists a house on wheels. The house is called Torpet and she draws her life from the sun. In turn she lets that energy flow on, to nourish all human beings, equiped with a heart, through an array of Magical tunes and happenings. Though she is on the path of the sun she has learned to harness the primordial powers of gas and flames to invoke awe and wonder, when the time is ripe. Her tinyhouse frame, wheels and solar charged batteries makes her nimble enough to appear in places normally not fit for a soundsystem. Her biggest passion is the creation of fleeting moments of joy and beauty that will forever be stored in the heart-discs of the people, to leave them with a sense of hope and optimism, for a future where more houses might be build like her."

The Solar Well Power Meter is solving the problem of visualizing the power left in the sun-charged batteries by filling a clear pipe with water where the water level corresponds to the power level. Lots of water - lots of power, no water - no power left.

## Parts

### Pipe
A clear acrylic (PMMA) pipe standing horizontally.

### Box
A wooden box hiding everything except the pipe.

### Sump
A receptacle within the box that stores the water when its not in the pipe.

### Pump
A electrically powered pump re-fills the pipe with water when activated.

### Pressure Sensor
A pressure sensor measures the water level in the pipe.

### Valve
A electrically opererated valve makes it possible to empty the pipe. Water drained flows back into the sump.

### Electrical System
A mess of wiring enables controlling the valve and pump based on input in the form of battery voltage and pressure sensor.

#### Control System

Continous control methods are not possible due to the solenoid valve and the water pump power influencing the battery voltage when activated. This leads to a state machine setup where the measuring and actuation is seperated. A wait-state is introduced that enables the battery voltage to go back to equilibirum when not loaded by actuators.

```
+-----------+
|           |
|  Measure  +<----+
|           |     |
+-----+-----+     |
      |           |
      v           |
+-----+-----+     |
|           |     |
|    Act    |     |
|           |     |
+-----+-----+     |
      |           |
      v           |
+-----+-----+     |
|           |     |
|   Wait    +-----+
|           |
+-----------+
```

## Prototype
Prototype for the solar well power meter consists of a 1000 x 40mm ⌀ clear PMMA pipe protruding from a 300x600 mm black plywood box. As a sump and pump a 1.5l windshield wiper system is used.
