//////////////////////////////
///
/// @author Sokol, Sam & Juanmi
/// 
/// This class represents the bone (including the mesh and the dual-quaternion)
///

#ifndef DQ_BONE_H_DEFINED
#define DQ_BONE_H_DEFINED

/// @brief This class represents a bone (sublcass of mesh, and containin the dual-quaternion)
namespace octet{
  class DQ_Bone : public resource{
    /// @brief This structure contains two different vec3 containin the world positions of the bone
    struct joints{
      vec3 root; //from parent
      vec3 leaf; //from child
    };

    // This is the dual-quaternion that contains the information of the curren transform
    DualQuat transform; //local
    // World positions
    joints w_positions;
    // Adjacent bones
    DQ_Bone* parent;
    dynarray<DQ_Bone*> children;
    // Meshes
    mesh_cylinder cylinder;
    mesh_sphere sphere;

  public:
    /// @brief Default constructor of the Bone
    DQ_Bone(){}
    /// @brief Constructor of the Bone from a Quaternion and a parent
    DQ_Bone(DualQuat n_transfom, DQ_Bone* n_parent){}
    /// @brief Adds a new child to the current bone
    void add_child(DQ_Bone* child){}
    /// @brief Update the w_positions information checking the parent and with the current dual-quaternion
    /// This function will be called by the skeleton before the draw process starts
    void obtain_joints(){
    }
    /// @brief Draw the meshes (cylinder and sphere)
    /// This function will be called by the skeleton AFTER the obtain joints process has finished
    void draw(){
    }
  };
}

#endif