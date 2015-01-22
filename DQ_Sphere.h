//////////////////////////////
///
/// @author Sam & Juanmi & Sokol
/// 
/// Small classes that represents a quaternion and a dual-quaternion
///


#ifndef DQ_SPHERE_H_INCLUDED
#define DQ_SPHERE_H_INCLUDED

#include "dualquat.h"
namespace octet{
  /// @brief This class represents a sphere which will have a charge, these
  /// sphere are design to be interacted upon by the robot arm.
  class DQ_Sphere : public resource{
  private:
    // private data for the DQ_sphere. 
    btRigidBody* rigid_body; // allow bt raycasting implementation plus physics sim.
    mat4t world_transform; 
    ref<scene_node> node;
    float magnetism_power; 
    mesh_instance* mesh_i;

    // pointer to visual scene used to add the shape to the world(s)
    visual_scene* app_scene;

  public:
    DQ_Sphere(){
      mesh_i = nullptr;
    };

    ~DQ_Sphere(){};

    /// @brief initialse defaults for the sphere.
    void init(visual_scene* vs, vec3 pos = vec3(0), float radius = 1.0f, float n_magnetism_power = 40.0f){
      material* purple;
      app_scene = vs;
      magnetism_power = n_magnetism_power;
      if (n_magnetism_power < 0)
        purple = new material(vec4(0.2f, 0.5f, 0.5f, 1));
      else
        purple = new material(vec4(0.5f, 0.2f, 0.8f, 1));
      world_transform[3] = vec4(pos, 1);
      mesh_instance* mesh_i = app_scene->add_shape(mat4t_in(world_transform), new mesh_sphere(vec3(0), radius), purple, true);
      node = mesh_i->get_node();
      rigid_body = node->get_rigid_body();
      rigid_body->setActivationState(DISABLE_DEACTIVATION);
    }

    /// @brief This function is used to get the rigid body of the ball
    btRigidBody* get_rigidbody(){
      return rigid_body;
    }

    /// @brief This function is used to get the position of the ball
    vec3 get_pos(){
      return world_transform[3].xyz();
    }

    scene_node* get_node(){
      return node;
    }

    /// @brief this function gets the end position of the DQ_Skeleton and then
    /// produces an attuenated force on the rigidbody of this sphere inversely 
    /// proportional to the square of the distance to the centre of the SPhere.
    void resolve_magnetic_force(vec3 wrist_pos){
      // calculate vector between the wrist and this sphere
      vec3 force = node->get_nodeToParent()[3].xyz() - wrist_pos;
      force.get()[1] = 0.0f;
      float magnitude = force.lengthRecip();
      force = force.normalize();
      magnitude = magnetism_power * magnitude;
      btVector3 temp = get_btVector3(force * magnitude);
      this->get_rigidbody()->applyCentralImpulse(temp);
    }

    /// @brief This function is used to set the magentism power of the ball
    void set_magnetism_power(float n_magnetism_power){
      magnetism_power = n_magnetism_power;
    }
  };
}

#endif

