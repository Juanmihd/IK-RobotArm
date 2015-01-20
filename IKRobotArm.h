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
  public:
    /// this is called when we construct the class before everything is initialised.
    IKRobotArm(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      cam = app_scene->get_camera_instance(0);
      world = app_scene->get_bt_world();
      assert(world);

      // SH: create debug shape to test object picker (should be removed eventually) 
      mat4t mat;
      mat4t transform_floor;
      transform_floor.rotateX90();
      transform_floor.translate(vec3(0, -2, 0));
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
      debug_skeleton->init();
      //Add the root bone
      DQ_Bone* root = new DQ_Bone(4.0f);
      DQ_Bone* forearm = new DQ_Bone(2.0f);
      DQ_Bone* arm = new DQ_Bone(1.0f);
      debug_skeleton->add_bone(root);
      debug_skeleton->add_bone(forearm, root);
      debug_skeleton->add_bone(arm, forearm);
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

      // raycasting
      if (is_key_going_down(key_lmb)){
        cast_ray_from_mouse(start, end);
        btCollisionWorld::ClosestRayResultCallback rayCallBack(start, end);
        world->rayTest(start, end, rayCallBack);
        if (rayCallBack.hasHit()){
          printf("ray cast world pos: x: %f y: %f z: %f\n", rayCallBack.m_hitPointWorld.x(), rayCallBack.m_hitPointWorld.y(), rayCallBack.m_hitPointWorld.z());
        }
      }
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}

#endif