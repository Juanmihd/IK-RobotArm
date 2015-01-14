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
  class DQ_Skeleton{
    //This bone is the root of the skeleton
    ref<DQ_Bone> root_bone;
    //This represent where the root bone is anchored
    DualQuat transform;
    //Info to draw the meshes 
    ref<visual_scene> app_scene;

    // sphere mesh set up
    ref<mesh_sphere> mesh_joint;
    ref<mesh_cylinder> mesh_bone;
    dynarray<mesh_instance> skeleton_mesh_instances;
    ref<material> mat_joint;
    ref<material> mat_bone;
    
    // position of the skeleton
    vec3 origin;

    /// @brief This function will update all the joints of the bones (starting from the root)
    void update_joints(){
      printf("DQ_Skeleton update_joints call\n");
      // tell the root bone his root position
      root_bone->set_root_position(origin);
      // update current bone's joints
      root_bone->fix_yourself();
    }

  public:
    /// @brief Default constructor of the DQ_Skeleton
    DQ_Skeleton(){
      printf("DQ_Skeleton Contstructor call \n");

      origin = vec3(0);
      // Initialising the meshes and materials
      // create the sphere and cylinder meshes used to draw the skeleton
      mesh_joint = new mesh_sphere();
      mesh_bone = new mesh_cylinder(zcylinder(vec3(0), 0.5f, 2.0f));
      //set materials
      mat_joint = new material(vec4(1, 0, 0, 1));
      mat_bone = new material(vec4(0, 1, 0, 1));
    }

    /// @brief Add the reference to the visual_scene
    void add_scene(visual_scene* scene){
      printf("DQ_Skeleton add_scene call\n");
      app_scene = scene;
      }

    /// @brief Will add a new bone to a given parent
    /// If we give no information of the parent, we asume that the new bone is the root bone
    void add_bone(const DualQuat &transform, DQ_Bone* parent = nullptr){
      DQ_Bone* new_bone = new DQ_Bone(transform);
      //funky code to obtain the lenght of the bone
      float bone_lenght = 5;

      //Create the scene nodes
      scene_node* joint_node = new scene_node();
      scene_node* bone_node = new scene_node();

      //Add the scene_nodes to the new bone
      new_bone->add_scene_nodes(joint_node, bone_node);

      // add the bones_meshes to the scene
      // we scale the bone mesh
      app_scene->add_mesh_instance(new mesh_instance(new_bone->obtain_joints().joint_node, mesh_joint, mat_joint));
      app_scene->add_mesh_instance(new mesh_instance(new_bone->obtain_joints().bone_node, mesh_bone, mat_bone));

      // Add child to the parent (or sets the root to be the new bone)
      if (parent == nullptr){
        root_bone = new_bone;
        //fix root_bone world positions
      }
      else
        parent->add_child(new_bone);
    }

    /// @brief Draws the skeleton (to be called by the app)
    /// Internally, this function will update all the joints and THEN will draw all the bones
    void draw(){
      printf("DQ_Skeleton draw call\n");
      // update all the joint positions of all the bones
      update_joints();
    }
  };
}

#endif