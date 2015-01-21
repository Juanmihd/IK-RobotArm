//////////////////////////////
///
/// @author Sokol, Sam & Juanmi
/// 
/// This class represents the bone (including the mesh and the dual-quaternion)
///

#ifndef DQ_BONE_H_INCLUDED
#define DQ_BONE_H_INCLUDED

#include "dualquat.h"


/// @brief This class represents a bone (sublcass of mesh, and containin the dual-quaternion)
namespace octet{
  class DQ_Bone : public resource{
    // This is the dual-quaternion that contains the information of the curren transform
    DualQuat transform; //local
    DualQuat world_transform; // world
    DualQuat best_world_transform; // best_world
    float length; //length of the bone

    //Some info to interpolate!
    DualQuat prev_transform; // previous local
    DualQuat next_transform; // next local
    DualQuat best_transform; // best_solution
    float theta;
    float costheta;
    float sintheta;

    //Info to constraint movement and random generation
    //If (0,0,0) no constraints
    bool rotation_constraint;
    vec3 rotation_axis; 
    // Minimum angle
    float angle_min; 
    // Maximum angle
    float angle_max;
    // Incremention angle
    float angle_inc;

    //scene_nodes structure
    struct SceneNodes{
      ref<scene_node> joint_node;
      ref<scene_node> bone_node;
    };
    SceneNodes scene_nodes;

    // Adjacent bones
    ref<DQ_Bone> parent; // The root bone will have a NULL parent
    dynarray<DQ_Bone*> children; //The leaf bone will have 0 children
    
  public:
    /// @brief Default constructor of the Bone
    DQ_Bone(){
      //printf("DQ_Bone Contstructor call \n");
    }

    /// @brief Constructor of the Bone from a Dual-Quaternion
    DQ_Bone(DualQuat n_transfom){
      transform = n_transfom;
      init_values();
    }

    /// @brief Constructor of the Bone by a given length
    DQ_Bone(float n_length){
      length = n_length;
      init_values();
      if (length < 0) length = -length;
      DualQuat translation = DualQuat(Quaternion(0, 0, 0, 1), Quaternion(0, length / 2.0f, 0, 0));
      transform = translation;
    }

    void init_values(){
      rotation_constraint = false;
      rotation_axis = (0.0f, 0.0f, 0.0f);
      angle_min = 0.0f;
      angle_max = 2 * 3.141592653f;
      angle_inc = 3.141592653f / 8.0f;
    }

    /// @brief This function will set the range of the random generation of next "jump"
    void set_range(float n_angle_inc, const vec3 &direction = (0.0f, 0.0f, 0.0f), float n_angle_min = 0.0f, float n_angle_max = 2 * 3.141592653f){
      angle_inc = n_angle_inc;
      rotation_axis = direction;
      angle_min = n_angle_min;
      angle_max = n_angle_max;
    }

    /// @brief This function will set the range of the random generation of next "jump"
    void constraint_in_axis(float x, float y, float z){
      rotation_axis = vec3(x, y, z);
      rotation_axis = rotation_axis.normalize();
      rotation_constraint = true;
    }

    /// @brief This function will set the range of the random generation of next "jump"
    void constraint_in_angle(float n_angle_min, float n_angle_max){
      angle_min = n_angle_min;
      angle_max = n_angle_max;
    }

    /// @brief Add the scene nodes for the joint and the bone
    void add_scene_nodes(scene_node* n_joint_node, scene_node* n_bone_node){
      //printf("DQ_Bone add_scene_nodes call\n");
      scene_nodes.joint_node = n_joint_node;
      scene_nodes.bone_node = n_bone_node;
    }

    /// @brief Adds a parent to this bone
    void set_parent(DQ_Bone* n_parent){
      parent = n_parent;
    }

    /// @brief Adds a new child to the current bone
    void add_child(DQ_Bone* child){
      //this bone is going to be the parent of the child bone
      child->set_parent(this);
      //add the children
      children.push_back(child);
    }

    /// @brief Update the w_positions information checking the parent and with the current dual-quaternion
    /// This function will be called by the skeleton before the draw process starts
    SceneNodes obtain_joints(){
      //Obtain the joints real world position
      return scene_nodes;
    }

    /// @brief Returns the position of the bone
    vec3 get_position_bone(){
      vec4 position = scene_nodes.bone_node->access_nodeToParent().z();
      return vec3(position.get()[0], position.get()[1], position.get()[2]);
    }

    /// @brief Returns the estimated position of the bone
    vec3 get_best_position_bone(){
      return best_world_transform.get_translation();
    }

    /// @brief Returns the estimated position of the bone
    vec3 get_world_position_bone(){
      return world_transform.get_translation();
    }

    /// @brief This function will fix his joints and will ask his children to fix themselves
    /// This function asumes that the current bone has a correct joint_node information (setted by the parent)
    /// This is a recursion. Careful. Sokol, we are talking to you.
    void fix_yourself(const DualQuat &n_transform){
      //Fix yourself
      world_transform = n_transform.qMult(transform);
      world_transform.normalize();

      //dual-quaternion-magically obtain bone node
      scene_nodes.bone_node->access_nodeToParent() = world_transform.get_matrix();

      //Ask your children to fix themselves
      for (size_t i = 0; i < children.size(); ++i)
      {
        children[i]->set_joint_node(world_transform);
        children[i]->fix_yourself(world_transform);
      }
    }

    /// @brief This function will obtain the potential ending of the current solution
    /// This function asumes that the current bone has a correct joint_node information (setted by the parent)
    /// This is a recursion. Careful. Sokol, we are talking to you.
    void test_yourself(const DualQuat &n_transform){
      //Fix yourself
      best_world_transform = n_transform.qMult(best_transform);
      best_world_transform.normalize();

      //Ask your children to fix themselves
      for (size_t i = 0; i < children.size(); ++i)
      {
        children[i]->test_yourself(best_world_transform);
      }
    }

    /// @brief This will go through the whole set of arms, setting the next_position as the best_position
    void set_best_as_next(){
      next_transform = best_transform;
      //Ask your children to fix themselves
      for (size_t i = 0; i < children.size(); ++i)
      {
        children[i]->set_best_as_next();
      }
    }

    /// Set the joint node
    void set_joint_node(DualQuat &n_transform){
      scene_nodes.joint_node->access_nodeToParent() = n_transform.get_matrix();
    }

    /// returns the length
    float get_length(){ 
      //printf("%f\n", length);
      return length;
    }

    /// @brief This function grants access to the children of the node
    dynarray<DQ_Bone*>  get_children(){
      return children;
    }

    /// @brief This function will set the next position that this bone has to go
    void set_next_position(DualQuat &n_position){
      prev_transform = transform;
      next_transform = n_position;
      costheta = prev_transform.dot_product(next_transform);
      theta = acosf(costheta);
      sintheta = sinf(theta);
    }

    /// @brief This function will set the best position that this bone has to go
    void set_best_position(DualQuat &n_position){
      best_transform = n_position;
    }

    /// @brief This function will interpolate the bone by the given information
    /// This function will receive a cur_tic and the total_tic. It will set the
    /// current world_transform getting into account the initial and the end
    /// position of the arm
    void animate_bone(int cur_tic, int total_tics){
      float tic = 1.0f*cur_tic / total_tics;

      //printf("transform: %f\n", tic);
      //transform.print_me();
      if (sintheta > 0.000001)
        transform = prev_transform * (sinf((1.0f - tic)*theta) / sintheta) +
        next_transform * (sinf(tic*theta) / sintheta);
      else
        transform = prev_transform + (next_transform + (prev_transform * -1.0f))*tic;
      transform.normalize();
      //Tell all the children to animate themselves
      for (size_t i = 0; i < children.size(); i++)
        children[i]->animate_bone(cur_tic, total_tics);
    }

    /// @brief This function will be called to finish an animation
    /// This will set the prev_transform and the current transform as the next transform so far
    /// it will recursively call its children to finish the animation
    void finish_animation(bool force){
      if (force){
        prev_transform = transform;
        next_transform = transform;
      }
      else{
        prev_transform = next_transform;
        transform = next_transform;
      }

      //Tell all the children to finish themselves
      for (size_t i = 0; i < children.size(); i++)
        children[i]->finish_animation(force);
    }

    /// @brief This function generates and return a new random position for this arm
    DualQuat generate_random_next(float new_angle, float x, float y, float z){
      vec3 new_axis = vec3(x, y, z);
      //printf("Randomly rotating %f degrees along (%f, %f, %f)\n", new_angle, new_axis.get()[0], new_axis.get()[1], new_axis.get()[2]);
      new_axis = new_axis.normalize();
      if (rotation_constraint)
        new_axis = rotation_axis;
      //printf("\n%f\n", angle_inc);
      new_angle = angle_inc * new_angle;
      //if (new_angle >= angle_max) new_angle = angle_max;
      //if (new_angle <= angle_min) new_angle = angle_min;
      float cos_half_angle = cos(new_angle / 2.0f);
      float sin_half_angle = sin(new_angle / 2.0f);
      //printf("Randomly rotating %f degrees along (%f, %f, %f)\n", new_angle, new_axis.get()[0], new_axis.get()[1], new_axis.get()[2]);
      //Creating the increment dualquat to add to the current dualquat
      DualQuat increment = DualQuat(Quaternion(new_axis.get()[0] * sin_half_angle, 
                                              new_axis.get()[1] * sin_half_angle,
                                              new_axis.get()[2] * sin_half_angle,
                                              cos_half_angle),
                                    Quaternion(0, 0, 0, 0));
      DualQuat temp = transform.qMult(increment);
      /*
      printf("Transform: \n");
      transform.print_me();
      printf("Increment: \n");
      increment.print_me();
      printf("Temp: \n");
      temp.print_me();
      printf("\n\n");*/
      return temp;
    }
  };
}

#endif