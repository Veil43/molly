#ifndef RENDERTOY_CAMRA_H
#define RENDERTOY_CAMRA_H

#include "rdtmath.h"
#include "types.h"
#include "utils.h"

class Camera {
public:
    static constexpr glm::vec3 kDefaultPosition             = glm::vec3(0.0f);
    static constexpr glm::vec3 kDefaultWorldUp              = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr f32       kDefaultPitch                = 0.0f;
    static constexpr f32       kDefaultYaw                  = -90.0f;
    static constexpr f32       kDefaultMovementSpeed        = 3.0f;  
    static constexpr f32       kDefaultMouseSensitivity     = 50.0f;  
    static constexpr f32       kDefaultVFOV                 = 45.0f;
    static constexpr f32       kDefaultNearDistance         = 0.1f;
    static constexpr f32       kDefaultFarDistance          = 100.0f;
    static constexpr f32       kDefaultAspectRatio          = 1.0f;
    // static constexpr glm::vec3 kDefaultLookAt  = glm::vec3(0.0f, 0.0f, -1.0f);

public:
    glm::vec3 m_position;
    glm::vec3 m_world_up;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_lookat;
    glm::vec3 m_forward;
    
    f32 m_vfov;
    f32 m_near;
    f32 m_far;
    f32 m_aspect_ratio;
    // FPS options
    i32 isFPS = false; // why is this camel case? cause idk still figuring it out.

    f32 m_pitch;
    f32 m_yaw;

    Camera(
        const glm::vec3& position   = kDefaultPosition, 
        const glm::vec3& world_up   = kDefaultWorldUp, 
        f32 pitch                   = kDefaultPitch, 
        f32 yaw                     = kDefaultYaw
    );
    Camera(f32 pos_x, f32 pos_y, f32 pos_z, f32 world_up_x, f32 world_up_y, f32 world_up_z, f32 pitch, f32 yaw);

    Camera(const Camera&);
    Camera& operator=(const Camera&);

    Camera(Camera&&) =delete;
    Camera& operator=(Camera&&) =delete;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    void processMovementInput(rdt::eMovement direction, f32 delta_time);

    void processMouseMovementInput(f32 xoffset, f32 yoffset, f32 delta_time);
    void processMouseScrollInput(f32 yoffset);

    void updateCameraVectors();
};

#endif // RENDERTOY_CAMRA_H