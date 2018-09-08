#include "Globals.h"
#include "dcRenderer.h"
#include "dcMath.h"

unsigned int sWidth = 600;
unsigned int sHeight = 800;

//Window to be displayed throughout game
sf::Window window;

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

	dcRender::Shader cubeShader;
	cubeShader.loadFromFile("cubeShape.vert","cubeShape.frag");
	cubeShader.use();
	
	dcRender::CubeRenderer cubeRenderer;
	cubeRenderer.init(glm::vec3(0, 0, 0), &cubeShader);

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

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeRenderer.draw(glm::vec3(0, -10.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1, 1, 1), glm::vec3(0.516, 0.461, 0.550));

		window.display();

	}

	window.close();
	return 0;
}