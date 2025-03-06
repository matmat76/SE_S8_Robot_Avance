// robot.c

#include "robot.h"
#include "mrpiz.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int robot_start(void) {
  fprintf(stdout,"Initializing robot ...\n"); 

  /*Initialization of robot*/
  if (mrpiz_init() != 0) {
    fprintf(stderr, "Failed to initialize robot. Errno: %d, Message: %s\n", errno, strerror(errno));
    return -1;
}
  fprintf(stdout,"Robot initialized successfully\n");

  /*Reset and check the encoders*/
  robot_reset_wheel_pos();

  if(robot_get_wheel_position(LEFT_WHEEL) == -1 || robot_get_wheel_position(RIGHT_WHEEL) == -1){
    fprintf(stdout,"Error : Failed to initialize encoder\n");
    return -1; 
  }
  // @todo
  robot_set_speed(0,0);
  return 0;
}

//Setting the speed of the robot for the wheels
void robot_set_speed(speed_pct_t left, speed_pct_t right) {
  // @todo
  /*On vérifie que la valeur de speed est comprise entre -100 et 100*/
  fprintf(stdout,"Robot.c : Setting speed - Left : %d, Right : %d\n",left,right);
  if(left<-100 || left>100 || right<-100 || right>100){
    printf("Robot.c : Invalid speed values \n");
    return;
  }
  mrpiz_motor_set(MRPIZ_MOTOR_LEFT,left);
  mrpiz_motor_set(MRPIZ_MOTOR_RIGHT,right);
  fprintf(stdout,"Robot.c : Speed set successfully\n");
}

//We give the wheel's value depending on the encoder of left or right motor
/*Allow to give the encoder position with a upper machine language*/ 
int robot_get_wheel_position(wheel_t wheel_id) {
  // @todo
  int position =0; 
  switch(wheel_id){
    case LEFT_WHEEL: 
      position = mrpiz_motor_encoder_get(MRPIZ_MOTOR_LEFT);
      fprintf(stdout,"Robot.c : Left encoder position : %d\n", mrpiz_motor_encoder_get(MRPIZ_MOTOR_LEFT));
      break; 
    case RIGHT_WHEEL:
      position = mrpiz_motor_encoder_get(MRPIZ_MOTOR_RIGHT);
      fprintf(stdout,"Robot.c : Right encoder position : %d\n", mrpiz_motor_encoder_get(MRPIZ_MOTOR_RIGHT));
      break;
    default : 
      fprintf(stdout,"Robot.c : Invalid wheel ID\n");
      position = -1; 
      break; 
  }
  return position;

}

//We reset the position of the wheels 
void robot_reset_wheel_pos(void) {
  // @todo
  mrpiz_motor_encoder_reset(MRPIZ_MOTOR_LEFT);
  mrpiz_motor_encoder_reset(MRPIZ_MOTOR_RIGHT);
}

robot_status_t robot_get_status(void) {
  // @todo 
  robot_status_t robot_status; 
  robot_status.left_sensor = mrpiz_proxy_sensor_get(MRPIZ_PROXY_SENSOR_FRONT_LEFT);
  robot_status.right_sensor = mrpiz_proxy_sensor_get(MRPIZ_PROXY_SENSOR_FRONT_RIGHT);
  robot_status.center_sensor = mrpiz_proxy_sensor_get(MRPIZ_PROXY_SENSOR_FRONT_CENTER);
  robot_status.center_left_sensor = mrpiz_proxy_sensor_get(MRPIZ_PROXY_SENSOR_FRONT_CENTER_LEFT);
  robot_status.center_right_sensor= mrpiz_proxy_sensor_get(MRPIZ_PROXY_SENSOR_FRONT_CENTER_RIGHT);
  robot_status.left_encoder = mrpiz_proxy_sensor_get(MRPIZ_MOTOR_LEFT);
  robot_status.right_encoder = mrpiz_proxy_sensor_get(MRPIZ_MOTOR_RIGHT);
  robot_status.battery = mrpiz_battery_level();
  
  return robot_status;
}

void robot_signal_event(notification_t event) {
  // @todo
  switch(event){
    case ROBOT_OK: 
      mrpiz_led_rgb_set(MRPIZ_LED_GREEN); 
      break; 
    case ROBOT_OBSTACLE:
      mrpiz_led_rgb_set(MRPIZ_LED_RED); 
      break; 
    case ROBOT_PROBLEM:
      mrpiz_led_rgb_set(MRPIZ_LED_BLUE); 
      break;
    case ROBOT_OFF:
      mrpiz_led_rgb_set(MRPIZ_LED_OFF); 
      break;
  }
}

void robot_close(void) {
  robot_set_speed(0, 0);
  mrpiz_close();
}
