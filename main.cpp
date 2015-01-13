////////////////////////////////////////////////////////////////////////////////
//
/// @author Sam & Juanmi
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// This is the Main for the project of the Robotic Arm
//

#define OCTET_BULLET 1

#include "octet.h"
#include "IKRobotArm.h"

/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::IKRobotArm app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}


