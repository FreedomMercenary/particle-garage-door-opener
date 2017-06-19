// This #include statement was automatically added by the Particle IDE.
#include <HC_SR04.h>

const int doorPin = D0;
const int echoPin = D3;
const int ledPin = D7;
const int lightPin = D1;
const int pingPin = D2;
HC_SR04 rangeFinder = HC_SR04(pingPin, echoPin, 10, 1000);
double distanceInches = 0.0;
double lastDistanceForVelocity = 0.0;
int runTimes = 0;
String status;
double velocity = 0.0; // Inches per second
int velocityCounter = 0;

String getStatus(double, double);
int toggleDoor(String);
int toggleLight(String);
void writeDoorPin();
void writeLightPin();

// Setup method to configure pins and web service endpoints
void setup() {
    pinMode(doorPin, OUTPUT);
    pinMode(lightPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    
    digitalWrite(doorPin, LOW);
    digitalWrite(lightPin, LOW);
    
    Particle.function("door", toggleDoor);
    Particle.function("light", toggleLight);
    Particle.variable("distance", distanceInches);
    Particle.variable("status", status);
    Particle.variable("velocity", velocity);
}

// Get the string representation of the status of the door
String getStatus(double distance, double velocity) {
    // Door is not moving
    if (velocity > -1 && velocity < 1) {
        return distance > 115 ? "Closed" : "Open";
    } else {
        return velocity > 0 ? "Opening" : "Closing";
    }
}

// Internal loop, calculate distance regularly here to determine door state
void loop() {
    distanceInches = rangeFinder.getDistanceInch();
    
    // Calculate velocity by estimating distance traveled per half second
    if (velocityCounter == 0) {
        lastDistanceForVelocity = distanceInches;
        velocityCounter++;
    } else if (velocityCounter == 2) {
        velocity = lastDistanceForVelocity - distanceInches;
        velocityCounter = 0;
    } else {
        velocityCounter++;
    }
    status = getStatus(distanceInches, velocity);
    
    // Make a new measurement about 4 times per second
    delay(250);
}

// Endpoint to trigger a change in the door
int toggleDoor(String command) {
    // Flash light to signal door will start moving
    writeLightPin();
    delay(500);
    writeLightPin();
    delay(500);
    writeLightPin();
    delay(500);
    writeLightPin();
    // Start LED flash
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    // Trigger door
    writeDoorPin();
    // Finish flash
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    
    return runTimes++;
}

// Endpoint to trigger a change in the light
int toggleLight(String command) {
    // Start LED flash
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    // Trigger light
    writeLightPin();
    // Finish flash
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    
    return runTimes++;
}

// Directly write to the door pin to move the door
void writeDoorPin() {
    digitalWrite(doorPin, HIGH);
    delay(100);
    digitalWrite(doorPin, LOW);
}

// Directly write to the light pin to toggle the light
void writeLightPin() {
    digitalWrite(lightPin, HIGH);
    delay(100);
    digitalWrite(lightPin, LOW);
}