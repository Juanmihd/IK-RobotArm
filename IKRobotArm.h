////////////////////////////////////////////////////////////////////////////////
//
/// @author Sam & Juanmi
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#ifndef IKROBOTARM_H_DEFINED
#define IKROBOTARM_H_DEFINED

#include "DQ_Skeleton.h"

namespace octet {
  /// Scene containing a box with octet.
  class IKRobotArm : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;

    // skeleton declaration
    DQ_Skeleton* debug_skeleton;
  public:
    /// this is called when we construct the class before everything is initialised.
    IKRobotArm(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      // initialise the object picker
      object_picker pickobject

      //Creating the skeleton. The first thing is to link it to the scene
      debug_skeleton = new DQ_Skeleton();
      debug_skeleton->add_scene(app_scene);
      //Init the skeleton (will set the initial position to the parameter given
      debug_skeleton->init();
      //Add the root bone
      DQ_Bone* root = new DQ_Bone(4.0f);
      DQ_Bone* forearm = new DQ_Bone(2.0f);
      DQ_Bone* arm = new DQ_Bone(1.0f);
      debug_skeleton->add_bone(root);
      debug_skeleton->add_bone(forearm, root);
      debug_skeleton->add_bone(arm, forearm);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // draw the skeleton
      debug_skeleton->draw();

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif