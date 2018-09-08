#include "Background.h"

Background::Background() {

}

Background::~Background() {

}

void Background::init() {
	// Create Vertex Array Object
	glGenVertexArrays(1, &m_VAO);

	// Create a Vertex Buffer Object and copy the vertex data to it
	glGenBuffers(1, &m_VBO);

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);

	m_shader.loadFromFile("shaders/cubeShape.vert", "shaders/cubeShape.frag");
	m_shader.use();

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(m_shader.getID(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(m_shader.getID(), "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Set up projection
	glm::mat4 view = glm::mat4(1);
	//view = glm::lookAt(
	//	glm::vec3(1.5f, 1.5f, 1.5f),
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 0.0f, 1.0f)
	//);
	view = glm::rotate(view, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(0.0f, -10.0f, 0.0f));
	m_shader.SetMatrix4("view", view);

	glm::mat4 proj = glm::mat4(1);
	proj = glm::perspective(glm::radians(45.0f), 600.0f / 800.0f, 0.1f, 100.0f);
	m_shader.SetMatrix4("proj", proj);
}

void Background::update(float dt) {
	m_time += dt;
	m_shader.use();
	m_shader.SetFloat("time", m_time);
}

void Background::draw() {
	m_shader.use();

	for (int i = 0; i < m_cubesInRow; i++) {
		for (int j = 0; j < m_cubesInColumn; j++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::rotate(
				model,
				m_time * glm::radians(60.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
			float xpos = (float)cosf(((float)i / m_cubesInRow) * 2.0f * (float)M_PI) * (1.5f * m_radius + cos(m_time / 2)*(m_radius / 2));
			float ypos = (float)sinf(((float)i / m_cubesInRow) * 2.0f * (float)M_PI) * (1.5f * m_radius + cos(m_time / 2)*(m_radius / 2));
			float zpos = (float)(-m_cubesInColumn + 2 * j) + cos(m_time + (i * 5)) * 2.0f;
			glm::vec3 position = glm::vec3(xpos, ypos, zpos);
			model = glm::translate(model, position);
			model = glm::rotate(
				model,
				m_time * glm::radians(180.0f),
				glm::vec3(cosf(m_time), sinf(m_time), 1.0f)
			);
			m_shader.SetMatrix4("model", model);
			glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
}

void Background::destroy() {
	m_shader.destroy();
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}
