
#pragma once

#include "Math.hpp"

namespace Ilya
{
    class Transform
    {
        public:

            Transform(const Mat4& transform, const Mat4& inverse):
                transform(transform), inv(inverse) {}

            Transform(const Mat4& transform):
                Transform(transform, inverse(transform))
            {}

        private:

            Mat4 transform;
            Mat4 inv;
    };
}