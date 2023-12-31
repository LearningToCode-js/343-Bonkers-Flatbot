#include "main.h"
#include "drivercontrol.hpp"
#include "devices.hpp"
#include "autons.hpp"
// #include "EZ-Template/api.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::print_ez_template();
  
  pros::delay(500); // Stop the user from doing anything while legacy ports configure.

  // Configure your chassis controls
  chassis.toggle_modify_curve_with_controller(true); // Enables modifying the controller curve with buttons on the joysticks
  chassis.set_active_brake(0); // Sets the active brake kP. We recommend 0.1.
  chassis.set_curve_default(4, 0); // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)  
  default_constants(); // Set the drive to your own constants from autons.cpp!
  modified_exit_condition();
  
  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.set_left_curve_buttons (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If using tank, only the left side is used. 
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.add_autons({
    Auton("offense", offense),     
    Auton("AMONG IS SO SUS", troll),
    Auton("safe defense", defensesafe),
    
    Auton("defense full", defense),
    Auton("defense qual", defenseQual),
    Auton("Nothing", doNothing)
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();
}



/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}



/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}



/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  chassis.reset_pid_targets(); // Resets PID targets to 0
  chassis.reset_gyro(); // Reset gyro position to 0
  chassis.reset_drive_sensor(); // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.
  setStickToCoast();
  ez::as::auton_selector.call_selected_auton(); // Calls selected auton from autonomous selector.
}



/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  // This is preference to what you like to drive on.
  chassis.set_drive_brake(MOTOR_BRAKE_COAST);
  setStickToHold();
  while (true) {

    chassis.tank(); // Tank control
    // chassis.arcade_standard(ez::SPLIT); // Standard split arcade
    // chassis.arcade_standard(ez::SINGLE); // Standard single arcade
    // chassis.arcade_flipped(ez::SPLIT); // Flipped split arcade
    // chassis.arcade_flipped(ez::SINGLE); // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
            toggleIntake();
        } 
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
            toggleWing();
        }
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
          // park.set_value(true);

          ratchetOut = !ratchetOut;
          ratchet.set_value(ratchetOut);
          pros::delay(250);
        }
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) {
            isFlywheelForward = !isFlywheelForward;
            if (isFlywheelForward) {
                flywheelForward();
            } else {
                flywheelBackward();
            }
            pros::delay(250);
        } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
            flywheelReset();
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
          stickCoast = !stickCoast;
          if (stickCoast) {
            setStickToCoast();
          } else {
            setStickToHold();
          }
          pros::delay(200);
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            stickUp();
        } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            stickDown();
        } else {
            stickReset();
        }
    pros::delay(ez::util::DELAY_TIME); // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
