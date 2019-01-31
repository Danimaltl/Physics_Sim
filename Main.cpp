#include "Globals.h"
#include "dcRenderer.h"
#include "dcMath.h"

unsigned int SCREEN_WIDTH = 1280;
unsigned int SCREEN_HEIGHT = 720;

//Window to be displayed throughout game
//sf::Window window;

struct Transform {
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;
};

struct PhysicsComponent {
	glm::vec3 currPos = glm::vec3(0,0,0);
	glm::vec3 oldPos = glm::vec3(0,0,0);
	glm::vec3 velocity = glm::vec3(0,0,0);
	float mass = 0;
	bool active = true;
};

class PhysicsSystem {
public:
	void update(float dt);
	void ApplyForce(glm::vec3 force);

	glm::vec3 ComputeGravity(const PhysicsComponent& c);
	glm::vec3 ComputeDrag(const PhysicsComponent& c);
	glm::vec3 ComputeSpring(PhysicsComponent* a, PhysicsComponent* b);

	int numEntities = 2;
	PhysicsComponent entities[2];
private:
	glm::vec3 resultantForce = glm::vec3(0, 0, 0);
	float dragCoefficient = 0.5f;
	float stiffness = 8.0f;
	float damping = 0.1f;
	float restLength = 1.0f;
};

void PhysicsSystem::update(float dt) {
	resultantForce = glm::vec3(0, 0, 0);
	ApplyForce(ComputeGravity(entities[1]));
	ApplyForce(ComputeDrag(entities[1]));
	ApplyForce(ComputeSpring(&entities[0], &entities[1]));

	glm::vec3 acceleration = resultantForce / entities[1].mass;
	entities[1].currPos += entities[1].velocity * dt;
	entities[1].velocity += acceleration * dt;
}

void PhysicsSystem::ApplyForce(glm::vec3 force) {
	resultantForce += force;
}

glm::vec3 PhysicsSystem::ComputeGravity(const PhysicsComponent& c) {
	glm::vec3 acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
	return c.mass * acceleration;
}

glm::vec3 PhysicsSystem::ComputeDrag(const PhysicsComponent& c) {
	return -(c.velocity * dragCoefficient);
}

glm::vec3 PhysicsSystem::ComputeSpring(PhysicsComponent* a, PhysicsComponent* b) {
	glm::vec3 direction = a->currPos - b->currPos;
	glm::vec3 force = glm::vec3(0, 0, 0);
	if (direction != glm::vec3(0, 0, 0)) {
		float length = dcMath::Magnitude(direction);
		dcMath::Normalize(direction);

		force = -stiffness * ((length - restLength) * direction);

		force += -damping * dcMath::Dot(a->velocity - b->velocity, direction);

		return -force;
	}
	return force;
}

class Cube {
public:
	Cube();
	~Cube();
	void init(glm::vec3 position, PhysicsComponent* p, dcRender::Shader* shader, glm::vec3 color);
	void update(float dt);
	void draw(glm::mat4 view);
	void destroy();
	Transform m_transform;
private:
	dcRender::Shader* m_shader;
	dcRender::CubeRenderer m_renderer;
	PhysicsComponent* m_phys;
	glm::vec3 m_color;
};

Cube::Cube() {

}

Cube::~Cube() {

}

void Cube::init(glm::vec3 position, PhysicsComponent* p, dcRender::Shader* shader, glm::vec3 color = glm::vec3(0.516f, 0.461f, 0.550f)) {
	assert(p != nullptr);
	m_phys = p;

	assert(shader != nullptr);
	m_shader = shader;
	m_shader->use();

	m_renderer.init(glm::vec3(0, 0, 0), m_shader);

	m_transform.position = position;
	m_transform.rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	m_transform.scale = glm::vec3(1, 1, 1);

	m_phys->mass = 1.0f;
	m_phys->currPos = m_transform.position;
	m_phys->oldPos = m_phys->currPos;
	
	m_color = color;
}

void Cube::update(float dt) {
	m_transform.position = m_phys->currPos;
}

void Cube::draw(glm::mat4 view) {
	m_shader->SetMatrix4("view", view);
	m_renderer.draw(m_transform.position, m_transform.rotation, m_transform.scale, m_color);
}

void Cube::destroy() {
	m_renderer.destroy();
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class Camera {
public:
	glm::vec3 position;
	void SetOrientation(float rightAngle, float upAngle);
	void OffsetOrientation(float rightAngle, float upAngle);
	glm::mat4 GetOrientation() const;
	glm::quat GetOrientationQuat() const;
	glm::vec3 GetFace() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	float GetUpAngle() const;
	float GetRightAngle() const;
	float m_upAngle = 0.0f;
	float m_rightAngle = 0.0f;
private:


};

void Camera::SetOrientation(float rightAngle, float upAngle)//in degrees
{
	m_rightAngle = rightAngle;
	m_upAngle = upAngle;
}

void Camera::OffsetOrientation(float rightAngle, float upAngle)//in degrees
{
	m_rightAngle += rightAngle;
	m_upAngle += upAngle;
}

glm::mat4 Camera::GetOrientation() const
{
	glm::quat q = glm::angleAxis(glm::radians(-m_upAngle), glm::vec3(1, 0, 0));
	q *= glm::angleAxis(glm::radians(m_rightAngle), glm::vec3(0, 1, 0));
	return glm::mat4_cast(q);
}

glm::quat Camera::GetOrientationQuat() const
{
	glm::quat q = glm::angleAxis(glm::radians(-m_upAngle), glm::vec3(1, 0, 0));
	q *= glm::angleAxis(glm::radians(m_rightAngle), glm::vec3(0, 1, 0));
	return q;
}

glm::vec3 Camera::GetFace() const {
	glm::vec3 front;
	front.x = cos(glm::radians(m_upAngle)) * -sin(glm::radians(m_rightAngle));
	front.y = -sin(glm::radians(m_upAngle));
	front.z = cos(glm::radians(m_upAngle)) * cos(glm::radians(m_rightAngle));
	return glm::normalize(front);
}

glm::vec3 Camera::GetUp() const {
	return glm::cross(GetFace(), GetRight());
}

glm::vec3 Camera::GetRight() const {
	return glm::cross(GetFace(), glm::vec3(0.0f, 1.0f, 0.0f));
}

float Camera::GetUpAngle() const {
	return m_upAngle;
}

float Camera::GetRightAngle() const {
	return m_rightAngle;
}


int main()
{
	//sf::ContextSettings settings;
	//settings.depthBits = 24;
	//settings.stencilBits = 8;
	//window.create(sf::VideoMode(sWidth, sHeight), "SHMUP Engine", sf::Style::Titlebar | sf::Style::Close, settings);

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics Sim", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Set OpenGL options
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	bool err = glewInit() != GLEW_OK;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	PhysicsSystem physicsSystem;

	dcRender::Shader cubeShader;
	cubeShader.loadFromFile("lamp.vert", "lamp.frag");
	Cube cube;

	cube.init(glm::vec3(0.0f, -2.0f, 0.0f), &physicsSystem.entities[1], &cubeShader);

	Cube cube2;
	cube2.init(glm::vec3(0.0f, 2.0f, 0.0f), &physicsSystem.entities[0], &cubeShader, glm::vec3(1.0f, 0.0f, 0.0f));

	sf::Clock clock;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	double xmouse, ymouse;

	Camera camera;
	camera.position = glm::vec3(0, 0, -10);


	float yaw = 0.0f;
	float pitch = 0.0f;

	float lastX = SCREEN_WIDTH / 2;
	float lastY = SCREEN_HEIGHT / 2;

	bool firstMouse = true;

	float ambientLight = 1.0f;

	while (!glfwWindowShouldClose(window))
	{   
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		float dt = clock.restart().asSeconds();

		glfwGetCursorPos(window, &xmouse, &ymouse);

		float speed = 2;

		if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			camera.position += (camera.GetFace() * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			camera.position -= (camera.GetFace() * speed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			camera.position -= (camera.GetRight() * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			camera.position += (camera.GetRight() * speed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			camera.position += (camera.GetUp() * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			camera.position -= (camera.GetUp() * speed * dt);
		}

		bool mouseOne = false;
		bool mouseTwo = false;
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			mouseOne = true;
		}
		else {
			mouseOne = false;
		}
		if (glfwGetMouseButton(window, 1) == GLFW_PRESS) {
			mouseTwo = true;
		}
		else {
			mouseTwo = false;
		}

		glm::mat4 view = glm::mat4(1);
		glm::mat4 rotate = glm::mat4(1);
		glm::vec3 cameraFront;
		if (mouseTwo) {
			if (firstMouse) // this bool variable is initially set to true
			{
				lastX = (float)xmouse;
				lastY = (float)ymouse;
				firstMouse = false;
			}
			float xoffset = (float)xmouse - lastX;
			float yoffset = lastY - (float)ymouse; // reversed since y-coordinates go from bottom to top
			lastX = (float)xmouse;
			lastY = (float)ymouse;

			float sensitivity = 0.20f; // change this value to your liking
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			////For a FPS camera we can omit roll
			//glm::quat orientation = glm::quat(glm::vec3(pitch, yaw, 0));
			//camera.rotation = camera.rotation * orientation;
			//camera.rotation = glm::normalize(camera.rotation);

			camera.OffsetOrientation(yaw, pitch);

			pitch = 0.0f;
			yaw = 0.0f;
		}
		else {
			firstMouse = true;
		}

		view = (camera.GetOrientation() * glm::translate(view, camera.position));

		physicsSystem.update(dt);
		cube.update(dt);
		cube2.update(dt);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
			ImGui::Text("UpAngle: %.3f, RightAngle: %.3f", camera.GetUpAngle(), camera.GetRightAngle());
			ImGui::SliderFloat("Ambient level", &ambientLight, 0.0f, 1.0f);
			ImGui::SliderFloat("RightAngle", &camera.m_rightAngle, -90.0f, 90.0f);    
			ImGui::SliderFloat("UpAngle", &camera.m_upAngle, -90.0f, 90.0f);
			ImGui::SliderFloat("RedCube x", &cube2.m_transform.position.x, -5.0f, 5.0f);
			ImGui::SliderFloat("RedCube y", &cube2.m_transform.position.y, -5.0f, 5.0f);
			ImGui::SliderFloat("RedCube z", &cube2.m_transform.position.z, -5.0f, 5.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse position is: %.3f , %.3f", xmouse - SCREEN_WIDTH/2, ymouse - SCREEN_HEIGHT/2);
			ImGui::Text("Camera position: %.3f, %.3f, %.3f", camera.position.x, camera.position.y, camera.position.z);
			ImGui::SliderFloat("Position x ", &camera.position.x, -5.0f, 5.0f);
			ImGui::SliderFloat("Position y ", &camera.position.y, -5.0f, 5.0f);
			ImGui::SliderFloat("Position z ", &camera.position.z, -5.0f, 5.0f);
			if (ImGui::Button("Reset"))
				camera.position = glm::vec3(0,0,-10);
			ImGui::Text("Camera rotation: %.3f, %.3f, %.3f, %.3f", camera.GetOrientationQuat().x, camera.GetOrientationQuat().y, camera.GetOrientationQuat().z, camera.GetOrientationQuat().w);
			//if (ImGui::Button("Boop"))
			//	camera.rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			glm::vec3 face = camera.GetFace();
			ImGui::Text("Face vector: %.3f, %.3f, %.3f", face.x, face.y, face.z);
			glm::vec3 right = camera.GetRight();
			ImGui::Text("Right vector: %.3f, %.3f, %.3f", right.x, right.y, right.z);
			ImGui::Text("Left Mouse: %s", mouseOne ? "true" : "false");
			ImGui::Text("Right Mouse: %s", mouseTwo ? "true" : "false");
			glm::vec2 test = dcMath::Normalize(glm::vec2(-1, 1));
			ImGui::Text("Test x: %f, y: %f", test.x, test.y);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubeShader.SetFloat("ambientStrength", ambientLight);
		cube.draw(view);
		cube2.draw(view);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	cube.destroy();
	cube2.destroy();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}