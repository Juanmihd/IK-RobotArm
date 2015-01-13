//////////////////////////////
///
/// @author Sokol, Sam & Juanmi
/// 
/// This class represents the skeleton (contains bones)
///

#ifndef DQ_SKELETON_H_DEFINED
#define DQ_SKELETON_H_DEFINED

#include "DQ_Bone.h"

/// @brief This class represent a skeleton made of dual-quaternions
///  Contains a set of bones, anchored to the root bone
namespace octet{
  class DQ_Skeleton{
    //This bone is the root of the skeleton
    DQ_Bone root_bone;
    //This represent where the root bone is anchored
    DualQuat transform;
    //Meshes 

    /// @brief This function will update all the joints of the bones (starting from the root)
    void update_joints(){}

    /// @brief This function will draw all the bones one by one (after the update joints is called)
    void draw_bones(){}

  public:
    /// @brief Default constructor of the DQ_Skeleton
    DQ_Skeleton(){}

    /// @brief Will add a new bone to the skeleton
    void add_bone(DQ_Bone bone){}

    /// @brief Draws the skeleton (to be called by the app)
    /// Internally, this function will update all the joints and THEN will draw all the bones
    void draw(){
      update_joints();
      draw_bones();
    }
  };
}

#endif