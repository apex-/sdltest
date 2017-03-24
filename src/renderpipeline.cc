#include "renderpipeline.h"

RenderPipeline::RenderPipeline()
{
    //ctor
}

RenderPipeline::~RenderPipeline()
{
    //dtor
}


void RenderPipeline::Draw(TlcInstance &tlcinstance) {

    TLCPrimitive* primitive = tlcinstance.GetTlcPrimitive();
    Matrix4 vpm = camera_.viewProjectionMatrix();
    Matrix4 tm = tlcinstance.GetTransform().getTransformation();
    Matrix4 mvpm = vpm * tm;

    if (primitive->isBoxInsideFrustrum(mvpm)) {

            vector<Vertex> vertices = primitive->getVertexArray();
            for (uint32_t i=0; i<primitive->getNumberOfVertices(); i++) {

                // Transform Vertex from model-space to view space
                vb_[i].ViewSpacePos(mvpm * vertices[i].Pos());


                // Calculate the screen position
                vb_[i].CalcScreenSpacePos();
            }


            // Clipping


            // Perspective Divide

            // Screen space transform

            // Triangle setup

            // Rasterize

//
//           for (uint32_t i=0; i<primitive.getNumberOfIndices(); i+=3) {
//
//                Vertex v1pin = primitive.getVertexArray()[primitive.getIndices()[i]];
//                Vertex v2pin = primitive.getVertexArray()[primitive.getIndices()[i+1]];
//                Vertex v3pin = primitive.getVertexArray()[primitive.getIndices()[i+2]];
//
//                Vertex v1p(v1pin);
//                Vertex v2p(v2pin);
//                Vertex v3p(v3pin);
//
//                v1p.m_pos = mvPrimitive * v1pin.m_pos;
//                v2p.m_pos = mvPrimitive * v2pin.m_pos;
//                v3p.m_pos = mvPrimitive * v3pin.m_pos;
//
//                v1p.perspectiveDivide();
//                v2p.perspectiveDivide();
//                v3p.perspectiveDivide();
//
//                v1p.toScreenCoordinates();
//                v2p.toScreenCoordinates();
//                v3p.toScreenCoordinates();
//
//                rasterizer.rasterize(v1p, v2p, v3p);
//            }

    }

}
