//////////////////////////////
///
/// @author Sokol, Sam & Juanmi
/// 
/// This class represents the skeleton (contains bones)
///

#ifndef DQ_SKELETON_H_INCLUDED
#define DQ_SKELETON_H_INCLUDED

#include "DQ_Bone.h"

/// @brief This class represent a skeleton made of dual-quaternions
///  Contains a set of bones, anchored to the root bone
namespace octet{
  enum TypeSkeleton_Anim { _RANDOM_DANCE = -1, _RANDOM_ALG = 0, _IK_ALG = 1 };
  enum StatusSkeleton { _STILL = -1, _ANIMATING = 0, _ALGORITHMING = 1 };
  enum {_REAL_RANDOM = 1};
  class DQ_Skeleton{
    //This bone is the root of the skeleton
    ref<DQ_Bone> root_bone;
    ref<DQ_Bone> wrist_bone;
    //This represent where the root bone is anchored
    DualQuat root_transform;
    //Info to draw the meshes 
    random rand_gen;
    ref<visual_scene> app_scene;
    // This is the status of the current skeleton
    StatusSkeleton status; 
    // This is the current distance to the objective
    vec3 dest_position;
    float range;
    int total_tic;

    // sphere mesh set up
    ref<mesh_sphere> mesh_joint;
    ref<material> mat_joint;
    ref<material> mat_bone; 

    /// @brief This function will update all the joints of the bones (starting from the root)
    void update_joints(){
      //printf("DQ_Skeleton update_joints call\n");
      // update current bone's joints
      if (root_bone != nullptr){
        //root_bone->animate_bone();
        root_bone->set_joint_node(root_transform);
        root_bone->fix_yourself(root_transform);
      }
    }

  public:
    /// @brief Default constructor of the DQ_Skeleton
    DQ_Skeleton(){
      range = 0;
      if (_REAL_RANDOM)
        rand_gen.set_seed(time(NULL));
      status = _STILL;
      //printf("DQ_Skeleton Contstructor call \n");
      root_bone = nullptr;
      // Initialising the meshes and materials
      // create the sphere and cylinder meshes used to draw the skeleton
      mesh_joint = new mesh_sphere(vec3(0, 0, 0), 0.75f);
      //set materials
      mat_joint = new material(vec4(1, 0, 0, 1));
      mat_bone = new material(vec4(0, 1, 0, 1));
    }

    /// @brief This function will return the status of the skeleton
    StatusSkeleton get_status(){ return status; }

    /// @brief inits the DQ_SKeleton, it needs a position to where to anchor the root bone
    void init(const vec3 &position = vec3(0,0,0)){
      //We obtain a pure translation with no rotation to represent the position of the anchor bone
      root_transform = DualQuat(Quaternion(0, 0, 0, 1), Quaternion(position.get()[0] / 2.0f, position.get()[1] / 2.0f, position.get()[2] / 2.0f, 0));
    }

    /// @brief Add the reference to the visual_scene
    void add_scene(visual_scene* scene){
      //printf("DQ_Skeleton add_scene call\n");
      app_scene = scene;
    }

    /// @brief Will add a new bone to a given parent
    /// If we give no information of the parent, we asume that the new bone is the root bone
    void add_bone(DQ_Bone* new_bone, DQ_Bone* parent = nullptr){
      //funky code to obtain the lenght of the bone
      float bone_lenght = 5;

      //Create the scene nodes
      scene_node* joint_node = new scene_node();
      scene_node* bone_node = new scene_node();

      //Add the scene_nodes to the new bone
      new_bone->add_scene_nodes(joint_node, bone_node);

      // add the bones_meshes to the scene
      mat4t cylinder_matrix;
      mesh_cylinder* mesh_bone;
      cylinder_matrix.translate(0, new_bone->get_length() / 2.0f, 0);
      cylinder_matrix.rotate(90, 1, 0, 0);
      mesh_bone = new mesh_cylinder(zcylinder(vec3(0), 0.5f, new_bone->get_length() / 2.0f), cylinder_matrix);
      range += new_bone->get_length();
      // we scale the bone mesh
      app_scene->add_mesh_instance(new mesh_instance(new_bone->obtain_joints().joint_node, mesh_joint, mat_joint));
      app_scene->add_mesh_instance(new mesh_instance(new_bone->obtain_joints().joint_node, mesh_bone, mat_bone));

      // Add child to the parent (or sets the root to be the new bone)
      if (parent == nullptr){
        root_bone = new_bone;
        root_bone->set_parent(nullptr);
        //fix root_bone world positions
        root_bone->set_joint_node(root_transform);
      }
      else
        parent->add_child(new_bone);
      wrist_bone = new_bone;
    }

    float get_range(){ return range; }

    /// @brief Draws the skeleton (to be called by the app)
    /// Internally, this function will update all the joints and THEN will draw all the bones
    void draw(){
      //printf("DQ_Skeleton draw call\n");
      // update all the joint positions of all the bones
      update_joints();
    }

    /// @brief This function will generate a new animation
    /// It will return the num of tics expected to produce this animation
    int start_animation(TypeSkeleton_Anim anim_type = _RANDOM_DANCE, vec3 n_position = vec3(0,0,0)){
      total_tic = 10;
      if (anim_type == _RANDOM_DANCE){
        randomize(root_bone);
        status = _ANIMATING;
      }
      else if (anim_type == _RANDOM_ALG){
        dest_position = n_position;
        float distance = random_algorithm(dest_position);
        //obtain the number of tics depending the distance!
        //Get position of the bone
        vec3 next_bone_position = wrist_bone->get_best_position_bone();
        vec3 bone_position = wrist_bone->get_position_bone();
        //Obtain distance of the wirst bone with n_position
        vec3 v_distance = next_bone_position - bone_position;
        float local_distance = v_distance.length();
        if (distance != 0)
          total_tic = 2 * distance;
        else
          total_tic = 40;
        status = _ALGORITHMING;
      }

      //Update the total_tic (ToDo) by the distance of the movement

      return total_tic;
    }

    /// @brief This will animate the skeleton
    /// To operate this function, the app will send the "lenght" that the animation will take place 
    /// Will give a total number of tics, and the cur tic
    void animate_skeleton(int cur_tic, int total_tics){
      root_bone->animate_bone(cur_tic, total_tics);
    }

    /// @brief This will stop the animation
    int finish_animation(bool force = false){
      root_bone->finish_animation(force);
      //Now it will check if it has to keep on animating or will go back to still
      if (force)
        status = _STILL;
      else if (status == _ALGORITHMING){
        //check distance, if it's close enough, stop! if not, keep on doing that!
        vec3 bone_position = wrist_bone->get_best_position_bone();
        //Obtain distance of the wirst bone with n_position
        vec3 v_distance = dest_position - bone_position;
        float current_distance = v_distance.length();
        printf("Current distance is... %f\n", current_distance);
        if (total_tic > 10) total_tic -= 2;
        if (current_distance < 1)
          status = _STILL;
        else
          total_tic = 2 * random_algorithm(dest_position);
      }
      else
        status = _STILL;
      return total_tic;
    }

    //--- Now will start some functions to set the next position for the arm

    /// @brief It will obtain a completely Random position of the tree
    void randomize(DQ_Bone* bone){
      //obtain random dual quaternion (only a small random rotation)
      DualQuat next_dualquat = bone->generate_random_next(rand_gen.get(-1.0f, 1.0f), rand_gen.get(-1.0f, 1.0f), rand_gen.get(-1.0f, 1.0f), rand_gen.get(-1.0f, 1.0f));
      bone->set_next_position(next_dualquat);
      //set the next position of this arm
      for (int i = 0; i < bone->get_children().size(); ++i){
        randomize(bone->get_children()[i]);
      }
    }

    /// @brief It will obtain a completely Random position of the tree (as a potential solution)
    void randomize_best(DQ_Bone* bone, float distance = 1){
      //obtain random dual quaternion (only a small random rotation)
      DualQuat next_dualquat = bone->generate_random_next(rand_gen.get(-0.5f, 0.5f)*distance, rand_gen.get(-1.0f, 1.0f), rand_gen.get(-1.0f, 1.0f), rand_gen.get(-1.0f, 1.0f));
      bone->set_best_position(next_dualquat);
      //set the next position of this arm
      for (int i = 0; i < bone->get_children().size(); ++i){
        randomize_best(bone->get_children()[i]);
      }
    }

    /// @brief This will obtain a random position of the arm, but trying to get close to the position
    /// It will obtain 5 possible positions and will choose the one closer to the objective.
    float random_algorithm(vec3 position){
      //Initialize values
      size_t num_tests = 100;
      //Obtain distance of the wirst bone with n_position
      vec3 v_distance = position - wrist_bone->get_best_position_bone();
      float distance_min = v_distance.length();
      //Start testing things
      for (size_t i = 0; i < num_tests; i++)
      {
        //Generate solution (recursively)
        randomize_best(root_bone, distance_min);
        //Evaluate solution
        root_bone->test_yourself(root_transform);
        //Obtain distance of the wirst bone with n_position
        vec3 v_distance = position - wrist_bone->get_best_position_bone();
        float current_distance = v_distance.length();
        //Check solution
        if (current_distance < distance_min){
          distance_min = current_distance;
          //Set best_solution as next_position
          root_bone->set_best_as_next();
        }
      }
      return distance_min;
    }

    /// @brief Returns the wirst node from the skeleton (it's a DQ_Bone!)
    DQ_Bone* get_wrist_node(){
      return wrist_bone;
    }
  };
}

#endif