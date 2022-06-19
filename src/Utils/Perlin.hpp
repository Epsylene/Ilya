
#pragma once

#include "Ilya.hpp"
#include "Vector3.hpp"

namespace Ilya
{

    class Perlin
    {
        public:

            Perlin()
            {
                for (int i = 0; i < pt_count; ++i)
                {
                    ranvec.emplace_back(unit(Vec3::random(-1.f, 1.f)));
                }

                gen_perm(permX);
                gen_perm(permY);
                gen_perm(permZ);
            }

            float noise(const Vec3& p) const
            {
                // Perlin noise is a kind of gradient noise, where a
                // lattice of random gradients are used to interpolate
                // values in between the cells. A point P in space is
                // given local coordinates (u, v, w) in the cell indexed
                // by (i, j, k); the next step is to generate the random
                // vectors at the vertices and interpolate between them.
                auto u = p.x - std::floor(p.x);
                auto v = p.y - std::floor(p.y);
                auto w = p.z - std::floor(p.z);

                auto i = static_cast<int>(std::floor(p.x));
                auto j = static_cast<int>(std::floor(p.y));
                auto k = static_cast<int>(std::floor(p.z));

                Vec3 weight[2][2][2];
                for (int dx = 0; dx < 2; ++dx)
                {
                    for (int dy = 0; dy < 2; ++dy)
                    {
                        for (int dz = 0; dz < 2; ++dz)
                        {
                            // Each cell of the Perlin lattice has 8 vectors
                            // that are to be interpolated ("weights"), which
                            // we will get from the 'ranvec' array; the index
                            // which to check this array is randomized
                            // alongside the X, Y and Z with 'permX/Y/Z',
                            // which each contain a shuffled integer range
                            // from 0 to 255. The "& 255" bitmask here serves as
                            // a modulo in the [0, 255] range because 256 is
                            // power of two; we use & instead of % because it
                            // works well with negative values (for example,
                            // -10 % 255 gives -10, while -10 & 255 gives 246,
                            // which is the expected value).
                            weight[dx][dy][dz] = ranvec[permX[(i + dx) & 255] ^
                                                        permY[(j + dy) & 255] ^
                                                        permZ[(k + dz) & 255]];
                        }
                    }
                }

                return perlin_interp(weight, u, v, w);
            }

            float turbulence(Vec3 p, int depth = 7) const
            {
                auto sum = 0.f;
                auto amplitude = 1.f;

                // Turbulence is the name given to a composite noise
                // resulting from the weighted sum of the noise function
                // over increasing frequencies; that is, if T is the
                // turbulence and N the noise function, we have that
                // T(p) = N(p) + (1/2)*N(2*p) + (1/4)*N(4*p) + ...
                // Noise constructed in this way is known as "value noise",
                // as opposed to "gradient noise", like the one used in the
                // Perlin noise function.
                for (int i = 0; i < depth; ++i)
                {
                    sum += amplitude * noise(p);
                    amplitude *= 0.5f;
                    p *= 2;
                }

                return std::abs(sum);
            }

        private:

            static const int pt_count = 256;
            std::vector<Vec3> ranvec {};
            std::vector<int> permX {}, permY {}, permZ {};

            static void gen_perm(std::vector<int>& p)
            {
                // Fill the vector with the integers from 0 to 255,
                // then shuffle them using a random engine.
                p.resize(pt_count);
                std::iota(p.begin(), p.end(), 0);

                auto engine = std::default_random_engine();
                std::ranges::shuffle(p, engine);
            }

            static float perlin_interp(Vec3 weight[2][2][2], float u, float v, float w)
            {
                float sum = 0.f;

                // Hermite cubics used to smooth the interpolation
                auto uu = u*u*(3 - 2*u);
                auto vv = v*v*(3 - 2*v);
                auto ww = w*w*(3 - 2*w);

                // To understand this formula better, let's first look at
                // bilinear interpolation. Let's say we have a square of
                // values c00, c01, c10, c11 (from bottom to top, left to
                // right), and we want to interpolate a value (x, y) inside
                // the square. To do so, we will first do two linear
                // interpolations; between c00 and c01 nx0 = (1-x)c00 + xc01,
                // and between c10 and c11 nx1 = (1-x)c10 + xc11. This gives
                // us two points nx0 and nx1 along the X-axis-aligned sides
                // of the square; then to get our interpolated point inside
                // the square, we just have to interpolate between nx0 and
                // nx1. This gives us n = (1-y)nx0 + ynx1 = (1-x)(1-u)c00
                // + y(1-x)c01 + x(1-y)c10 + xyc11. The reasoning stays
                // the same when extending to the third dimension, and
                // expanding the for loop below gives exactly the terms
                // we need for our interpolation.
                for (int i = 0; i < 2; ++i)
                {
                    for (int j = 0; j < 2; ++j)
                    {
                        for (int k = 0; k < 2; ++k)
                        {
                            Vec3 dist {u - i, v - j, w - k};
                            sum += (i*uu + (1 - i)*(1 - uu)) *
                                   (j*vv + (1 - j)*(1 - vv)) *
                                   (k*ww + (1 - k)*(1 - ww)) *
                                   dot(weight[i][j][k], dist);
                        }
                    }
                }

                return sum;
            }
    };
}