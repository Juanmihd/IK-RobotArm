////////////////////////////////////////////////////////////////////////////////
//
/// @author Sam & Juanmi
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#ifndef IKROBOTARM_H_DEFINED
#define IKROBOTARM_H_DEFINED

#include "DQ_Skeleton.h"
#include "DQ_Sphere.h"

namespace octet {
  /// Scene containing a box with octet.
  class IKRobotArm : public app {
    enum {_NUM_SPHERES = 8};
    // scene for drawing box
    ref<visual_scene> app_scene;
    ref<scene_node> camera_node;
    DQ_Sphere* test_sphere;
    DQ_Sphere* test_sphere2;
    dynarray<ref<DQ_Sphere>> spheres;

    // for the raycasting
    btDiscreteDynamicsWorld *world;
    camera_instance* cam;
    vec3 dest_position;

    // skeleton declaration
    DQ_Skeleton* debug_skeleton;
    dynarray<ref<DQ_Skeleton>> arms;
    bool dancing_skeleton;
    bool moving_skeleton;

    // Random destination
    random random_gen;

    int xOfCam, yOfCam, zOfCam;
    int cur_tic;
    int total_tic;
  public:
    /// this is called when we construct the class before everything is initialised.
    IKRobotArm(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      random_gen.set_seed(time(NULL));
      app_scene = new visual_scene();
      app_scene->create_default_camera_and_lights();
      cam = app_scene->get_camera_instance(0);
      camera_node = app_scene->get_camera_instance(0)->get_node();
      cam->get_node()->access_nodeToParent()[3] = vec4(0, 0, 0, 1);
      cam->get_node()->access_nodeToParent().translate(vec3(0, 50, 0));
      cam->get_node()->access_nodeToParent().rotateX(-90);
      world = app_scene->get_bt_world();

      // SH: Table construction 
      mat4t transform;
      mat4t transform_wall_top;
      transform.translate(vec3(0, -2, 0));
      transform.rotate(90, 1, 0, 0);
      material* green = new material(vec4(0.6f, 0.81f, 0.2f, 1));
      material* orange = new material(vec4(0.81f, 0.3f, 0.2f, 1));
      material* blue = new material(vec4(0, 0.5f, 1.0f, 1.0f));
      app_scene->add_shape(transform, new mesh_box(vec3(16, 10, 0.5f)), green, false);
      transform.loadIdentity();
      transform.translate(-18, -2, 2);
      app_scene->add_shape(transform, new mesh_box(vec3(2, 0.5f, 8)), orange, false);
      transform.loadIdentity();
      transform.translate(18, -2, -2);
      app_scene->add_shape(transform, new mesh_box(vec3(2, 0.5f, 8)), orange, false);
      transform.loadIdentity();
      transform.translate(0, 0, -11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 2, 1)), blue, false);
      transform.loadIdentity();
      transform.translate(0, 0, 11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 2, 1)), blue, false);
      transform.loadIdentity();
      transform.translate(21, 0, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 12)), blue, false);
      transform.loadIdentity();
      transform.translate(-21, 0, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 12)), blue, false);
      // end of table construction

      // Adding spheres
      for (size_t i = 0; i != _NUM_SPHERES; ++i){
        DQ_Sphere* new_sphere = new DQ_Sphere();
        //generate a random position for the ball
        vec3 position = vec3(random_gen.get(-15.0f,15.0f),
                              2.0f * i,
                              random_gen.get(-10.0f, 10.0f)
                              );
        //obtain a random number between 0 and 1, and pic
        float magnetic_force = (random_gen.get(-1.0f, 1.0f)) < 0.0f ? 40.0f : -40.0f;
        //obtain 
        float radius = random_gen.get(0.6f, 1.4f);
        new_sphere->init(app_scene, position, radius, magnetic_force);
        spheres.push_back(new_sphere);
      }
      // End spheres

      //Creating the skeleton. The first thing is to link it to the scene
      debug_skeleton = new DQ_Skeleton();
      debug_skeleton->add_scene(app_scene);
      //Init the skeleton (will set the initial position to the parameter given
      debug_skeleton->init(vec3(0, 0, 0));
      dancing_skeleton = false;
      //Add the root bone
      DQ_Bone* root = new DQ_Bone(0.0001f);
      DQ_Bone* real_root = new DQ_Bone(2.5f);
      DQ_Bone* forearm = new DQ_Bone(1.5f);
      DQ_Bone* second_forearm = new DQ_Bone(1.5f);
      DQ_Bone* arm = new DQ_Bone(4.0f);
      real_root->constraint_in_axis(0.0f, 1.0f, 0.0f);
      forearm->constraint_in_axis(0.0f, 0.0f, 1.0f);
      arm->constraint_in_axis(0.0f, 1.0f, 0.0f);
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
      //Move Camera Left
      if (is_key_down('1')){
        xOfCam = -1;
        yOfCam = 0;
        zOfCam = 0;
        app_scene->get_camera_instance(0)->get_node()->translate(vec3(xOfCam, yOfCam, zOfCam));
      }
      //Move Camera Down
      else if (is_key_down('2')){
        xOfCam = 0;
        yOfCam = -1;
        zOfCam = 0;
        app_scene->get_camera_instance(0)->get_node()->translate(vec3(xOfCam, yOfCam, zOfCam));
      }
      else if (is_key_down('3')){
        xOfCam = 0;
        yOfCam = 0;
        zOfCam = -1;
      }
      else if (is_key_down('4')){
        camera_node->remove_parent();
      }
      else if (is_key_going_down('5')){
        debug_skeleton->get_wrist_node()->obtain_joints().bone_node->add_child(camera_node);

      }
      if (is_key_down('A')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL)
          skeleton_dance_once(debug_skeleton);
      }
      else if (is_key_going_down('D')){
        dancing_skeleton = !dancing_skeleton;
      }
      else if (is_key_down(key_ctrl) && is_key_down(key_up)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->translate(vec3(0.0f, 0.0f, -0.5f));
      }
      else if (is_key_down(key_ctrl) && is_key_down(key_down)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->translate(vec3(0.0f, 0.0f, 0.5f));
      }
      else if (is_key_down(key_ctrl) && is_key_down(key_left)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->translate(vec3(-0.5f, 0.0f, 0.0f));
      }
      else if (is_key_down(key_ctrl) && is_key_down(key_right)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->translate(vec3(0.5f, 0.0f, 0.0f));
      }
      else if (is_key_down(key_up)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(0.0f, 5.0f, -10.0f));
      }
      else if (is_key_down(key_down)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(0.0f, 5.0f, 10.0f));
      }
      else if (is_key_down(key_left)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(-20.0f, 5.0f, 0.0f));
      }
      else if (is_key_down(key_right)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(20.0f, 5.0f, 0.0f));
      }
      else if (is_key_going_down('S')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          dest_position = vec3(random_gen.get(-10, 10), random_gen.get(0, 10), random_gen.get(-10, 10));
          dest_position = dest_position.normalize() * debug_skeleton->get_range();
          printf("Trying to get to %f, %f, %f\n", dest_position.get()[0], dest_position.get()[1], dest_position.get()[2]);
          total_tic = debug_skeleton->start_animation(_RANDOM_ALG, dest_position);
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
            vec3 increment = vec3(0.0f, 2.0f, 0.0f);
            printf("ray cast world pos: x: %f y: %f z: %f\n", rayCallBack.m_hitPointWorld.x(), rayCallBack.m_hitPointWorld.y(), rayCallBack.m_hitPointWorld.z());
            total_tic = debug_skeleton->start_animation(_RANDOM_ALG, pos + increment);
            cur_tic = 0;
          }
        }
      }
      else if (is_key_going_down('Q')){ //Stop all animation
        debug_skeleton->finish_animation(true);
      }

      if (is_key_going_down(' ')){
        printf("force is being called!\n");
        vec3 wrist_pos = debug_skeleton->get_wrist_node()->get_world_position_bone();
        for (size_t i = 0; i != _NUM_SPHERES; ++i){
          spheres[i]->resolve_magnetic_force(wrist_pos);
        }
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

      //// update matrices. assume 30 fps.
      app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif