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
	void init(glm::vec3 position, PhysicsComponent* p,glm::vec3 color);
	void update(float dt);
	void draw(glm::mat4 view);
	void destroy();

private:
	dcRender::Shader m_shader;
	dcRender::CubeRenderer m_renderer;
	Transform m_transform;
	PhysicsComponent* m_phys;
	glm::vec3 m_color;
};

Cube::Cube() {

}

Cube::~Cube() {

}

void Cube::init(glm::vec3 position, PhysicsComponent* p, glm::vec3 color = glm::vec3(0.516f, 0.461f, 0.550f)) {
	assert(p != nullptr);
	m_phys = p;

	m_shader.loadFromFile("cubeShape.vert", "cubeShape.frag");
	m_shader.use();

	m_renderer.init(glm::vec3(0, 0, 0), &m_shader);

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
	m_shader.SetMatrix4("view", view);
	m_renderer.draw(m_transform.position, m_transform.rotation, m_transform.scale, m_color);
}

void Cube::destroy() {
	m_renderer.destroy();
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
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

	Cube cube;
	cube.init(glm::vec3(0.0f, -2.0f, 0.0f), &physicsSystem.entities[1]);

	Cube cube2;
	cube2.init(glm::vec3(0.0f, 2.0f, 0.0f), &physicsSystem.entities[0], glm::vec3(1.0f, 0.0f, 0.0f));

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

	Transform camera;
	camera.position = glm::vec3(0, 0, -10);
	camera.rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

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
			camera.position += (dcMath::ForwardVector(camera.rotation) * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			camera.position -= (dcMath::ForwardVector(camera.rotation) * speed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			camera.position += (dcMath::LeftVector(camera.rotation) * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			camera.position -= (dcMath::LeftVector(camera.rotation) * speed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			camera.position -= (dcMath::UpVector(camera.rotation) * speed * dt);
		}
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			camera.position += (dcMath::UpVector(camera.rotation) * speed * dt);
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

		if (mouseTwo) {
			glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));

		}

		glm::mat4 view = glm::mat4(1);
		//view = glm::rotate(view, camera.rotation);
		view = glm::translate(view, camera.position);

		physicsSystem.update(dt);
		cube.update(dt);
		cube2.update(dt);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse position is: %.3f , %.3f", xmouse, ymouse);
			ImGui::Text("Camera: %.3f, %.3f, %.3f", camera.position.x, camera.position.y, camera.position.z);
			ImGui::Text("Left Mouse: %s", mouseOne ? "true" : "false");
			ImGui::Text("Right Mouse: %s", mouseTwo ? "true" : "false");
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
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