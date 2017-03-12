#include "TLCPrimitive.h"

#include <vector>

TLCPrimitive::TLCPrimitive()
{
    //ctor
}

TLCPrimitive::~TLCPrimitive()
{
    //dtor
}

TLCPrimitive::TLCPrimitive(const TLCPrimitive& other)
{
    //copy ctor
}

bool TLCPrimitive::loadFromFile(const char *filename)
{
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, NULL, true);

    if (result) {
        cout << "success" << endl;
    } else {
        cout << "failed loading obj file" << endl;
    }


//    for (int i=0; i<attrib.vertices.size(); i+=3) {
//
//        //shapes[0].mesh
//
//        float x = attrib.vertices[i];
//        float y = attrib.vertices[i+1];
//        float z = attrib.vertices[i+2];
//
//        Vertex v(x,y,z);
//        vertexArray.push_back(v);
//    }

    int j = 0;
    for (int i=0; i<shapes[0].mesh.indices.size(); i+=3) {

        //shapes[0].mesh
        tinyobj::index_t xi = shapes[0].mesh.indices[i];
        tinyobj::index_t yi = shapes[0].mesh.indices[i+1];
        tinyobj::index_t zi = shapes[0].mesh.indices[i+2];

        float x = attrib.vertices[xi.vertex_index];
        float y = attrib.vertices[yi.vertex_index];
        float z = attrib.vertices[zi.vertex_index];

        Vertex v(x,y,z);

        //cout << " xi:" << xi.vertex_index << " yi:" << yi.vertex_index << " zi:" << zi.vertex_index << endl;
        //cout << " x:" << x << " y:" << y << " z:" << z << endl;
       // cout << endl;

        vertexArray.push_back(v);
        indices.push_back(j);
        j++;

        //indices.push_back(xi.vertex_index);
        //indices.push_back(yi.vertex_index);
        //indices.push_back(zi.vertex_index);
    }


    return result;
/// Loads .obj from a file.
/// 'attrib', 'shapes' and 'materials' will be filled with parsed shape data
/// 'shapes' will be filled with parsed shape data
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`
/// 'mtl_basedir' is optional, and used for base directory for .mtl file.
/// In default(`NULL'), .mtl file is searched from an application's working
/// directory.
/// 'triangulate' is optional, and used whether triangulate polygon face in .obj
/// or not.
//bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
//             std::vector<material_t> *materials, std::string *err,
//             const char *filename, const char *mtl_basedir = NULL,
//             bool triangulate = true);

}


uint32_t TLCPrimitive::getNumberOfVertices()
{
    return vertexArray.size();
}


vector<Vertex>& TLCPrimitive::getVertexArray()
{
    return vertexArray;
}


vector<uint32_t>& TLCPrimitive::getIndices()
{
    return indices;
}


uint32_t TLCPrimitive::getNumberOfIndices()
{
    return indices.size();
}


Transform& TLCPrimitive::getModelWorldTransform()
{
    return modelWorldTransform;
}


VIEWSTATUS TLCPrimitive::isInsideFrustrum(Camera& camera) {

    Matrix4 vp = camera.getViewProjection();
    Matrix4 mvp = vp * modelWorldTransform.getTransformation();

    Vector4 mvpbb0 = mvp * aabb[0];
    Vector4 mvpbb1 = mvp * aabb[1];

    return inside;

}
