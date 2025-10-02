#include <Servo.h>

Servo leftLeg;
Servo rightLeg;
Servo liftServo;

// Timing
unsigned long lastUpdate = 0;
const int interval = 20;  // Update interval in ms

// Walking state
int swingAngle = 60;
int swingStep = 1;

// Lifting state
int currentLiftAngle = 90;
int liftTargetAngle = 90;
int liftStep = 1;
bool liftTargetSet = false;

bool liftingLeft = true;    // Alternates each cycle
int liftPhase = 0;          // 0 = lift foot, 1 = swing, 2 = lower foot

void setup() {
  leftLeg.attach(9);        // Left leg servo pin
  rightLeg.attach(10);      // Right leg servo pin
  liftServo.attach(11);     // Lifter servo pin

  leftLeg.write(swingAngle);
  rightLeg.write(180 - swingAngle);  // Mirror motion
  liftServo.write(currentLiftAngle);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= interval) {
    lastUpdate = currentTime;

    // === SMOOTH LIFTING LOGIC ===
    if (currentLiftAngle != liftTargetAngle) {
      if (abs(currentLiftAngle - liftTargetAngle) <= liftStep) {
        currentLiftAngle = liftTargetAngle;
      } else if (currentLiftAngle < liftTargetAngle) {
        currentLiftAngle += liftStep;
      } else if (currentLiftAngle > liftTargetAngle) {
        currentLiftAngle -= liftStep;
      }

      liftServo.write(currentLiftAngle);
    }

    // === MAIN GAIT STATE MACHINE ===
    switch (liftPhase) {
      case 0: // 🦶 LIFT ONE FOOT
        if (!liftTargetSet) {
          liftTargetAngle = liftingLeft ? 0 : 40;  // 0° = lift left, 40° = lift right
          liftTargetSet = true;
        }
        if (currentLiftAngle == liftTargetAngle) {
          liftPhase = 1;
          liftTargetSet = false;
        }
        break;

      case 1: // ↔️ SWING BOTH LEGS
        swingAngle += swingStep;

        // Clamp swing angle to 60–120 range
        if (swingAngle > 120) swingAngle = 120;
        if (swingAngle < 60) swingAngle = 60;

        leftLeg.write(swingAngle);
        rightLeg.write(180 - swingAngle);  // Mirrored mount

        // Switch to lowering when end of swing is reached
        if (swingAngle == 120 || swingAngle == 60) {
          swingStep = -swingStep;
          liftPhase = 2;
          liftTargetSet = false;
        }
        break;

      case 2: // 🔽 LOWER FOOT
        if (!liftTargetSet) {
          liftTargetAngle = 90;  // Neutral = both feet down
          liftTargetSet = true;
        }
        if (currentLiftAngle == liftTargetAngle) {
          liftingLeft = !liftingLeft;  // Alternate next step
          liftPhase = 0;
          liftTargetSet = false;
        }
        break;
    }
  }
}
