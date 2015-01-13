//////////////////////////////
///
/// @author Sam & Juanmi
/// 
/// Small classes that represents a quaternion and a dual-quaternion
///

#ifndef DualQuat_H_DEFINED
#define DualQuat_H_DEFINED

/// @brief This is a copy of Andy's class quaternionwith some minor improvements
namespace octet {
  namespace math {
    class Quaternion : public vec4
    {
    public:
      /// @brief This is the constructor, initialize a quaternion with w = 1 and v = (0,0,0)
      Quaternion() : vec4(0, 0, 0, 1) {}
      /// @brief This will create a quaternion with a given set of values
      Quaternion(float x, float y, float z, float w) : vec4(x, y, z, w) {}
      /// @brief This is the copy constructor for the quaternion
      Quaternion(const vec4 &r) { *(vec4*)this = r; }
      /// @brief This is the product between quaternions
      Quaternion operator*(const Quaternion &r) const { return Quaternion(qmul(r)); }
      /// @brief This is the product of the quaternion with a scalar
      Quaternion operator*(float r) const { return Quaternion((vec4&)*this * r); }
      /// @brief This is to be able to concatenate different multiplications of quaternions
      Quaternion &operator*=(const Quaternion &r) { *(vec4*)this = qmul(r); return *this; }
      /// @brief This is the conjugate (uses vec4 conjugate)
      Quaternion conjugate() const { return qconj(); }
      /// @brief This is to rotate the quaternion along a vector (it will be rotate by the magnitudine)
      vec4 rotate(const vec4 &r) const { return (*this * r) * conjugate(); }
      /// @brief This is to be able to normalize the quaternion (uses the vector normalizer)
      void normalize(){
        this->normalize();
      }
      /// @brief This is to be able to access the dot product of the vector
      float dot(const vec4 &r) const{
        return this->dot(r);
      }
    };
  }
}

/// @brief This is the class of the dual quaternion (consisting only in two quaternions together)
namespace octet{
  namespace math{
    class DualQuat{
      /// The real part of the dual-quaternion, represents the rotation
      Quaternion real_part;
      /// The dual part of the dual-quaternion, represents translation*rotation*0.5f
      Quaternion dual_part;
    public:
      /// @brief Constructor of the dual-quaternion. The default value is ((1,(0,0,0),(0,(0,0,0)))
      DualQuat(){
        real_part = Quaternion(0, 0, 0, 1);
        dual_part = Quaternion(0, 0, 0, 0);
      }
      /// @brief This creates a dual-quaternion with a given real part and a given dual part (both quaternions)
      DualQuat(Quaternion n_real_part, Quaternion n_dual_part){
        real_part = n_real_part;
        dual_part = n_dual_part;
      }
      /// @brief This is the dot product with another dual-quaternion
      /// This will receive another dual-quaternion and will return the float result
      float dot_product(DualQuat n_DualQuat){
        return this->real_part.dot(n_DualQuat.real_part);
      }
      /// @brief Multiplicator with an scalar
      DualQuat operator*(float scalar){
        real_part = Quaternion(real_part * scalar);
        dual_part = Quaternion(dual_part * scalar);
        return *this;
      }
      /// @brief This is to multiply quaternions
      DualQuat operator*(DualQuat n_DualQuat){
        real_part = real_part * n_DualQuat.real_part;
        dual_part = dual_part * n_DualQuat.real_part + real_part * n_DualQuat.dual_part;
      }
      /// @brief This is to normalize a dual-quaternion
      void normalize(){
        float magnitudine = real_part.dot(real_part);
        // This is to correct errors with the float point. If it's close to zero, it's zero!
        // And therefore, if it's zero, it has to "explode"
        assert(magnitudine > 0.000001f);
        real_part = real_part * (1.0f / magnitudine);
        dual_part = dual_part * (1.0f / magnitudine);
      }
      /// @brief This is to add quaternions
      DualQuat operator+(DualQuat n_DualQuat){
        real_part = real_part + n_DualQuat.real_part;
        dual_part = dual_part + n_DualQuat.dual_part;
      }
      /// @brief This return a new dual-quaternion that is the conjugate of the current one
      DualQuat conjugate(){
        return DualQuat(real_part.conjugate(), dual_part.conjugate());
      }
      /// @brief This will return a quaternion representing the rotation of the dual-quaternion
      Quaternion get_rotation(){
        return real_part;
      }
      /// @brief This will return a vec3 representing the translation of the dual-quaternion
      vec3 get_translation(){
        Quaternion translation = (dual_part * 2.0f) * real_part.conjugate();
        return vec3(translation.get()[0], translation.get()[1], translation.get()[2]);
      }
      /// @brief This will return a matrix valid for octet from the dual-quaternion
      /// As a side effect will normalize the current dual-quaternion
      mat4t get_matrix(){
        mat4t n_matrix;
        // These are the elements of the quaternion that represent the rotation
        float r_x = real_part.get()[0];
        float r_y = real_part.get()[1];
        float r_z = real_part.get()[2];
        float r_w = real_part.get()[3];
        // This is the vector that represent the translation
        vec3 translation = get_translation();
        // Now we combine that information and build the matrix that will represent the same information as the dual-quaternion
        n_matrix.init(vec4( r_w*r_w + r_x*r_x - r_y*r_y - r_z*r_z,  2*r_x*r_y + 2*r_w*r_z,  2*r_x*r_z - 2*r_w*r_y,  0 ),
                      vec4( 2*r_x*r_y - 2*r_w*r_z,  r_w*r_w + r_y*r_y - r_x*r_x - r_z*r_z,  2*r_y*r_z + 2*r_w*r_x,  0 ),
                      vec4( 2*r_x*r_z + 2*r_w*r_y,  2*r_y*r_z - 2*r_w*r_x,  r_w*r_w + r_z*r_z - r_x*r_x - r_y*r_y,  0 ),
                      vec4( translation.get()[0],  translation.get()[1],  translation.get()[2]  , 1 ));

        return n_matrix;
      }
    };
  }
}
  
#endif