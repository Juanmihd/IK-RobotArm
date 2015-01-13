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
    // World positions
    vec3 root_position; //from parent
    vec3 leaf_position; //from child

    //scene_nodes structure
    struct SceneNodes{
      ref<scene_node> joint_node;
      ref<scene_node> bone_node;
    };
    SceneNodes scene_nodes;

    // Adjacent bones
    DQ_Bone* parent; // The root bone will have a NULL parent
    dynarray<DQ_Bone*> children; //The leaf bone will have 0 children

  public:
    /// @brief Default constructor of the Bone
    DQ_Bone(){
      printf("DQ_Bone Contstructor call \n");
    }

    /// @brief Constructor of the Bone from a Quaternion
    DQ_Bone(DualQuat n_transfom){}

    /// @brief Add the scene nodes for the joint and the bone
    void add_scene_nodes(scene_node* n_joint_node, scene_node* n_bone_node){
      printf("DQ_Bone add_scene_nodes call\n");
      scene_nodes.joint_node = n_joint_node;
      scene_nodes.bone_node = n_bone_node;
    }

    /// @brief Adds a new child to the current bone
    void add_child(DQ_Bone* child){}

    /// @brief Update the w_positions information checking the parent and with the current dual-quaternion
    /// This function will be called by the skeleton before the draw process starts
    SceneNodes obtain_joints(){
      //Obtain the joints real world position
      return scene_nodes;
    }
  };
}

#endif