# Intro to Embedded Systems

Group 5 : We Love CPP

## Power

You need to supply power to the device, You can use USB power or other power supplies that can provide 5V

## Modes

There are 4 modes to this device

### Mode 0 : Auto Mode

In this mode the Ultrasonic sensor will measure the speed and display the traffic condition as an LED with RED being heavy traffic, ORANGE being medium traffic and GREEN being no traffic/low traffic

#### Conditions

**Distance** >= 20 cm -> **Green Led**

**Speed** >= 12 cm/s -> **Green Led**

**Distance** < 20 cm

- **Speed** < 3 cm/s -> **Red Led**
- 3 cm/s < **Speed** < 12 cm/s -> **Orange Led**

### Mode 1 : Green

In this mode the LED will alway display GREEN essentially a bypass mode to always show green, In this mode the ultrasonic sensor has no effect on the LED color

### Mode 2 : Red

In this mode the LED will alway display RED essentially a bypass mode to always show red, In this mode the ultrasonic sensor has no effect on the LED color

### Mode 4 : Error

This mode will occour in two conditions, one when an interrupt occours or the vibration sensor detects a vibration (A car hitting the traffic pole) or when the water sensor detects water (The road is flooded)

## Controls

There are multiple way to control the system

### IR Remote

Using a remote we can change the mode on the device on the fly

|Button Number | Function|
|------------ | -------------|
|Button 0  | Change mode to Mode 0 (Auto Mode)|
|Button 1  | Change mode to Mode 1 (Green Mode)|
|Button 2  | Change mode to Mode 2 (Red Mode)|

### Joystick

Using the Joystick we can control the stepper motor and servo motor combination to have a Full Motion X and Y movement that can be used with a CCTV to surveil the surrounding area

## LCD

On the LCD it will display the current time, temperature and mode.
