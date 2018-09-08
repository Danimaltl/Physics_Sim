//Dan Cochran 2018
//Based on https://learnopengl.com/In-Practice/2D-Game/Setting-up
#include "dcRenderer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

dcRender::Shader::Shader() {

}

dcRender::Shader::~Shader() {

}

void dcRender::Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource) {
	GLuint sVertex, sFragment;
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	// Shader Program
	m_id = glCreateProgram();
	glAttachShader(m_id, sVertex);
	glAttachShader(m_id, sFragment);
	glLinkProgram(m_id);
	checkCompileErrors(m_id, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
}

void dcRender::Shader::loadFromFile(const char* vertexFile, const char* fragmentFile) {
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexFile);
		fShaderFile.open(fragmentFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files: " << vertexFile << ", " << fragmentFile << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// 2. Now create shader object from source code
	this->compile(vShaderCode, fShaderCode);
}

void dcRender::Shader::destroy() {
	glDeleteProgram(m_id);
}

void dcRender::Shader::checkCompileErrors(GLuint object, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}

void dcRender::Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix) {
	GLuint uniform = glGetUniformLocation(m_id, name);
	if (uniform == -1) {
		printf("Couldn't find mat4 uniform: %s\n", name);
	}
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
}

void dcRender::Shader::SetFloat(const GLchar *name, GLfloat value) {
	GLuint uniform = glGetUniformLocation(m_id, name);
	if (uniform == -1) {
		printf("Couldn't find float uniform: %s\n", name);
	}
	glUniform1f(uniform, value);
}

void dcRender::Shader::SetVector3(const GLchar *name, const glm::vec3 &value) {
	GLuint uniform = glGetUniformLocation(m_id, name);
	if (uniform == -1) {
		printf("Couldn't find vec3 uniform: %s\n", name);
	}
	glUniform3f(uniform, value.x, value.y, value.z);
}

void dcRender::Shader::SetVector2(const GLchar *name, const glm::vec2 &value) {
	GLuint uniform = glGetUniformLocation(m_id, name);
	if (uniform == -1) {
		printf("Couldn't find vec2 uniform: %s\n", name);
	}
	glUniform2f(uniform, value.x, value.y);
}

dcRender::CircleRenderer::CircleRenderer() {

}

dcRender::CircleRenderer::~CircleRenderer() {

}

void dcRender::CircleRenderer::init(int numPoints, Shader* shader) {
	float radius = 1;
	m_shader = shader;
	m_numPoints = numPoints;
	m_vertArraySize = numPoints * 2;

	GLfloat* vertices = new float[m_vertArraySize];

	glm::mat4 model(1);
	model = glm::translate(model, glm::vec3(200, 200, 0.0f));
	model = glm::scale(model, glm::vec3(100, 100, 1.0f));

	for (int i = 0; i < numPoints * 2; i += 2) {
		float angle = (float)i * 2.0f * (float)M_PI / (float)numPoints;
		vertices[i] = cos(angle) * radius;
		vertices[i + 1] = sin(angle) * radius;

		glm::vec4 vert = model * glm::vec4(vertices[i], vertices[i + 1], 0.0f, 1.0f);
		//printf("Vert%d: x:%f, y:%f\n", i, vert.x, vert.y);
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_vertArraySize, vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete vertices;
	vertices = nullptr;
}

void dcRender::CircleRenderer::draw(glm::vec2 position, float rotation, float radius, glm::vec3 color) {
	m_shader->use();
	glm::mat4 model(1);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(radius, radius, 1.0f));

	m_shader->SetMatrix4("model", model);
	m_shader->SetVector3("circleColor", color);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_numPoints);
	glBindVertexArray(0);
}

void dcRender::CircleRenderer::destroy() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

dcRender::PolyRenderer::PolyRenderer() {

}

dcRender::PolyRenderer::~PolyRenderer() {

}

void dcRender::PolyRenderer::init(GLfloat* points, int size, glm::vec2 center, GLuint drawMethod, Shader* shader) {
	float radius = 1;
	m_shader = shader;
	m_vertArraySize = size;
	m_numPoints = size / 2;
	m_drawMethod = drawMethod;
	m_center = center;

	GLfloat* vertices = points;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_vertArraySize, vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void dcRender::PolyRenderer::draw(glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color) {
	m_shader->use();
	glm::mat4 model(1);

	model = glm::translate(model, glm::vec3(position - m_center, 0.0f));

	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::translate(model, glm::vec3(m_center.x * scale.x, m_center.y * scale.y, 0.0f));
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-m_center.x * scale.x, -m_center.y * scale.y, 0.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(scale, 1.0f));

	m_shader->SetMatrix4("model", model);
	m_shader->SetVector3("circleColor", color);

	glBindVertexArray(m_VAO);
	glDrawArrays(m_drawMethod, 0, m_numPoints);
	glBindVertexArray(0);
}

void dcRender::PolyRenderer::destroy() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

/* Cube Renderer */
dcRender::CubeRenderer::CubeRenderer() {

}

dcRender::CubeRenderer::~CubeRenderer() {

}

void dcRender::CubeRenderer::init(glm::vec3 center, Shader* shader) {
	m_shader = shader;
	m_center = center;

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

	m_shader->use();

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(m_shader->getID(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(m_shader->getID(), "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Set up projection
	glm::mat4 view = glm::mat4(1);
	//view = glm::lookAt(
	//	glm::vec3(1.5f, 1.5f, 1.5f),
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 0.0f, 1.0f)
	//);
	//view = glm::rotate(view, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
	m_shader->SetMatrix4("view", view);

	glm::mat4 proj = glm::mat4(1);
	proj = glm::perspective(glm::radians(45.0f), (float)sWidth / (float)sHeight, 0.1f, 100.0f);
	m_shader->SetMatrix4("proj", proj);
}

void dcRender::CubeRenderer::draw(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 color) {
	m_shader->use();
	glm::mat4 model(1);

	//translate to world position
	model = glm::translate(model, glm::vec3(position - m_center));

	//translate to center and rotate?
	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	//model = glm::translate(model, glm::vec3(m_center.x * scale.x, m_center.y * scale.y, 0.0f));
	model *= glm::toMat4(rotation);
	//model = glm::translate(model, glm::vec3(-m_center.x * scale.x, -m_center.y * scale.y, 0.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, scale);

	m_shader->SetMatrix4("model", model);
	m_shader->SetVector3("flatColor", color);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void dcRender::CubeRenderer::destroy() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}


/* Text Renderer */
dcRender::TextRenderer::TextRenderer() {

}

dcRender::TextRenderer::~TextRenderer() {

}

void dcRender::TextRenderer::init() {

	m_shader.loadFromFile("shaders/text.vert", "shaders/text.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(sWidth), 0.0f, static_cast<GLfloat>(sHeight));
	m_shader.use();
	m_shader.SetMatrix4("projection", projection);

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "assets/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		TextChar character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		m_characters[c] = character;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void dcRender::TextRenderer::draw(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	y = (GLfloat)sHeight - y;

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	// Activate corresponding render state
	m_shader.use();
	m_shader.SetVector3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		TextChar ch = m_characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void dcRender::TextRenderer::draw(TextComponent t) {
	draw(t.text, t.position.x, t.position.y, t.scale, t.color);
}

void dcRender::TextRenderer::destroy() {
	m_shader.destroy();
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);

	for (int i = 0; i < 128; i++) {
		glDeleteTextures(1, &m_characters[i].TextureID);
	}
}
