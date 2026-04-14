#include <sfe.hpp>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 800;

struct ShaderTable {
    OpenGL::Shader box_shader;

    void initalize() {
        this->box_shader = OpenGL::Shader::create({"../../Shaders/cube.vert", "../../Shaders/cube.frag"});
    }

    void compile() {
        this->box_shader.compile();
    }
};

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

    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    OpenGL::VertexLayout layout = OpenGL::VertexLayout::create({
        OpenGL::VertexElement{0, OpenGL::BufferStrideTypeInfo::VEC3},
        OpenGL::VertexElement{sizeof(glm::vec3), OpenGL::BufferStrideTypeInfo::VEC2}
    });

    OpenGL::DrawData draw_data = OpenGL::DrawData::create(layout, vertices);
    OpenGL::VertexArrayObject vao = OpenGL::VertexArrayObject::create();
    OpenGL::VertexBufferObject vbo = OpenGL::VertexBufferObject::allocate(vao, layout, vertices);
    OpenGL::Texture container_texture = OpenGL::Texture::load_from_file(0, "../../Assets/Textures/container.jpg");
    OpenGL::Texture face_texture = OpenGL::Texture::load_from_file(1, "../../Assets/Textures/awesomeface.png");

    shaders.initalize();

    // OpenGL::Mesh cube = OpenGL::Mesh::cube();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1);

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        shaders.box_shader.use();
        shaders.box_shader.set_model(model);
        shaders.box_shader.set_view(view);
        shaders.box_shader.set_projection(projection);
        shaders.box_shader.set_texture("container", container_texture);
        shaders.box_shader.set_texture("face", face_texture);
        OpenGL::render_vao(vao, draw_data);

        // OpenGL::render_model(cube_model, &shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

/*
TODO(Jovanni): 
- [] probably I shouldn't even allow you to create a VBO if you don't have a VAO (since it doesn't work on mac)
- [] RenderQueue is really smart, every frame an entity with the mesh component will submit itself to the render queue.
struct RenderCommand {
    Shader* shader;
    Mesh* mesh;
    Material* material; // pointer only to avoid copying
    glm::mat4 model;
};

- [] ShaderTable

struct ShaderTable {
    Shader default_shader;
    Shader standard_shader; 
    Shader pbr_shader; 
    Shader frasnel_shader; 

    // ui shaders
    Shader ui_shader;
    // ...
}

// so you got to specify the shader, mat, and the mesh for a mesh component
new MeshComponent(shader, mat, mesh)

- [] Remove concept of model an made it so that you Entity* e = load_model(path);
    - [] heirarchy of meshes


*/