#include "Globals.h"
#include "dcRenderer.h"
#include "dcMath.h"

unsigned int sWidth = 1280;
unsigned int sHeight = 720;

//Window to be displayed throughout game
sf::Window window;

struct Transform {
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;
};

struct PhysicsComponent {
	glm::vec3 currPos = glm::vec3(0,0,0);
	glm::vec3 oldPos = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
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
	float dragCoefficient = 0.2f;
};

void PhysicsSystem::update(float dt) {
	resultantForce = glm::vec3(0, 0, 0);
	ApplyForce(ComputeGravity(entities[1]));
	ApplyForce(ComputeDrag(entities[1]));
	//ApplyForce(ComputeSpring(&entities[0], &entities[1]));

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

class Cube {
public:
	Cube();
	~Cube();
	void init(glm::vec3 position, PhysicsComponent* p,glm::vec3 color);
	void update(float dt);
	void draw();
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

void Cube::draw() {
	m_renderer.draw(m_transform.position, m_transform.rotation, m_transform.scale, m_color);
}

void Cube::destroy() {
	m_renderer.destroy();
}

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	window.create(sf::VideoMode(sWidth, sHeight), "SHMUP Engine", sf::Style::Titlebar | sf::Style::Close, settings);

	// Set OpenGL options
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	PhysicsSystem physicsSystem;

	Cube cube;
	cube.init(glm::vec3(0.0f, 0.0f, 0.0f), &physicsSystem.entities[1]);

	Cube cube2;
	cube2.init(glm::vec3(0.0f, 2.0f, 0.0f), &physicsSystem.entities[0], glm::vec3(1.0f, 0.0f, 0.0f));

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		float dt = clock.restart().asSeconds();
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		physicsSystem.update(dt);
		cube.update(dt);
		cube2.update(dt);

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.draw();
		cube2.draw();

		window.display();
	}

	cube.destroy();
	cube2.destroy();

	window.close();
	return 0;
}