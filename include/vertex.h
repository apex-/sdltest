#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <stdint.h>

#include "global.h"
#include "vectors.h"
#include "matrices.h"

class Vertex
{
    public:
        Vertex() { };
        Vertex(const Vertex &rhs);
        Vertex(float x, float y, float z);

        inline Vector4 Pos() { return pos_; }
        inline void Pos(Vector4 pos) { pos_ = pos; }
        inline void Pos(float x, float y, float z) { pos_.x = x; pos_.y = y; pos_.z = z; }

        inline Vector4 Normal() { return normal_; }
        inline void Normal(Vector4 normal) { normal_; }
        inline void Normal(float x, float y, float z) { normal_.x = x; normal_.y = y; normal_.z = z; }

        inline Vector2 TexCoord() { return texcoord_; }
        inline void TexCoord(Vector2 texcoord) { texcoord_ = texcoord; }
        inline void TexCoord(float u, float v) { texcoord_.x = u; texcoord_.y = v; }

        inline uint32_t Color() { return color_; }
        inline uint32_t Color(uint32_t color ) { color_ = color; }

        virtual ~Vertex();

        friend std::ostream& operator<<(std::ostream& os, const Vertex& m);

    private:
        Vector4 pos_;
        Vector4 normal_;
        Vector2 texcoord_;
        uint32_t color_;

};

#endif // VERTEX_H
