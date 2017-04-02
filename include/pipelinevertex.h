#ifndef PIPELINEVERTEX_H
#define PIPELINEVERTEX_H

#include "global.h"
#include "vectors.h"
#include <inttypes.h>


enum FrustrumPlanes
{
    kLeftPlaneBit   = 0x01,
    kRightPlaneBit  = 0x02,
    kBottomPlaneBit = 0x04,
    kTopPlaneBit    = 0x08,
    kNearPlaneBit   = 0x10,
    kFarPlaneBit    = 0x20,
};

inline FrustrumPlanes operator|(FrustrumPlanes a, FrustrumPlanes b)
{return static_cast<FrustrumPlanes>(static_cast<int>(a) | static_cast<int>(b));}

//inline FrustrumPlanes& operator=(const FrustrumPlanes& other)
//{return static_cast<FrustrumPlanes>(static_cast<int>(other));}

//template<class T> inline T operator~ (T a) { return (T)~(int)a; }
//template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
//template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
//template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
//template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
//template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
//template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }


//const int kLeftPlane   = 0x01,
//const int kRightPlane  = 0x02,
//const int kBottomPlane = 0x04,
//const int kTopPlane    = 0x08,
//const int kNearPlane   = 0x10,
//const int kFarPlane    = 0x20,


class PipelineVertex
{
    public:
        PipelineVertex();
        virtual ~PipelineVertex();

        inline Vector4& ViewSpacePos() { return view_space_pos_; } const
        inline void ViewSpacePos(Vector4 view_space_pos) { view_space_pos_ = view_space_pos; }
        inline void ViewSpacePos(float x, float y, float z, float w) { view_space_pos_.x=x; view_space_pos_.y=y; view_space_pos_.z=z; view_space_pos_.w=w; }

        inline Vector4& ViewSpaceNormal() { return view_space_normal_; }  const
        inline void ViewSpaceNormal(Vector4 view_space_normal) { view_space_normal_ = view_space_normal; }

        inline uint8_t IsLit() { return is_lit_; }  const
        inline void IsLit(uint8_t is_lit) { is_lit_ = is_lit; }

        inline uint8_t ClipFlags() { return clip_flags_; }  const
        inline void ClipFlags(uint8_t clip_flags) { clip_flags_ = clip_flags; }

        inline uint16_t Wasted() { return wasted_; }  const
        inline void Wasted(uint16_t wasted) { wasted_ = wasted; }

        inline Vector2& ScreenSpacePos() { return screen_space_pos_; }  const
        inline void ScreenSpacePos(Vector2 screen_space_pos) { screen_space_pos_ = screen_space_pos; }

        inline uint32_t Color() { return color_; }  const
        inline void Color(uint32_t color) { color_ = color; }

        inline uint32_t Specular() { return specular_; }  const
        inline void Specular(uint32_t specular) { specular_ = specular; }

        inline float U() { return u_; }  const
        inline void U(float u) { u_ = u; }

        inline float V() { return v_; }  const
        inline void V(float v) { v_ = v; }

        inline void CalcScreenSpacePos() {

            screen_space_pos_.x = ((VIEWPORT_WIDTH -1) * ((view_space_pos_.x / view_space_pos_.w) + 1)) / 2.0f;
            screen_space_pos_.y = ((VIEWPORT_HEIGHT -1) * -((view_space_pos_.y / view_space_pos_.w) - 1)) / 2.0f;
        }

        inline void CalcClipFlags(uint8_t bbclipflags) {

            clip_flags_ = 0;

            if (bbclipflags) { // Check if the bounding-box got clipped
                if (bbclipflags & kLeftPlaneBit) // left plane
                    clip_flags_ |= (view_space_pos_.x <= -view_space_pos_.w) ? kLeftPlaneBit : 0;
                if (bbclipflags & kRightPlaneBit) // right plane
                    clip_flags_ |= (view_space_pos_.x >= view_space_pos_.w) ? kRightPlaneBit : 0;
                if (bbclipflags & kBottomPlaneBit) // bottom plane
                    clip_flags_ |= (view_space_pos_.y <= -view_space_pos_.w) ? kBottomPlaneBit : 0;
                if (bbclipflags & kTopPlaneBit) // top plane
                    clip_flags_ |= (view_space_pos_.y >= view_space_pos_.w) ? kTopPlaneBit : 0;
                if (bbclipflags & kNearPlaneBit) // near plane
                    clip_flags_ |= (view_space_pos_.z >= view_space_pos_.w) ? kNearPlaneBit : 0;
                if (bbclipflags & kFarPlaneBit) // far plane {
                    clip_flags_ |= (view_space_pos_.z <= -view_space_pos_.w) ? kFarPlaneBit : 0;
            }
        }

    private:
        // 60 bytes
        Vector4 view_space_pos_; // 16
        Vector4 view_space_normal_; // 16
        uint8_t is_lit_; // 1
        uint8_t clip_flags_; //1
        uint16_t wasted_; // 2

        Vector2 screen_space_pos_; // 8
        uint32_t color_; // 4
        uint32_t specular_; // 4
        float u_, v_; // 8
};

#endif // PIPELINEVERTEX_H
