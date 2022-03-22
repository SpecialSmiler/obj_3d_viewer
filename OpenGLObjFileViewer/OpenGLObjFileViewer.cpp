#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader_s.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <filesystem>

void key_callback(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int whiteTextureId;
unsigned int matcapTextureId;


enum class ViewportShaderMode
{
	Lit,
	MatCap,
	Wireframe,
};

glm::vec4 bgColor(0.1f, 0.1f, 0.1f, 1.0f);

bool isUserActing = false;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

ViewportShaderMode currentViewportShader = ViewportShaderMode::Lit;

int main(int argc, char* argv[])
{
	std::cout << "Hello World!" << std::endl;
	std::cout << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::cout << "arg[" << i << "]: " << argv[i] << std::endl;
	}

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Renderer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad
	// ------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::string cwd = std::filesystem::current_path().string();
	// scene init
	string objPath = cwd + "\\Resource\\Monkey_smooth.obj";
	//string objPath = "F:\\Model\\Sample\\新建文件夹\\Monkey.obj";
	//string objPath = "F:\\Model\\Sample\\temp\\Monkey.obj";
	//string objPath = "F:\\Model\\Sample\\SphereCube.obj";
	//string objPath = "F:\\Model\\Sample\\Cylinder.obj";
	//string objPath = "F:\\Model\\Sample\\Monkey_sub.obj";
	//string objPath = "F:\\Model\\Sketchfab\\survival-guitar-backpack-low-poly\\backpack\\backpack.obj";

	if (argv[1])
	{
		objPath = std::string(argv[1]);
	}
	Model objModel(objPath);

	//Matcap texture
	std::string whiteTexturePath = "\\Resource\\solid_diffuse.png";
	std::string matcapFilePath = "\\Resource\\matcap_metal.png";
	whiteTextureId = TextureFromFile(cwd + whiteTexturePath);
	matcapTextureId = TextureFromFile(cwd + matcapFilePath);


	// Shader
	std::string litVertShader = cwd + "\\GLSL\\Lit.vert";
	std::string litFragShader = cwd + "\\GLSL\\Lit.frag";
	std::string UnlitVertShader = cwd + "\\GLSL\\Unlit.vert";
	std::string UnlitFragShader = cwd + "\\GLSL\\Unlit.frag";
	std::string MatcapVertShader = cwd + "\\GLSL\\Matcap.vert";
	std::string MatcapFragShader = cwd + "\\GLSL\\Matcap.frag";
	std::cout << cwd << endl;
	Shader litShader(litVertShader.c_str(), litFragShader.c_str());
	Shader unlitShader(UnlitVertShader.c_str(), UnlitFragShader.c_str());
	Shader matcapShader(MatcapVertShader.c_str(), MatcapFragShader.c_str());
	Shader wireframeShader(UnlitVertShader.c_str(), UnlitFragShader.c_str());

	// pre-render setting
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	//glDisable(GL_POLYGON_OFFSET_FILL);
	//glEnable(GL_POLYGON_OFFSET_LINE); // doesn't work for GL_LINES, only work for GL_LINE
	//glPolygonOffset(1.0f, 1. 0f);
	//glDisable(GL_POLYGON_OFFSET_LINE);
	// render 
	// ------------------------------

	isUserActing = true;
	while (!glfwWindowShouldClose(window))
	{

		key_callback(window);

		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //参数应该是bitmap

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		if (currentViewportShader == ViewportShaderMode::Lit)
		{
			litShader.use();
			litShader.setMat4("model", model);
			litShader.setMat4("view", view);
			litShader.setMat4("projection", projection);
			//Light setting
			litShader.setVec3("viewPos", camera.CameraPos);
			litShader.setFloat("material.shininess", 32.0f);
			litShader.setVec3("dirLight.direction", 1.0f, -0.7f, -2.0f);
			litShader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
			litShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
			litShader.setVec3("dirLight.specular", 0.6f, 0.6f, 0.6f);
			objModel.Draw(litShader);
		}
		else if (currentViewportShader == ViewportShaderMode::MatCap)
		{
			matcapShader.use();
			matcapShader.setMat4("model", model);
			matcapShader.setMat4("view", view);
			matcapShader.setMat4("projection", projection);
			objModel.DrawMatcap(matcapShader, matcapTextureId);
		}
		else if (currentViewportShader == ViewportShaderMode::Wireframe)
		{
			unlitShader.use();
			unlitShader.setMat4("model", model);
			unlitShader.setMat4("view", view);
			unlitShader.setMat4("projection", projection);
			unlitShader.setVec3("color", 0.9f, 0.9f, 0.9f);
			objModel.Draw(unlitShader);

			wireframeShader.use();
			wireframeShader.setMat4("model", model);
			wireframeShader.setMat4("view", view);
			wireframeShader.setMat4("projection", projection);
			wireframeShader.setVec3("color", 1.0f, 0.0f, 0.0f);
			objModel.DrawWireframe(wireframeShader);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

}

bool firstMouse = true;
bool isLmbPressed = false;
bool isMmbPressed = false;
bool isRmbPressed = false;


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		camera.SwitchToWalkMode(true);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		isRmbPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		camera.SwitchToWalkMode(false);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isRmbPressed = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		isLmbPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		isLmbPressed = false;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		isMmbPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		isMmbPressed = false;
	}
}

//static int oldState = GLFW_RELEASE;
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (isRmbPressed)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		camera.WalkRotate(xoffset, yoffset);
	}
	else if (isLmbPressed || isMmbPressed)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;
		if (isLmbPressed)
		{
			camera.TurntableRotate(xoffset, yoffset);
		}
		else if (isMmbPressed)
		{
			camera.TurntableTranslate(-xoffset, -yoffset);
		}
	}
	else
	{
		firstMouse = true;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!isRmbPressed)
	{
		camera.TurntableZoom(yoffset);
	}
	else
	{
		camera.WalkZoom(yoffset);
	}
}

void key_callback(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !isRmbPressed)
		camera.FocusToOrigin();
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		if (currentViewportShader != ViewportShaderMode::Lit)
		{
			currentViewportShader = ViewportShaderMode::Lit;
			std::cout << "Switch to Lit Mode" << std::endl;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		if (currentViewportShader != ViewportShaderMode::MatCap)
		{
			currentViewportShader = ViewportShaderMode::MatCap;
			std::cout << "Switch to MatCap Mode" << std::endl;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		if (currentViewportShader != ViewportShaderMode::Wireframe)
		{
			currentViewportShader = ViewportShaderMode::Wireframe;
			std::cout << "Switch to Wireframe Mode" << std::endl;
		}
	}

	if (isRmbPressed)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.WalkTranslate(Camera_Walk::DOWN, deltaTime);
	}
}