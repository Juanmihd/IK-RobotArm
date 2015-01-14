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
    ref<DQ_Bone> parent; // The root bone will have a NULL parent
    dynarray<DQ_Bone*> children; //The leaf bone will have 0 children


    /// @brief Updates the world positions of this bone
    /// This function assumes that its parent's joint node has updated world coordinates
    /// This function will fix the world position and the scene_nodes of the joint and bone
    void update_w_positions(){
      //CODE TO DO THAT
      //Use dual-quaternions wizzardy to obtain own leaf position
    }

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

      //Set up the scene nodes to their proper positions (with the Dual-Quaternion)
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
      //fix the world positions and, then, the scene nodes
      update_w_positions();
    }

    /// @brief Update the w_positions information checking the parent and with the current dual-quaternion
    /// This function will be called by the skeleton before the draw process starts
    SceneNodes obtain_joints(){
      //Obtain the joints real world position
      return scene_nodes;
    }

    /// @brief This function will fix his joints and will ask his children to fix themselves
    /// This is a recursion. Careful. Sokol, we are talking to you.
    void fix_yourself(){
      //Fix yourself
      update_w_positions();
      //Ask your children to fix themselves
      for (size_t i = 0; i < children.size(); ++i)
      {
        //Tell each child his root position
        children[i]->set_root_position(leaf_position);
        //Tell him to fix himself
        children[i]->fix_yourself();
      }
    }

    // @brief This function returns the position of the leaf of the parent
    void set_root_position(const vec3 &position){
      root_position = position;
    }
  };
}

#endif