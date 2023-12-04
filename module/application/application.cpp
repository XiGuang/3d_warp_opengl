//
// Created by Admin on 2023/11/27.
//

#include <glad/glad.h>
#include "application.h"
#include <format>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <glm/glm.hpp>
#include "../stb_image/stb_image.h"

DEFINE_bool(img_flip, false, "flip image vertically");
DEFINE_bool(polygon_mode, false, "polygon mode");
DEFINE_bool(control_camera, false, "enable to control camera");
DEFINE_bool(is_warp, false, "enable to warp");
DEFINE_int32(threads_num, 4, "threads num");

void
Application::loadModels(const std::filesystem::path &dict_path, int start_index_1, int end_index_1, int start_index_2,
                        int end_index_2, int threads_num) {
    std::vector<std::filesystem::path> model_paths;
    std::vector<std::string> model_names;
    for (int i = start_index_1; i <= end_index_1; i++) {
        for (int j = start_index_2; j <= end_index_2; j++) {
            std::filesystem::path file_path(dict_path);
            std::string model_name = std::format("Tile_+{:03d}_+{:03d}.obj", i, j);
            file_path.append(std::format("Tile_+{:03d}_+{:03d}", i, j));
            model_paths.emplace_back(file_path / model_name);
            model_names.emplace_back(model_name);
        }
    }
    _model_manager.loadModels(model_paths, model_names, threads_num);
}

bool Application::init() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DEPTH_BITS, 32);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    _window = glfwCreateWindow(_config["viewport"]["width"].as<int>(), _config["viewport"]["height"].as<int>(), "LearnOpenGL", nullptr, nullptr);
    if (_window == nullptr) {
        LOG(ERROR) << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(_window, this);

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, _framebuffer_size_callback);
    if (FLAGS_control_camera) {
        glfwSetCursorPosCallback(_window, _mouse_callback);
    }

    // tell GLFW to capture our mouse
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        LOG(ERROR) << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(FLAGS_img_flip);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // draw in wireframe
    if (FLAGS_polygon_mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return true;
}

Application::Application(const std::filesystem::path &config_path) {
    _config = YAML::LoadFile(config_path.string());
    init();
    _warper = Warper(_config["viewport"]["width"].as<int>(), _config["viewport"]["height"].as<int>(),
                     _config["shader"]["warper_vertex_shader_path"].as<std::string>(),
                     _config["shader"]["warper_fragment_shader_path"].as<std::string>());
    _shader = Shader(_config["shader"]["vertex_shader_path"].as<std::string>(),
                     _config["shader"]["fragment_shader_path"].as<std::string>());
    _model_manager = ModelManager(_config["load_model"]["default_texture_path"].as<std::string>(),
                                  _config["load_model"]["default_texture_name"].as<std::string>());
    loadModels(_config["load_model"]["dict_path"].as<std::string>(), _config["load_model"]["start_index_1"].as<int>(),
               _config["load_model"]["end_index_1"].as<int>(), _config["load_model"]["start_index_2"].as<int>(),
               _config["load_model"]["end_index_2"].as<int>(), FLAGS_threads_num);
//    loadModel("../model/test/test.obj","test");
}

void Application::_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    auto *application = (Application *) glfwGetWindowUserPointer(window);
    application->_warper.setViewport(Eigen::Vector4d(0, 0, width, height));
}

void Application::_mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    auto *application = (Application *) glfwGetWindowUserPointer(window);
    if (application->_first_mouse) {
        application->_last_x = (float)xpos;
        application->_last_y = (float)ypos;
        application->_first_mouse = false;
    }

    float xoffset = application->_last_x - (float)xpos;
    float yoffset = application->_last_y - (float)ypos; // reversed since y-coordinates go from bottom to top

    application->_last_x = (float)xpos;
    application->_last_y = (float)ypos;

    application->_warper.getCamera1().processMouseMovement(xoffset, yoffset);
}

void Application::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (!FLAGS_control_camera) {
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(FORWARD, _delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(BACKWARD, _delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(LEFT, _delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(RIGHT, _delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(ROTATE_LEFT, _delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        _warper.getCamera1().processKeyboard(ROTATE_RIGHT, _delta_time);
}

bool Application::run() {
    updateCamera();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(_window)) {
        // per-frame time logic
        auto currentFrame = static_cast<float>(glfwGetTime());
        _delta_time = currentFrame - _last_frame;
        _last_frame = currentFrame;

        // input
        processInput(_window);

        // 进行映射
        if (FLAGS_is_warp) {
            _warper.warp(_model_manager, FLAGS_threads_num);
        }

        // clear
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        _shader.use();

        // view/projection transformations
        glm::mat4 projection = _warper.getCamera1().getProjectionMatrix();
        glm::mat4 view = _warper.getCamera1().getViewMatrix();
        _shader.setMat4("projection", projection);
        _shader.setMat4("view", view);

        // render the loaded model
        _model_manager.drawAllModel(_shader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    return true;
}

void Application::updateCamera() {
    glm::vec3 position1(492786.14298105857, 2493744.2364600948, 215.55511619430035),
            position2(492782.77934889164, 2493743.0074915341, 224.51109054777771),
            position3(492753.13358036103,2493743.0265364042,225.53376364521682),
            front(0, 0, 1),
            up(0, -1, 0),
            origin_point1(493089.5233, 2493512.718, 188.1),
            origin_point2(493089.5233, 2493512.718, 188.1),
            fix_point2{0,-4,-15};
    // 492844.5,2493816.5, 110.5

    glm::mat3 rotation1(-0.056613499472061943, 0.99684242183571214, 0.055678521049320473,
                        0.97378912078740676, 0.042827127455479772, 0.22338438931581872,
                        0.22029448452836028, 0.066865710065958789, -0.97313889908057793),
            rotation2(0.99997687614017161, 0.0039067723412189934, -0.0055663556136665649,
                      0.0032042275201267872, -0.99263997685343375, -0.121060353867041,
                      -0.0059983423496136612, 0.1210397186145432, -0.99262954137319981),
            rotation3(0.99999041731692573,0.0029595783743092,-0.0032258596013641262,
                      0.0024512640785848291,-0.9890532102742069,-0.14753893909983007,
                      -0.0036271998481517409,0.14752961784717283,-0.98905098719890572);
    float fov(2*std::atan((23.5/_warper.getWidth()*_warper.getHeight())/(2*25.500749588012695))),aspect((float)_warper.getWidth()/(float)_warper.getHeight());

    NewCamera camera1(position1 - origin_point1, front, up, rotation1,fov,aspect,_far,_near),
            camera2(position2 - origin_point2, front, up, rotation2,fov,aspect,_far,_near),
            camera3(position3-origin_point2,front,up,rotation3,fov,aspect,_far,_near),
            test_camera({0,0,3},front,up,rotation1,fov,aspect,_far,_near);
    _warper.update(camera3, camera1);
}

void Application::loadModel(const std::filesystem::path &path, const std::string &name) {
    _model_manager.loadModel(name,path.string());
}
