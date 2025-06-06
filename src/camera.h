#ifndef MOLLY_CAMERA_H
#define MOLLY_CAMERA_H

#include "molly_math.h"
#include "types.h"
#include "input.h"

class Camera {
public:
    static constexpr glm::vec3 kDefaultPosition             = glm::vec3(0.0f);
    static constexpr glm::vec3 kDefaultWorldUp              = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr f32       kDefaultPitch                = 0.0f;
    static constexpr f32       kDefaultYaw                  = -90.0f;
    static constexpr f32       kDefaultMovementSpeed        = 1.0f;  
    static constexpr f32       kDefaultMouseSensitivity     = 50.0f;  
    static constexpr f32       kDefaultVFOV                 = 45.0f;
    static constexpr f32       kDefaultNearDistance         = 0.1f;
    static constexpr f32       kDefaultFarDistance          = 100.0f;
    static constexpr f32       kDefaultAspectRatio          = 1.0f;

public:
    glm::vec3 m_position;
    glm::vec3 m_world_up;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_target;
    glm::vec3 m_forward;
    
    f32 m_vfov;
    f32 m_near;
    f32 m_far;
    f32 m_aspect_ratio;
    // FPS options
    i32 isFPS = false; // why is this camel case? cause idk still figuring it out.

    f32 m_pitch;
    f32 m_yaw;

    f32 m_movement_speed = kDefaultMovementSpeed;

    Camera(
        const glm::vec3& position   = kDefaultPosition, 
        f32 pitch                   = kDefaultPitch, 
        f32 yaw                     = kDefaultYaw,
        const glm::vec3& world_up   = kDefaultWorldUp
    );
    Camera(f32 pos_x, f32 pos_y, f32 pos_z, f32 world_up_z, f32 pitch, f32 yaw, f32 world_up_x, f32 world_up_y);

    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;
    void process_movement_input(eMovement direction, f32 delta_time);

    void process_mouse_movement_input(f32 xoffset, f32 yoffset, f32 delta_time);
    void process_mouse_scroll_input(f32 yoffset);

    void update_camera_vectors();
};

#endif // MOLLY_CAMERA_H