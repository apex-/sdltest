#ifndef PLANE_H
#define PLANE_H

#include "vectors.h"

class Plane
{
    public:
        Plane() {};
        Plane(float a, float b, float c, float d) : a_(a), b_(b), c_(c), d_(d) {};
        virtual ~Plane();

        float a() const { return a_; }
        void a(float a) { a_ = a; }
        float b() const { return b_; }
        void b(float b) { b_ = b; }
        float c() const { return c_; }
        void c(float c) { c_ = c; }
        float d() const { return d_; }
        void d(float d) { d_ = d; }

        void set(float a, float b, float c, float d) {a_=a, b_=b, c_=c, d_=d; };

        /** Dot product, gives the distnace from the plane to a point */
        float dot(Vector3 vec) const;

        /**
         * Dot product, gives the distnace from the plane to a point
         * Note that this implementation assumes a w component of 1.0
         */
        float dot(Vector4 vec) const;

        /**
         * Normalizes the plane coefficients
         */
        void normalize();

    private:
        float a_;
        float b_;
        float c_;
        float d_;
};

#endif // PLANE_H
