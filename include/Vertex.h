#ifndef VERTEX_H
#define VERTEX_H
#include <stdint.h>

class Vertex
{
    public:
        Vertex();
        Vertex(Vertex &rhs);
        Vertex(float x, float y, float z);
        virtual ~Vertex();

        void print();

        float x;
        float y;
        float z;

        uint32_t argb;

    protected:

    private:

};

#endif // VERTEX_H
