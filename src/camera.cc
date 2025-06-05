#include "camera.h"
#include "utils.h"

Camera::Camera(const glm::vec3& position, f32 pitch, f32 yaw, const glm::vec3& world_up)
    : m_position{position},
      m_world_up{world_up},
      m_pitch{pitch},
      m_yaw{yaw},
      
      m_vfov{kDefaultVFOV},
      m_near{kDefaultNearDistance},
      m_far{kDefaultFarDistance},
      m_aspect_ratio{kDefaultAspectRatio}
{
    this->update_camera_vectors();
}

Camera::Camera(f32 pos_x, f32 pos_y, f32 pos_z, f32 world_up_z, f32 pitch, f32 yaw, f32 world_up_x, f32 world_up_y) 
    : m_position{pos_x, pos_y, pos_z}, 
      m_world_up{world_up_x, world_up_y, world_up_z}, 
      m_pitch{pitch}, 
      m_yaw{yaw},
      m_vfov{kDefaultVFOV},
      m_near{kDefaultNearDistance},
      m_far{kDefaultFarDistance},
      m_aspect_ratio{kDefaultAspectRatio}
{
    this->update_camera_vectors();
}

Camera::Camera(const Camera& other) 
    : m_position{other.m_position},
      m_world_up{other.m_world_up},
      m_pitch{other.m_pitch},
      m_yaw{other.m_yaw},
      m_vfov{other.m_vfov},
      m_near{other.m_near},
      m_far{other.m_far},
      m_aspect_ratio{other.m_aspect_ratio}
{
    this->update_camera_vectors();
}

Camera& Camera::operator=(const Camera& other) {
    m_position = other.m_position;
    m_world_up = other.m_world_up;
    m_pitch = other.m_pitch;
    m_yaw = other.m_yaw;
    m_yaw = other.m_yaw;
    m_vfov = other.m_vfov;
    m_near = other.m_near;
    m_far = other.m_far;
    m_aspect_ratio = other.m_aspect_ratio;

    this->update_camera_vectors();
    return *this;
}

glm::mat4 Camera::get_view_matrix() const {
    /*
        Why pos + lookat?
        Because that is the POINT we are lookat not the direction
    */
    return glm::lookAt(this->m_position, this->m_position + this->m_target, this->m_up);
}

glm::mat4 Camera::get_projection_matrix() const {
    return glm::perspective(glm::radians(this->m_vfov), this->m_aspect_ratio, this->m_near, this->m_far);
}


void Camera::process_movement_input(utils::eMovement direction, f32 delta_time) {
    switch (direction) {
        case utils::eMovement::kForward : {
            this->m_position += this->m_forward * this->m_movement_speed * delta_time;
        } break;
        
        case utils::eMovement::kBackward : {
            this->m_position -= this->m_forward * this->m_movement_speed * delta_time;
        } break;
        
        case utils::eMovement::kLeft : {
            this->m_position -= this->m_right * this->m_movement_speed * delta_time;
        } break;
        
        case utils::eMovement::kRight : {
            this->m_position += this->m_right * this->m_movement_speed * delta_time;
        } break;

        default : {

        } break;
    }

    this->update_camera_vectors();
}

void Camera::process_mouse_movement_input(f32 xoffset, f32 yoffset, f32 delta_time) {
    xoffset *= kDefaultMouseSensitivity * delta_time;
    yoffset *= kDefaultMouseSensitivity * delta_time;

    this->m_yaw += xoffset;
    this->m_pitch += yoffset;

    if (this->m_pitch >= 89.9f) {
        this->m_pitch = 89.9f;
    } else if (this->m_pitch <= -89.9f) {
        this->m_pitch = -89.9f;
    }

    this->update_camera_vectors();
}

void Camera::process_mouse_scroll_input(f32 yoffset) {
    this->m_vfov -= yoffset;

    if (this->m_vfov < 1.0f) {
        this->m_vfov = 1.0f;
    }
    if (this->m_vfov > 45.0f) {
        this->m_vfov = 45.0f;
    } 
}

void Camera::update_camera_vectors() {
    glm::vec3 new_target;
    new_target.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    new_target.y = sin(glm::radians(this->m_pitch));
    new_target.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    this->m_target = glm::normalize(new_target);
    this->m_forward = this->m_target;

    if (isFPS) {
        this->m_forward.y = 0;
    }

    this->m_right = glm::normalize(glm::cross(this->m_forward, this->m_world_up));
    this->m_up = glm::normalize(glm::cross(this->m_right, this->m_forward));
}
