
#pragma once

#include "Core.hpp"
#include "Utils/Transform.hpp"

namespace Ilya
{
    class Shape
    {
        public:

            /// @brief Generic shape, defined in object
            /// coordinate space
            ///
            /// @param objtoworld Object space to world space
            ///     transform
            /// @param reverse_orientation Whether the surface
            ///     normals should be reversed from default (a
            ///     rectangular light hanging from the ceiling,
            ///     for example)
            ///
            /// Another member is set, `T_swaps_handedness`,
            /// which tells if the object-space to world-space
            /// transform swaps the coordinate system
            /// handedness.
            Shape(const Ref<Transform>& objtoworld, bool reverse_orientation);

            virtual Bounds objspace_bounds() const = 0;
            virtual Bounds worldspace_bounds() const;

            virtual bool hit(const Ray& r, float& t, SurfaceInteraction& isect) const = 0;
            virtual float area() const = 0;

        public:

            const Ref<Transform> objtoworld, worldtoobj;
            const bool reverse_orientation;
            const bool T_swaps_handedness;
    };
}