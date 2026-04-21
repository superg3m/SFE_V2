#include <sfe.hpp>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 800;

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

struct Engine {
    GLFWwindow* window;
    OpenGL::RenderQueue queue;
    OpenGL::RenderState render_state;
    InputManager input;
    AssetManager asset_manager;

    Scene active_scene;

    bool init() {
        this->window = GLFW_INIT();
        if (!this->window) {
            return false;
        }

        this->input.init();
        if (!INPUT_GLFW_SETUP(&this->input, this->window)) {
            return false;
        }

        this->asset_manager.init();
        this->render_state.set_depth_test_or_use_cached(true);
        this->queue = OpenGL::RenderQueue::create(&this->render_state);

        this->asset_manager.load_shader("box_shader", {"cube.vert", "cube.frag"});
        this->asset_manager.load_shader("model_shader", {"model.vert", "model.frag"});
        this->asset_manager.load_shader("uniform_shader", {"uniform.vert", "uniform.frag"});

        OpenGL::Shader* uniform_shader = &this->asset_manager.shaders["uniform_shader"];
        this->asset_manager.materials[{"uniform_color_green", 0}] = OpenGL::Material::create(uniform_shader);
        this->asset_manager.materials[{"uniform_color_green", 0}].set_vec3("uColor", 0, 1, 0);

        this->asset_manager.load_texture("awesome_face", 0, "awesomeface.png");
        this->asset_manager.load_texture("container", 1, "container.jpg");

        return true;
    }

    void update(float dt) {
        active_scene.update(dt);

        if (input.get_key(KEY_A, PRESSED|RELEASED)) {
            LOG_ERROR("WOW\n");
        }

        if (input.get_key_pressed(KEY_R)) {
            this->asset_manager.reload_shaders();
        }

        if (input.get_key_pressed(KEY_ESCAPE)) {
            glfwSetWindowShouldClose(this->window, true);
        }

        input.poll();
    }
};

struct Editor {
    bool show_demo_window = false;

    bool init(Engine* engine) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        if (!ImGui_ImplGlfw_InitForOpenGL(engine->window, true)) {
            LOG_ERROR("[IMGUI ERROR]: ImGui_ImplGlfw_InitForOpenGL() Failed\n");
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init("#version 330")) {
            LOG_ERROR("[IMGUI ERROR]: ImGui_ImplOpenGL3_Init(#version 330) Failed\n");
            return false;
        }

        return true;
    }

    void render() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
            ImGui::Begin("Inspector");
                ImGui::Checkbox("Demo Window", &this->show_demo_window);
            ImGui::End();

            if (this->show_demo_window) {
                ImGui::ShowDemoWindow(&this->show_demo_window);
            }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

int main(int argc, char** argv) {
    Engine engine = {};
    if (!engine.init()) {
        return -1;
    }

    Editor editor = {};
    if (!editor.init(&engine)) {
        return -1;
    }

    OpenGL::Mesh backpack_mesh = engine.asset_manager.load_mesh("backpack/backpack.obj", &engine.asset_manager.shaders["model_shader"]);
    OpenGL::Mesh default_aabb_mesh = OpenGL::Mesh::AABB();

    float dt = 0.0f; // Time between current frame and last frame
    float previous_time = 0.0f; // Time of last frame
    while (!glfwWindowShouldClose(engine.window)) {
        gl_error_check(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
        gl_error_check(glClearColor(0.2, 0.2, 0.2, 1));

        float current_time = glfwGetTime(); // Returns time in seconds
        dt = current_time - previous_time;
        previous_time = current_time;

        engine.update(dt);

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        glm::quat rotation = glm::angleAxis((float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = model * glm::mat4_cast(rotation);
        model = glm::scale(model, glm::vec3((sin((float)glfwGetTime()) + 2), 1, 1));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        {
            OpenGL::RenderCommandOpaque command = {};
            command.vao = &default_aabb_mesh.vao;
            command.entry = &default_aabb_mesh.entries[0];
            command.material = &engine.asset_manager.materials[{"uniform_color_green", 0}];
            command.model = model * backpack_mesh.aabb.to_transform_matrix4();
            command.view = view;
            command.projection = projection;
            engine.queue.submit(command);
        }
        for (OpenGL::MeshEntry& entry : backpack_mesh.entries) {
            OpenGL::RenderCommandOpaque command = {};
            command.vao = &backpack_mesh.vao;
            command.entry = &entry;
            command.material = &engine.asset_manager.materials[entry.material_key];
            command.model = model;
            command.view = view;
            command.projection = projection;
            engine.queue.submit(command);

            command = {};
            command.vao = &default_aabb_mesh.vao;
            command.entry = &default_aabb_mesh.entries[0];
            command.material = &engine.asset_manager.materials[{"uniform_color_green", 0}];
            command.model = model * entry.aabb.to_transform_matrix4();
            command.view = view;
            command.projection = projection;
            engine.queue.submit(command);
        }
        
        engine.queue.draw();
        editor.render();

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    return 0;
}

/*
TODO(Jovanni):
- [] change glm to my own math
- [] change containers to my containers
- [] String
- [] switch all this pointer nonsense to handles... for the assets
- [] Look at video and see where I can start abstracting the engine and application.
- [X] Mesh should just have VAO, Entries
- [X] entity manager, should have like all the entities[256], should have:
    std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
- [x] Scene
- [] active camera

- [] probably you can just add a 
    ComponentFlags to an entity this will make serialization trivial.
    Theres also another reason this is nice. I can guarentee the order in which entity 
    data/components will update.
    update.
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


The end goal of this project is the following:
- [] robust rendering system (account for framebuffer objects)
    - [] texture for depth, color, light, normals
        - These shoulnd't be in the shader, theres should just be thier own shader and i just swap out the shader
        that way its easy and doesn't clutter up shaders.

- [] Physics (collisions, shooting)
- [] Multiple Cameras
- [] Animations
- [] Show entity heiarchy
- [] pickable entities (probably ray based)
- [] Hot reloading
- [] Material system thats nice and hotswappable
- [] Scene system with nice lighting abilities (spotlight, sunlight, pointlights)
- [] nice imgui and imguizmo
- [] AS FEW Syscalls during a frame as possible!!!
- [] Profile and code instrumentation in imgui
*/