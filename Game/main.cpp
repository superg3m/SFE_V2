#include <sfe.hpp>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 800;

struct TextureTable {
    OpenGL::Texture face;
    OpenGL::Texture container;

    void initalize() {
        this->face = OpenGL::Texture::load_from_file(1, "../../Assets/Textures/awesomeface.png");
        this->container = OpenGL::Texture::load_from_file(1, "../../Assets/Textures/container.jpg");
    }
};

struct ShaderTable {
    OpenGL::Shader box_shader;
    OpenGL::Shader model_shader;
    OpenGL::Shader color_uniform_shader;

    void initalize() {
        this->box_shader = OpenGL::Shader::create({"../../Assets/Shaders/cube.vert", "../../Assets/Shaders/cube.frag"});
        this->model_shader = OpenGL::Shader::create({"../../Assets/Shaders/model.vert", "../../Assets/Shaders/model.frag"});
        this->color_uniform_shader = OpenGL::Shader::create({"../../Assets/Shaders/uniform.vert", "../../Assets/Shaders/uniform.frag"});
    }

    void compile() {
        this->box_shader.compile();
        this->model_shader.compile();
    }
};


TextureTable textures;
ShaderTable shaders;

struct MaterialTable {
    OpenGL::Material uniform_color_green;
    OpenGL::Material uniform_color_red;
    void initalize() {
        this->uniform_color_green = OpenGL::Material::create(&shaders.color_uniform_shader);
        this->uniform_color_green.set_vec3("uColor", 0, 1, 0);

        this->uniform_color_red = OpenGL::Material::create(&shaders.color_uniform_shader);
        this->uniform_color_red.set_vec3("uColor", 1, 0, 0);
    }
};

MaterialTable materials;

GLFWwindow* GLFW_INIT() {
    RUNTIME_ASSERT_MSG(glfwInit(), "Failed to init glfw\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window\n");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD\n");
        glfwTerminate();
        exit(-1);
    }

    glfwSwapInterval(1);
    // glfwSetInputMode(window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    gl_error_check(glEnable(GL_MULTISAMPLE));
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT); 
    // glEnable(GL_FRAMEBUFFER_SRGB);

    return window;
}

int main(int argc, char** argv) {
    GLFWwindow* window = GLFW_INIT();

    OpenGL::RenderState render_state = {};
    render_state.set_depth_test_or_use_cached(true);

    // InputSystem input;
    // input.previous_mouse
    // input.current_mouse
    // glm::vec3 delta = input.current - input.previous;
    shaders.initalize();
    textures.initalize();
    materials.initalize();

    OpenGL::RenderQueue queue = OpenGL::RenderQueue::create(&render_state);
    OpenGL::Mesh backpack_mesh = OpenGL::Mesh::load_from_file(&shaders.model_shader, "../../Assets/Models/backpack/backpack.obj");
    // OpenGL::Mesh cube_mesh = OpenGL::Mesh::cube();

    OpenGL::Mesh default_aabb_mesh = OpenGL::Mesh::AABB();
    // OpenGL::Mesh backpack_aabb_mesh = OpenGL::Mesh::AABB(backpack_mesh.aabb);

    while (!glfwWindowShouldClose(window)) {
        gl_error_check(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
        gl_error_check(glClearColor(0.2, 0.2, 0.2, 1));

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        // glm::quat rotation = glm::angleAxis((float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        // model = model * glm::mat4_cast(rotation);
        model = glm::scale(model, glm::vec3((sin((float)glfwGetTime()) + 2), 1, 1));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        {
            OpenGL::RenderCommandOpaque command = {};
            command.vao = &default_aabb_mesh.vao;
            command.entry = &default_aabb_mesh.meshes[0];
            command.material = &materials.uniform_color_green;
            command.model = model * backpack_mesh.aabb.to_transform_matrix4();
            command.view = view;
            command.projection = projection;
            queue.submit(command);
        }
        for (OpenGL::MeshEntry& entry : backpack_mesh.meshes) {
            OpenGL::RenderCommandOpaque command = {};
            command.vao = &backpack_mesh.vao;
            command.entry = &entry;
            command.material = &backpack_mesh.materials[entry.material_index];
            command.model = model;
            command.view = view;
            command.projection = projection;
            queue.submit(command);

            command = {};
            command.vao = &default_aabb_mesh.vao;
            command.entry = &default_aabb_mesh.meshes[0];
            command.material = &materials.uniform_color_green;
            command.model = model * entry.aabb.to_transform_matrix4();
            command.view = view;
            command.projection = projection;
            queue.submit(command);
        }
        
        queue.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

/*
TODO(Jovanni):
- [] entity manager, should have like all the entities[256], should have:
- [] Scene
- [] active camera
- [] maybe get materials out of Mesh and have it just be a global table
    - THIS WILL BE THE KEY TO TEH HASHMAP: "../../Assets/Models/backpack/material/" + i
    material_index = g_materials.size() + i
- [] imgui, imguizmo
- [] Render aabbs for both the main mesh and the submeshes! (This is a good question!)
- [] entity manager, should have like all the entities[256], should have:
    Entity* create_entity() // give you a return &entites[next++]
    // realisitically this hsould give you a handle but one thing at a time batman
    std::vector<Entity*> QueryComponentList<MeshComponent>() 
    std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
- [] Start shifting the responsibility on the entity system
- [] Entity Picking
- [] Need to really think about how the hierarchy stuff is gonna owrk witih rendering
    - the reason this is a difficult problem is because you want to use the offset into the VBO
    because thats just much faster you don't have to bind and rebind a different vao every draw call.
    However in order for this to work i need to standardize how the mesh command actually does things.

- [] Initialize everything with {} and also default params on the struct members!
- [] load animations with assimp

SkyBoxComponent 
OpenGL::RenderCommand command = {};
command.shader = &shaders.model_shader;
command.mesh = &backpack_mesh;
command.model = identity;
command.view = glm::mat4(glm::mat3(view)); // remove translation
command.projection = projection;

SRT : THIS IS IMPORTANT!!!
Column vector | (GLM): T * R * S	| S → R → T
Row vector    | (GM):  S * R * T	| S → R → T

// so you got to specify the shader, mat, and the mesh for a mesh component
new MeshComponent(shader, mat, mesh)

- [] Remove concept of model an made it so that you Entity* e = Entity::load_model(path);
    - [] heirarchy of meshes


- [] Entitiy stuff!
    - Use an id for the color picking convert the id into a color
    - entities should be stored in a static array for now
    - if you want to make an entity you need to ask the engine->create_entity()
         this will set the id/index, and other tsuff and return to you a pointer to an Entity

    - To ensure some dederminism Entities need to update their children
    - Components will just be a vector or hashmap on the actutal entity
*/