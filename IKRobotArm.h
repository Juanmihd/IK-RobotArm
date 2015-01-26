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
    ref<scene_node> camera_tree_node;
    DQ_Sphere* test_sphere;
    DQ_Sphere* test_sphere2;
    dynarray<ref<DQ_Sphere>> spheres;
    dynarray<ref<material>> table_mat;
    dynarray<ref<mesh_instance>> table_instances; 
    int index_mat;

    // for the raycasting
    btDiscreteDynamicsWorld *world;
    camera_instance* cam;
    mat4t starting_cam;
    vec3 dest_position;

    // skeleton declaration
    DQ_Skeleton* debug_skeleton;
    dynarray<ref<DQ_Skeleton>> arms;
    float top_bottom;
    float left_right;
    bool dancing_skeleton;
    bool moving_skeleton;
    bool moving_up_skeleton;
    int ball_grabbed;
    int magnetism;

    // Random destination
    random random_gen;
    mat4t old_cam_position;
    bool arm_fixed_cam;

    int cur_tic;
    int total_tic;
  public:
    /// this is called when we construct the class before everything is initialised.
    IKRobotArm(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      moving_up_skeleton = false;
      ball_grabbed = -1; index_mat = 0;
      arm_fixed_cam = false;
      top_bottom = -9.5f;
      left_right = 0.0f;
      magnetism = 1;
      random_gen.set_seed(time(NULL));
      app_scene = new visual_scene();
      camera_tree_node = new scene_node();
      app_scene->create_default_camera_and_lights();
      cam = app_scene->get_camera_instance(0);
      camera_node = app_scene->get_camera_instance(0)->get_node();
      camera_node->translate(vec3(0.0f, 25.0f, 0.0f));
      camera_node->rotate(-40.0f, vec3(1.0f, 0.0f, 0.0f));
      camera_tree_node->translate(vec3(0.0f, 0.0f, 30.0f));
      camera_tree_node->add_child(camera_node);
      
      world = app_scene->get_bt_world();

      // SH: Table construction 
      mat4t transform;
      mat4t transform_wall_top;
      transform.translate(vec3(0, -2, 0));
      transform.rotate(90, 1, 0, 0);
      image* jupiter = new image("assets/electricfield.gif");
      image* jupiter_mask = new image("assets/electricfieldmask.gif");
      image* jupiter2 = new image("assets/electricfield2.gif");
      image* jupiter_mask2 = new image("assets/electricfield2mask.gif");
      table_mat.resize(2);
      table_instances.resize(5);
      param_shader *shader = new param_shader("shaders/default.vs", "shaders/multitexture.fs");
      table_mat[0] = new material(vec4(1, 1, 1, 1), shader);
      table_mat[1] = new material(vec4(1, 1, 1, 1), shader);
      table_mat[0]->add_sampler(0, app_utils::get_atom("jupiter"), jupiter, new sampler());
      table_mat[0]->add_sampler(1, app_utils::get_atom("jupiter_mask"), jupiter_mask, new sampler()); //new material(vec4(0.6f, 0.81f, 0.2f, 1));
      table_mat[1]->add_sampler(0, app_utils::get_atom("jupiter2"), jupiter2, new sampler());
      table_mat[1]->add_sampler(1, app_utils::get_atom("jupiter_mask2"), jupiter_mask2, new sampler()); //new material(vec4(0.6f, 0.81f, 0.2f, 1));
      material* orange = new material(vec4(0.81f, 0.3f, 0.2f, 1));
      material* purple = new material(vec4(0.5f, 0.0f, 0.5f, 1));
      material* cold_red = new material(vec4(0.2f, 0.2f, 0.2f, 1));

      image* walls = new image("assets/Walls.gif");
      image* walls_mask = new image("assets/Walls.gif"); table_mat[1]->add_sampler(0, app_utils::get_atom("jupiter2"), jupiter2, new sampler());
      param_shader *shader2 = new param_shader("shaders/default.vs", "shaders/multitexture.fs"); 
      material* transparent = new material(vec4(1, 1, 1, 1), shader2);
      transparent->add_sampler(0, app_utils::get_atom("walls"), walls, new sampler());
      transparent->add_sampler(1, app_utils::get_atom("walls_mask"), walls_mask, new sampler());
      
      app_scene->add_shape(transform, new mesh_box(vec3(21, 10, 0.5f)), table_mat[0], false);
      table_instances[0] = app_scene->get_mesh_instance(0);
      transform.loadIdentity();
      transform.translate(0, 0, -11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 5, 1)), transparent, false);
      transform.loadIdentity();
      transform.translate(0, 0, 11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 5, 1)), transparent, false);
      transform.loadIdentity();
      transform.translate(-21, -2, -8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), transparent, false);
      transform.loadIdentity();
      transform.translate(-21, -2, 8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), transparent, false);
      transform.loadIdentity();
      transform.translate(21, -2, 8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), transparent, false);
      transform.loadIdentity();
      transform.translate(21, -2, -8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), transparent, false);
      transform.loadIdentity();
      transform.translate(0, -2, -11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 2, 1)), purple, false);
      transform.translate(0, 6.75, 1.5);
      app_scene->add_shape(transform, new mesh_box(vec3(22, 0.25, 0.25)), cold_red, false);
      transform.loadIdentity();
      transform.translate(0, -2, 11);
      app_scene->add_shape(transform, new mesh_box(vec3(20, 2, 1)), purple, false);
      transform.translate(0, 6.75, -1.5);
      app_scene->add_shape(transform, new mesh_box(vec3(22, 0.25, 0.25)), cold_red, false);
      transform.loadIdentity();
      transform.translate(21, -2, 8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), purple, false);
      transform.loadIdentity();
      transform.translate(21, -2, -8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), purple, false);
      transform.loadIdentity();
      transform.translate(21, -2, 0);
      transform.translate(-2, 6.75, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(0.25, 0.25, 12)), cold_red, false);
      transform.loadIdentity();
      transform.translate(-21, -2, -8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), purple, false);
      transform.loadIdentity();
      transform.translate(-21, -2, 8);
      app_scene->add_shape(transform, new mesh_box(vec3(1, 2, 6)), purple, false);
      transform.loadIdentity();
      transform.translate(-21, -2, 0);
      transform.translate(2, 6.75, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(0.25, 0.25, 12)), cold_red, false);
      transform.loadIdentity();
      transform.translate(-21, 3, 11);
      app_scene->add_shape(transform, new mesh_box(vec3(0.5, 6, 0.5)), cold_red, false);
      transform.translate(0, 1, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(2.5, 0.25, 2.5)), cold_red, false);
      transform.loadIdentity();
      transform.translate(21, 3, 11);
      app_scene->add_shape(transform, new mesh_box(vec3(0.5, 6, 0.5)), cold_red, false);
      transform.translate(0, 1, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(2.5, 0.25, 2.5)), cold_red, false);
      transform.loadIdentity();
      transform.translate(-21, 3, -11);
      app_scene->add_shape(transform, new mesh_box(vec3(0.5, 6, 0.5)), cold_red, false);
      transform.translate(0, 1, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(2.5, 0.25, 2.5)), cold_red, false);
      transform.loadIdentity();
      transform.translate(21, 3, -11);
      app_scene->add_shape(transform, new mesh_box(vec3(0.5, 6, 0.5)), cold_red, false);
      transform.translate(0, 1, 0);
      app_scene->add_shape(transform, new mesh_box(vec3(2.5, 0.25, 2.5)), cold_red, false);
      // end of table construction

      // Adding spheres
      for (size_t i = 0; i != _NUM_SPHERES; ++i){
        DQ_Sphere* new_sphere = new DQ_Sphere();
        //generate a random position for the ball
        vec3 position = vec3(random_gen.get(-15.0f, 15.0f),
                             random_gen.get(2.0f, 7.0f) * i + 7.0f,
                             random_gen.get(-8.0f, 8.0f)
                              );
        //obtain a random number between 0 and 1, and pic
        float magnetic_force = (random_gen.get(-1.0f, 1.0f)) < 0.0f ? 40.0f : -40.0f;
        //obtain 
        float radius = random_gen.get(0.6f, 1.4f);
        new_sphere->init(app_scene, position, radius, magnetic_force);
        spheres.push_back(new_sphere);
        position.get()[0] = position.get()[0] * random_gen.get(0.0f, 0.2f);
        position.get()[1] = 0;
        position.get()[2] = position.get()[2] * random_gen.get(0.0f, 0.2f);
        btVector3 temp = get_btVector3(position * -0.5f);
        spheres[i]->get_rigidbody()->applyCentralImpulse(temp);
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
      debug_skeleton->add_bone(root);
      debug_skeleton->add_bone(real_root, root);
      debug_skeleton->add_bone(forearm, real_root);
      debug_skeleton->add_bone(second_forearm, forearm);
      debug_skeleton->add_bone(arm, second_forearm);   
      skeleton_dance_once(debug_skeleton);//Creating the skeleton. The first thing is to link it to the scene

      debug_skeleton->init(vec3(0.0f, 5.0f, -9.5f));
    }

    // @brief This function launchs the moving up movement
    void move_up_skeleton(){
      if (!moving_up_skeleton){
        moving_up_skeleton = true;
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          vec3 position = vec3(0.0f, 1.0f, 0.0f);
          position = position.normalize() * debug_skeleton->get_range();
          position.get()[1] += 5;
          total_tic = debug_skeleton->start_animation(_RANDOM_ALG, position);
          cur_tic = 0;
        }
      }
    }

    // Reseting the game (rethrows the balls from the top)
    void reset_game(){
      for (size_t i = 0; i != _NUM_SPHERES; ++i){
        //generate a random position for the ball
        vec3 position = vec3(random_gen.get(-15.0f, 15.0f),
          random_gen.get(2.0f, 7.0f) * i + 7.0f,
          random_gen.get(-8.0f, 8.0f)
          );
        spheres[i]->get_rigidbody()->clearForces();
        spheres[i]->get_rigidbody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        spheres[i]->get_rigidbody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        btTransform reset_transform = btTransform(get_btMatrix3x3(mat4t(1)), get_btVector3(position));
        spheres[i]->get_rigidbody()->setWorldTransform(reset_transform);
        position.get()[0] = position.get()[0] * random_gen.get(0.0f, 0.2f);
        position.get()[1] = 0;
        position.get()[2] = position.get()[2] * random_gen.get(0.0f, 0.2f);
        btVector3 temp = get_btVector3(position * -0.5f);
        spheres[i]->get_rigidbody()->applyCentralImpulse(temp);
      }
    }

    /// This will launch a random_dance_movenet
    void skeleton_dance_once(DQ_Skeleton *skeleton){
      total_tic = skeleton->start_animation();
      cur_tic = 0;
    }

    void rotate_camera(float angle){
      camera_tree_node->translate(vec3(0.0f, 0.0f, -30.0f));
      camera_tree_node->rotate(angle, vec3(0.0f, 1.0f, 0.0f));
      camera_tree_node->translate(vec3(0.0f, 0.0f, 30.0f));
    }

    /// This will define the actions of the user (mouse, keyboard...)
    void user_actions(){
      //Move Camera Left
      if (is_key_down('1')){
        if (!arm_fixed_cam)
          rotate_camera(-2.0f);
      }
      //Move Camera Down
      else if (is_key_down('2')){
        if (!arm_fixed_cam)
          rotate_camera(2.0f);
      }
      else if (is_key_going_down('3')){
        if (arm_fixed_cam){
          arm_fixed_cam = false;
          camera_tree_node->access_nodeToParent().loadIdentity();
          camera_tree_node->translate(vec3(0.0f, 0.0f, 30.0f));
          camera_node->access_nodeToParent().loadIdentity();
          camera_node->translate(vec3(0.0f, 25.0f, 0.0f));
          camera_node->rotate(-40.0f, vec3(1.0f, 0.0f, 0.0f));
        }
        else{
          arm_fixed_cam = true;
          camera_tree_node->access_nodeToParent().loadIdentity();
          camera_tree_node->translate(vec3(0.0f, 50.0f, 0.0f));
          camera_node->access_nodeToParent().loadIdentity();
          camera_node->rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
        }
      }
      if (is_key_down('A')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL)
          skeleton_dance_once(debug_skeleton);
      }
      else if (is_key_going_down('D')){
        dancing_skeleton = !dancing_skeleton;
      }
      else if (is_key_down(key_ctrl) && is_key_going_down(key_up)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(0.0f, 5.0f, -9.5f));
        left_right = 0.0f;
        top_bottom = -9.5f;
      }
      else if (is_key_down(key_ctrl) && is_key_going_down(key_down)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(0.0f, 5.0f, 9.5f));
        left_right = 0.0f;
        top_bottom = 9.5f;
      }
      else if (is_key_down(key_ctrl) && is_key_going_down(key_left)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(-19.0f, 5.0f, 0.0f));
        left_right = -19.0f;
        top_bottom = 0.0f;
      }
      else if (is_key_down(key_ctrl) && is_key_going_down(key_right)){
        debug_skeleton->finish_animation(true);
        debug_skeleton->init(vec3(19.0f, 5.0f, 0.0f));
        left_right = 19.0f;
        top_bottom = 0.0f;
      }
      else if (is_key_down(key_up)){
        move_up_skeleton();
        if (left_right == -19.0f || left_right == 19.0f){
          if (top_bottom > -9.5f){
            debug_skeleton->translate(vec3(0.0f, 0.0f, -0.5f));
            top_bottom -= 0.5f;
          }
        }
      }
      else if (is_key_down(key_down)){
        move_up_skeleton();
        if (left_right == -19.0f || left_right == 19.0f){
          if (top_bottom < 9.5f){
            debug_skeleton->translate(vec3(0.0f, 0.0f, 0.5f));
            top_bottom += 0.5f;
          }
        }
      }
      else if (is_key_down(key_left)){
        move_up_skeleton();
        if (top_bottom == -9.5f || top_bottom == 9.5f){
          if (left_right > -19.0f){
            debug_skeleton->translate(vec3(-0.5f, 0.0f, 0.0f));
            left_right -= 0.5f;
          }
        }
      }
      else if (is_key_down(key_right)){
        move_up_skeleton();
        if (top_bottom == -9.5f || top_bottom == 9.5f){
          if (left_right < 19.0f){
            debug_skeleton->translate(vec3(0.5f, 0.0f, 0.0f));
            left_right += 0.5f;
          }
        }
      }
      else if (is_key_going_down('S')){
        debug_skeleton->finish_animation(true);
        if (debug_skeleton->get_status() == _STILL){
          dest_position = vec3(random_gen.get(-10, 10), random_gen.get(0, 10), random_gen.get(-10, 10));
          dest_position = dest_position.normalize() * debug_skeleton->get_range();
          //printf("Trying to get to %f, %f, %f\n", dest_position.get()[0], dest_position.get()[1], dest_position.get()[2]);
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
      else if (is_key_going_down('O')){
        reset_game();
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
            vec3 increment = vec3(0.0f, 1.0f, 0.0f);
            //printf("ray cast world pos: x: %f y: %f z: %f\n", rayCallBack.m_hitPointWorld.x(), rayCallBack.m_hitPointWorld.y(), rayCallBack.m_hitPointWorld.z());
            total_tic = debug_skeleton->start_animation(_RANDOM_ALG, pos + increment);
            cur_tic = 0;
          }
        }
      }
      else if (is_key_going_down('Q')){ //Stop all animation
        debug_skeleton->finish_animation(true);
      }

      if (is_key_going_down(' ')){ //Magnetise!
        //printf("force is being called!\n");
        vec3 wrist_pos = debug_skeleton->get_wrist_node()->get_world_position_bone();
        for (size_t i = 0; i != _NUM_SPHERES; ++i){
          if (i != ball_grabbed)
            spheres[i]->resolve_magnetic_force(wrist_pos);
        }
      }
      /*
      if (is_key_going_down('G')){ //grab the closest ball if is closer than x
        if (ball_grabbed == -1){
          bool found_ball = false;
          vec3 wrist_pos = debug_skeleton->get_wrist_node()->get_world_position_bone();
          for (size_t i = 0; i != _NUM_SPHERES && !found_ball; ++i){
            //Check distance
            vec3 v_distance = spheres[i]->get_node()->get_nodeToParent()[3].xyz() - wrist_pos;
            float distance = v_distance.length();
            //if closer than x, grab it, and out
            if (distance < 3.0f){
              found_ball = true;
              ball_grabbed = i;
              spheres[ball_grabbed]->get_node()->access_nodeToParent() = mat4t(1);
              debug_skeleton->get_wrist_node()->obtain_joints().bone_node->add_child(spheres[ball_grabbed]->get_node());
            }
          }
        }
        else{
          spheres[ball_grabbed]->get_node()->remove_parent(); 
          spheres[ball_grabbed]->get_node()->access_nodeToParent() = debug_skeleton->get_wrist_node()->obtain_joints().bone_node->get_nodeToParent();

          ball_grabbed = -1;
        }
      }*/
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

      index_mat = (index_mat + 1) % 20;
      table_instances[0]->set_material(table_mat[index_mat / 10]);
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
          moving_up_skeleton = false;
          cur_tic = 0;
          total_tic = debug_skeleton->finish_animation();
        }
      }
      else{
        if (dancing_skeleton){
          skeleton_dance_once(debug_skeleton);
        }
      }

      //Add confetti when dancing
      if (dancing_skeleton)
        reset_game();

      debug_skeleton->draw();

      //// update matrices. assume 30 fps.
      app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif