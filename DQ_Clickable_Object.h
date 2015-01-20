//////////////////////////////
///
/// @author Sokol, Sam & Juanmi
/// 
/// This class will form the base for all clickable objects
///

#ifndef DQ_CLICKABLE_OBJ_H_INCLUDED
#define DQ_CLICKABLE_OBJ_H_INCLUDED

#include "dualquat.h"
/// @brief Objects of this class will clickable, doing so shall return a pointer to this obejct
/// Objects of this class will have the option to be bullet physics objects.
namespace octet{
  class ClickableObject : public resource{
  private:
    DualQuat dual_quat;
    mesh _mesh;
    material *mat;
    bool is_dynamic;
    float mass;
    btRigidBody* rigidbody;
    ref<visual_scene> app_scene;

  public:
    ClickableObject(){}
    ~ClickableObject(){}

    /// @brief initialise the clickable object given all parameters
    void init(visual_scene &app_scene_in, DualQuat dual_quat ){

    }
  };
}


#endif