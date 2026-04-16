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

    void initalize() {
        this->box_shader = OpenGL::Shader::create({"../../Shaders/cube.vert", "../../Shaders/cube.frag"});
        this->model_shader = OpenGL::Shader::create({"../../Shaders/model.vert", "../../Shaders/model.frag"});
    }

    void compile() {
        this->box_shader.compile();
        this->model_shader.compile();
    }
};

TextureTable textures;
ShaderTable shaders;

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

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT); 
    // glEnable(GL_FRAMEBUFFER_SRGB);

    return window;
}

int main(int argc, char** argv) {
    GLFWwindow* window = GLFW_INIT();

    // InputSystem input;
    // input.previous_mouse
    // input.current_mouse
    // glm::vec3 delta = input.current - input.previous;
    shaders.initalize();
    textures.initalize();

    OpenGL::RenderQueue queue;
    OpenGL::Mesh backpack_mesh = OpenGL::Mesh::load_from_file("../../Assets/Models/backpack/backpack.obj"); // OpenGL::Mesh::create(layout, vertices);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1);

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -8.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        for (OpenGL::MeshEntry& entry : backpack_mesh.meshes) {
            OpenGL::RenderCommandOpaque command = {};
            command.shader = &shaders.model_shader;
            command.vao = &backpack_mesh.vao;
            command.entry = &entry;
            command.material = &backpack_mesh.materials[entry.material_index];

            command.model = model;
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
- [] Rename some stuff thats annoying for example VAO and VBO (its a lot of typing...)
- [] RenderState (store like active vao, are you in wireframe mode, are you depth testing and so on)
- [] Start shifting the responsibility on the entity system
- [] Render aabbs for both the main mesh and the submeshes!
- [] REMOVE ALL OPENGL DOGSHIT TYPES just use u32 and be done with it haha
- [] Need to really think about how the hierarchy stuff is gonna owrk witih rendering
    - the reason this is a difficult problem is because you want to use the offset into the VBO
    because thats just much faster you don't have to bind and rebind a different vao every draw call.
    However in order for this to work i need to standardize how the mesh command actually does things.

    1. BIG TRADEOFF Is because htis is a hot function it sucks to have this be called so often 
    when it doesn't do that much work in here, but maybe the overhead of hte actual call instruction is
    neglegable compared to calling into opengl?

    struct MeshComponent : public Component {
        using Component::Component;

        OpenGL::VAO* vao;
        OpenGL::MeshEntry* entry;
        OpenGL::Material* material;
        OpenGL::Shader* shader;
        OpenGL::RenderQueue* queue;

        bool should_render_mesh = true;
        bool render_mesh_wireframe = false;

        // OpenGL::Mesh* aabb_mesh;
        // bool should_render_mesh_aabb = false;

        MeshComponent(Entity* owner, OpenGL::RenderQueue* queue, OpenGL::Mesh* mesh);
        void update(float dt) override;
    };

    void draw() {
        for (RenderCommandOpaque& command : this->commands) {
            Shader* shader = command.shader;
            VAO* vao = command.vao;  // instead of mesh
            Material* material = command.material; // instead of mesh
            MeshEntry* entry = command.entry; // instead of mesh

            // this needs some OpenGL::RenderState so I don't call into the opengl code if I don't have to
            glPolygonMode(GL_FRONT_AND_BACK, command.render_mesh_wireframe ? GL_LINE : GL_FILL);

            vao->bind();
            shader->use();

            glm::mat4 view = active_camera.get_view_matrix()
            glm::mat4 projection = active_camera.get_projection_matrix()

            shader->set_model(command.model);
            shader->set_view(view);
            shader->set_projection(projection);
            if (material) {
                command.shader->set_material(material);
            }
        
            if (entry.index_count > 0) {
                gl_check_error(glDrawElementsBaseVertex(
                    entry.draw_type, entry.index_count, GL_UNSIGNED_INT, 
                    (void*)(sizeof(unsigned int) * entry.base_index), 
                    entry.base_vertex
                ));
            } else {
                gl_check_error(glDrawArrays(
                    entry.draw_type,
                    entry.base_vertex,
                    entry.vertex_count
                ));
            }
        }

        this->commands.clear();
    }

- [] Initialize everything with {} and also default params on the struct members!
- [x] Load models with assimp 
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