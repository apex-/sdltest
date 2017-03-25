#ifndef TLCINSTANCE_H
#define TLCINSTANCE_H

#include "transform.h"
#include "tlcprimitive.h"

class TlcInstance
{
    public:

        TlcInstance(TlcPrimitive *tlcprimitive) { tlcprimitive_ = tlcprimitive; }

        TlcInstance();
        Transform& GetTransform() { return transform_; }
        void SetTransform(Transform transform) { transform_ = transform; }
        TlcPrimitive* GetTlcPrimitive() { return tlcprimitive_; }
        virtual ~TlcInstance();

    private:
        TlcPrimitive *tlcprimitive_;
        Transform transform_;
};

#endif // TLCINSTANCE_H
