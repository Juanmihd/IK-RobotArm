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

    // Random destination
    random random_gen;

    int cur_tic;
    int total_tic;
  public:
    /// this is called when we construct the class before everything is initialised.
    IKRobotArm(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      //Creating the skeleton. The first thing is to link it to the scene
      debug_skeleton = new DQ_Skeleton();
      debug_skeleton->add_scene(app_scene);
      //Init the skeleton (will set the initial position to the parameter given
      debug_skeleton->init();
      //Add the root bone
      DQ_Bone* root = new DQ_Bone(0.001f);
      DQ_Bone* real_root = new DQ_Bone(3.0f);
      DQ_Bone* forearm = new DQ_Bone(2.0f);
      DQ_Bone* arm = new DQ_Bone(4.0f);
      root->constraint_in_axis(1.0f, 1.0f, 1.0f);
      real_root->constraint_in_axis(1.0f, 1.0f, 1.0f);
      forearm->constraint_in_axis(1.0f, 0, 0.0f);
      arm->constraint_in_axis(0.0f, 0, 1.0f);
      debug_skeleton->add_bone(root);
      debug_skeleton->add_bone(real_root, root);
      debug_skeleton->add_bone(forearm, real_root);
      debug_skeleton->add_bone(arm, forearm);
      skeleton_dance_once(debug_skeleton);
    }

    /// This will launch a random_dance_movenet
    void skeleton_dance_once(DQ_Skeleton *skeleton){
      total_tic = skeleton->start_animation();
      cur_tic = 0;
    }

    /// This will define the actions of the user (mouse, keyboard...)
    void user_actions(){
      if (is_key_down('A')){
        if (debug_skeleton->get_status() == _STILL)
          skeleton_dance_once(debug_skeleton);
      }
      else if (is_key_going_down('S')){
        if (debug_skeleton->get_status() == _STILL){
          vec3 position = vec3(random_gen.get(-10, 10), random_gen.get(0, 10), random_gen.get(-10, 10));
          position = position.normalize() * debug_skeleton->get_range();
          printf("Trying to get to %f, %f, %f\n", position.get()[0], position.get()[1], position.get()[2]);
          total_tic = debug_skeleton->start_animation(_RANDOM_ALG, position);
          cur_tic = 0;
        }
      }
      else if (is_key_going_down('Q')){ //Stop all animation
        debug_skeleton->finish_animation(true);
      }
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // controls the arm
      user_actions();

      //// draw the skeleton
      if (debug_skeleton->get_status() != _STILL){
        debug_skeleton->animate_skeleton(cur_tic, total_tic);
       // printf("Curr_frame: %i\n", cur_tic);
        ++cur_tic;
        if (cur_tic > total_tic){
          cur_tic = 0;
          total_tic = debug_skeleton->finish_animation();
        }
      }

      debug_skeleton->draw();

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif