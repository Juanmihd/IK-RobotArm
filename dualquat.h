//////////////////////////////
///
/// @author Sam & Juanmi
/// 
/// Small classes that represents a quaternion and a dual-quaternion
///

/// @brief This is a copy of Andy's class quaternionwith some minor improvements
namespace octet {
  namespace math {
    class quaternion : public vec4
    {
    public:
      /// @brief This is the constructor, initialize a quaternion with w = 1 and v = (0,0,0)
      quaternion() : vec4(0, 0, 0, 1) {}
      /// @brief This will create a quaternion with a given set of values
      quaternion(float x, float y, float z, float w) : vec4(x, y, z, w) {}
      /// @brief This is the copy constructor for the quaternion
      quaternion(const vec4 &r) { *(vec4*)this = r; }
      /// @brief This is the product between quaternions
      quaternion operator*(const quaternion &r) const { return quaternion(qmul(r)); }
      /// @brief This is the product of the quaternion with a scalar
      quaternion operator*(float r) const { return quaternion((vec4&)*this * r); }
      /// @brief This is to be able to concatenate different multiplications of quaternions
      quaternion &operator*=(const quaternion &r) { *(vec4*)this = qmul(r); return *this; }
      /// @brief This is the conjugate (uses vec4 conjugate)
      quaternion conjugate() const { return qconj(); }
      /// @brief This is to rotate the quaternion along a vector (it will be rotate by the magnitudine)
      vec4 rotate(const vec4 &r) const { return (*this * r) * conjugate(); }
      /// @brief This is to be able to normalize the quaternion (uses the vector normalizer)
      void normalize(){
        this->normalize();
      }
    };
  }
}

/// @brief This is the class of the dual quaternion (consisting only in two quaternions together)
namespace octet{
  namespace math{
    class dualquat{
      /// The real part of the dual-quaternion, represents the rotation
      quaternion real_part; 
      /// The dual part of the dual-quaternion, represents translation*rotation*0.5f
      quaternion dual_part; 
    public:
      /// @brief Constructor of the dual-quaternion. The default value is ((1,(0,0,0),(0,(0,0,0)))
      dualquat(){
        real_part = quaternion(0, 0, 0, 1);
        dual_part = quaternion(0, 0, 0, 0);
      }
      /// @brief This creates a dual-quaternion with a given real part and a given dual part (both quaternions)
      dualquat(quaternion n_real_part, quaternion n_dual_part){
        real_part = n_real_part;
        dual_part = n_dual_part;
      }
    };
  }
}