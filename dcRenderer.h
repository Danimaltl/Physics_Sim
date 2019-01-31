//Dan Cochran
//Based on https://learnopengl.com/In-Practice/2D-Game/Setting-up
//			https://learnopengl.com/In-Practice/Text-Rendering

#pragma once
#include "Globals.h"

namespace dcRender {
	class Shader {
	public:
		const glm::mat4 projection = glm::ortho(0.0f, 600.0f, 800.0f, 0.0f, -1.0f, 1.0f);
		Shader();
		~Shader();
		void use() { glUseProgram(m_id); }
		void compile(const GLchar* vertexSource, const GLchar* fragmentSource);
		void loadFromFile(const char* vertexFile, const char* fragmentFile);
		void destroy();

		void SetMatrix4(const GLchar *name, const glm::mat4 &matrix);
		void SetFloat(const GLchar *name, GLfloat value);
		void SetVector3(const GLchar *name, const glm::vec3 &value);
		void SetVector2(const GLchar *name, const glm::vec2 &value);

		GLuint getID() { return m_id; }
	private:
		void checkCompileErrors(GLuint object, std::string type);
		GLuint m_id;
	};

	class CircleRenderer {
	public:
		CircleRenderer();
		~CircleRenderer();
		void init(int numPoints, Shader* shader);
		void draw(glm::vec2 position, float rotation, float radius, glm::vec3 color);
		void destroy();
	private:
		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		Shader* m_shader = nullptr;
		int m_numPoints;
		int m_vertArraySize;
	};

	class PolyRenderer {
	public:
		PolyRenderer();
		~PolyRenderer();
		void init(GLfloat* points, int size, glm::vec2 center, GLuint drawMethod, Shader* shader);
		void draw(glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color);
		void destroy();
	private:
		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		Shader* m_shader = nullptr;
		int m_numPoints;
		int m_vertArraySize;
		GLuint m_drawMethod;
		glm::vec2 m_center;
	};

	class CubeRenderer {
	public:
		CubeRenderer();
		~CubeRenderer();
		void init(glm::vec3 center, Shader* shader);
		void draw(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 color);
		void destroy();
	private:
		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		Shader* m_shader = nullptr;

		glm::vec3 m_center;
	};

	class LightCubeRenderer {
	public:
		LightCubeRenderer();
		~LightCubeRenderer();
		void init(glm::vec3 center, Shader* shader);
		void draw(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 color);
		void destroy();
	private:
		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		Shader* m_shader = nullptr;

		glm::vec3 m_center;
	};

	/* Text Rendering */
	struct TextChar {
		GLuint TextureID;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
	};

	struct TextComponent {
		std::string text = "";
		glm::vec3 color = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec2 position = glm::vec2(0.0f,0.0f);
		GLfloat scale = 1.0f;
		TextComponent() {}
		~TextComponent(){};
	};

	class TextRenderer {
	public:
		TextRenderer();
		~TextRenderer();
		void destroy();
		void init();
		void draw(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
		void draw(TextComponent t);
	private:
		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		Shader m_shader;
		glm::vec2 m_center;
		TextChar m_characters[128];
	};
}
