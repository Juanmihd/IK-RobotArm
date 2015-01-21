////////////////////////////////////////////////////////////////////////////////
//
/// @author Sam & Juanmi
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#ifndef IKROBOTARM_H_DEFINED
#define IKROBOTARM_H_DEFINED

#include "DQ_Skeleton.h"
#include "DQ_Clickable_Object.h"

namespace octet {
  /// Scene containing a box with octet.
  class IKRobotArm : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;

    // for the raycasting
    btDiscreteDynamicsWorld *world;
    camera_instance* cam;

    // skeleton declaration
    DQ_Skeleton* debug_skeleton;
    bool dancing_skeleton;
    bool moving_skeleton;

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
      app_scene = new visual_scene();
      app_scene->create_default_camera_and_lights();
      cam = app_scene->get_camera_instance(0);
      cam->get_scene_node()->access_nodeToParent();
      world = app_scene->get_bt_world();
      assert(world);

      // SH: create debug shape to test object picker (should be removed eventually) 
      mat4t mat;
      mat4t transform_floor;
      transform_floor.translate(vec3(0, -2, 0));
      transform_floor.rotate(90, 1, 0, 0);
      material* purple = new material(vec4(0.5f, 0.2f, 0.8f, 1));
      material* green = new material(vec4(0.6f, 0.81f, 0.2f, 1));
      mat.translate(-3, 6, 0);
      app_scene->add_shape(mat, new mesh_sphere(vec3(2, 2, 2), 2), purple, true);
      app_scene->add_shape(transform_floor, new mesh_box(vec3(10, 10, 0.5f)), green, false);
      // end of code that should be delete eventually


      //Creating the skeleton. The first thing is to link it to the scene
      debug_skeleton = new DQ_Skeleton();
      debug_skeleton->add_scene(app_scene);
      //Init the skeleton (will set the initial position to the parameter given
      debug_skeleton->init(vec3(2, 5, 0));
      dancing_skeleton = false;
      //Add the root bone
      DQ_Bone* root = new DQ_Bone(0.0001f);
      DQ_Bone* real_root = new DQ_Bone(2.5f);
      DQ_Bone* forearm = new DQ_Bone(1.5f);
      DQ_Bone* second_forearm = new DQ_Bone(1.5f);
      DQ_Bone* arm = new DQ_Bone(4.0f);
      //root->constraint_in_axis(1.0f, 1.0f, 1.0f);
      //root->constraint_in_angle(10, -10);
      //real_root->constraint_in_axis(1.0f, 1.0f, 1.0f);
      //real_root->constraint_in_angle(10, -10);
      real_root->constraint_in_axis(0.0f, 1.0f, 0.0f);
      //forearm->constraint_in_angle(10, -10);
      forearm->constraint_in_axis(0.0f, 0.0f, 1.0f);
      //arm->constraint_in_angle(10, -10);
      debug_skeleton->add_bone(root);
      debug_skeleton->add_bone(real_root, root);
      debug_skeleton->add_bone(forearm, real_root);
      debug_skeleton->add_bone(second_forearm, forearm);
      debug_skeleton->add_bone(arm, second_forearm);
      skeleton_dance_once(debug_skeleton);//Creating the skeleton. The first thing is to link it to the scene
      
    }

    /// This will launch a random_dance_movenet
    void skeleton_dance_once(DQ_Skeleton *skeleton){
      total_tic = skeleton->start_animation();
      cur_tic = 0;
    }

    /// This will define the actions of the user (mouse, keyboard...)
    void user_actions(){
      if (is_key_down('A')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL)
          skeleton_dance_once(debug_skeleton);
      }
      else if (is_key_going_down('D')){
        dancing_skeleton = !dancing_skeleton;
      }
      else if (is_key_down(key_up)){
        debug_skeleton->init(vec3(0.0f, 5.0f, 5.5f));
      }
      else if (is_key_down(key_down)){
        debug_skeleton->init(vec3(0.0f, 5.0f, -5.5f));
      }
      else if (is_key_down(key_left)){
        debug_skeleton->init(vec3(-5.5f, 5.0f, 0.0f));
      }
      else if (is_key_down(key_right)){
        debug_skeleton->init(vec3(5.5f, 5.0f, 0.0f));
      }
      else if (is_key_going_down('S')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          vec3 position = vec3(random_gen.get(-10, 10), random_gen.get(0, 10), random_gen.get(-10, 10));
          position = position.normalize() * debug_skeleton->get_range();
          printf("Trying to get to %f, %f, %f\n", position.get()[0], position.get()[1], position.get()[2]);
          total_tic = debug_skeleton->start_animation(_RANDOM_ALG, position);
          cur_tic = 0;
        }
      }
      else if (is_key_going_down('R')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          vec3 position = vec3(0.0f, 1.0f, 0.0f);
          position = position.normalize() * debug_skeleton->get_range();
          position.get()[1] += 5;
          total_tic = debug_skeleton->start_animation(_RANDOM_ALG, position);
          cur_tic = 0;
        }
      }
      else if (is_key_going_down(key_lmb)){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          btVector3 start, end;
          cast_ray_from_mouse(start, end);
          btCollisionWorld::ClosestRayResultCallback rayCallBack(start, end);
          world->rayTest(start, end, rayCallBack);
          if (rayCallBack.hasHit()){
            vec3 pos = get_vec3(rayCallBack.m_hitPointWorld);
            printf("ray cast world pos: x: %f y: %f z: %f\n", rayCallBack.m_hitPointWorld.x(), rayCallBack.m_hitPointWorld.y(), rayCallBack.m_hitPointWorld.z());
            total_tic = debug_skeleton->start_animation(_RANDOM_ALG, pos);
            cur_tic = 0;
          }
        }
      }
      else if (is_key_going_down('Q')){ //Stop all animation
        debug_skeleton->finish_animation(true);
      }
    }

    /// This function handles the raycasting of the 
    void cast_ray_from_mouse(btVector3 &bt_start, btVector3 &bt_end){
        int mx = 0, my = 0;
        int vx = 0, vy = 0;
        this->get_mouse_pos(mx, my);
        this->get_viewport_size(vx, vy);
        float x = (float)(mx - vx / 2) *2.0f / vx;
        float y = (float)(vy / 2 - my) *2.0f / vy;
        cam = app_scene->get_camera_instance(0);
        ray _ray = cam->get_ray(x, y);
        
        vec4 start = vec4(_ray.get_start(), 1);
        vec4 end = vec4(_ray.get_end(), 1);

        bt_start = get_btVector3(start.xyz());
        bt_end = get_btVector3(end.xyz());
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
      btVector3 start, end;

      // draw the skeleton
      debug_skeleton->draw();
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
      else{
        if (dancing_skeleton){
          skeleton_dance_once(debug_skeleton);
        }
      }

      debug_skeleton->draw();

      // raycasting
      //if (is_key_going_down(key_lmb)){
      //  cast_ray_from_mouse(start, end);
      //  btCollisionWorld::ClosestRayResultCallback rayCallBack(start, end);
      //  world->rayTest(start, end, rayCallBack);
      //  if (rayCallBack.hasHit()){
      //    printf("ray cast world pos: x: %f y: %f z: %f\n", rayCallBack.m_hitPointWorld.x(), rayCallBack.m_hitPointWorld.y(), rayCallBack.m_hitPointWorld.z());
      //  }
      //}
      //// update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif