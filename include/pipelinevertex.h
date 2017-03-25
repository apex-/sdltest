#ifndef PIPELINEVERTEX_H
#define PIPELINEVERTEX_H

#include "global.h"
#include "vectors.h"
#include <inttypes.h>

class PipelineVertex
{
    public:
        PipelineVertex();
        virtual ~PipelineVertex();

        inline Vector4 ViewSpacePos() { return view_space_pos_; }
        inline void ViewSpacePos(Vector4 view_space_pos) { view_space_pos_ = view_space_pos; }

        inline Vector4 ViewSpaceNormal() { return view_space_normal_; }
        inline void ViewSpaceNormal(Vector4 view_space_normal) { view_space_normal_ = view_space_normal; }

        inline uint8_t IsLit() { return is_lit_; }
        inline void IsLit(uint8_t is_lit) { is_lit_ = is_lit; }

        inline uint8_t ClipFlags() { return clip_flags_; }
        inline void ClipFlags(uint8_t clip_flags) { clip_flags_ = clip_flags; }

        inline uint16_t Wasted() { return wasted_; }
        inline void Wasted(uint16_t wasted) { wasted_ = wasted; }

        inline Vector2 ScreenSpacePos() { return screen_space_pos_; }
        inline void ScreenSpacePos(Vector2 screen_space_pos) { screen_space_pos_ = screen_space_pos; }

        inline uint32_t Color() { return color_; }
        inline void Color(uint32_t color) { color_ = color; }

        inline uint32_t Specular() { return specular_; }
        inline void Specular(uint32_t specular) { specular_ = specular; }

        inline float U() { return u_; }
        inline void U(float u) { u_ = u; }

        inline float V() { return v_; }
        inline void V(float v) { v_ = v; }

        inline void CalcScreenSpacePos() {

            screen_space_pos_.x = ((VIEWPORT_WIDTH -1) * -((view_space_pos_.x/view_space_pos_.w) - 1)) / 2.0f;
            screen_space_pos_.y = ((VIEWPORT_HEIGHT -1) * -((view_space_pos_.y/view_space_pos_.w) - 1)) / 2.0f;
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
