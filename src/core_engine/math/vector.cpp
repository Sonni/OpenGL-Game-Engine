#include "vector.hpp"
#include "quaternion.hpp"

vec3f vec3f::rotate(const quaternion &rotation) const
{
    quaternion conjugateQ = rotation.negate();
    quaternion w = rotation * (*this) * conjugateQ;

    vec3f ret(w.get_x(), w.get_y(), w.get_z());

    return ret;
}
