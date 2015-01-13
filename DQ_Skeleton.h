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

    /// @brief This function will update all the joints of the bones (starting from the root)
    void update_joints(){}

  public:
    /// @brief Default constructor of the DQ_Skeleton
    DQ_Skeleton(){}

    /// @brief Add the reference to the visual_scene
    void add_scene(visual_scene* scene){
      app_scene = scene;
    }

    /// @brief Will add a new bone to a given parent
    /// If we give no information of the parent, we asume that the new bone is the root bone
    void add_bone(DualQuat child, DQ_Bone* parent = nullptr){
      DQ_Bone* new_bone = new DQ_Bone(child);
      //Create the scene nodes
      scene_node* joint_node = new scene_node();
      scene_node* bone_node = new scene_node();
      //Add the scene_nodes to the new bone
      new_bone->add_scene_nodes(joint_node, bone_node);

      //Here will be the code for the initialisation of the scene nodes and mesh instances

      // Add child to the parent (or sets the root to be the new bone)
      if (parent == nullptr)
        root_bone = new_bone;
      else
        parent->add_child(new_bone);
    }

    /// @brief Draws the skeleton (to be called by the app)
    /// Internally, this function will update all the joints and THEN will draw all the bones
    void draw(){
      update_joints();
    }
  };
}

#endif