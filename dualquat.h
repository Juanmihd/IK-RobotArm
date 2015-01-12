//////////////////////////////
///
/// @author Sam & Juanmi
/// 
/// Small class that represents a dual-quaternion
///

namespace octet {
  namespace math {
    class quaternion : public vec4
    {
    public:
      quaternion() : vec4(0, 0, 0, 1) {}
      quaternion(float x, float y, float z, float w) : vec4(x, y, z, w) {}
      quaternion(const vec4 &r) { *(vec4*)this = r; }
      quaternion operator*(const quaternion &r) const { return quaternion(qmul(r)); }
      quaternion operator*(float r) const { return quaternion((vec4&)*this * r); }
      quaternion &operator*=(const quaternion &r) { *(vec4*)this = qmul(r); return *this; }
      quaternion conjugate() const { return qconj(); }
      vec4 rotate(const vec4 &r) const { return (*this * r) * conjugate(); }
      void normalize(){
        this->normalize();
      }
    };
  }
}

namespace octet{
  namespace math{
    class dualquat{
      quaternion real_part; // rotation
      quaternion dual_part; // trans*rotation*0.5f
    public:
      dualquat(){
        real_part = quaternion(0, 0, 0, 1);
        dual_part = quaternion(0, 0, 0, 0);
      }

      dualquat(quaternion n_real_part, quaternion n_dual_part){
        real_part = n_real_part;
        dual_part = n_dual_part;
      }
    };
  }
}