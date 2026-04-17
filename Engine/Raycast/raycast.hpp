#include <core.hpp>

glm::vec3 screenspace_to_worldspace_raycast(double x_screen_space_pos, double y_screen_space_pos, glm::mat4 view, glm::mat4 projection, u32 WINDOW_WIDTH, u32 WINDOW_HEIGHT) {
    float x = ((2.0f * x_screen_space_pos) / WINDOW_WIDTH) - 1.0f; // normalizing to (-1, 1) from (0, 1)
    float y = 1.0f - ((2.0f * y_screen_space_pos) / WINDOW_HEIGHT);  // normalizing to (-1, 1) from (0, 1) | Also flipping vertically
    float z = 1.0f;
    glm::vec3 ndc = glm::vec3(x, y, z);
    glm::vec4 clip_space = glm::vec4(ndc.x, ndc.y, ndc.z, 1.0f);
    glm::vec4 view_space = glm::inverse(projection) * clip_space;
    view_space = glm::vec4(view_space.x, view_space.y, view_space.z, 0.0f);

    glm::vec4 world_space = glm::inverse(view) * view_space;
    return glm::normalize(glm::vec3(world_space));
}