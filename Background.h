#pragma once
#include "Globals.h"
#include "dcRenderer.h"

class Background {
public:
	Background();
	~Background();

	void init();
	void update(float dt);
	void draw();
	void destroy();

private:
	dcRender::Shader m_shader;
	GLuint m_VAO;
	GLuint m_VBO;



	float m_time = 0;
	float m_cubesInColumn = 20.0f;
	float m_cubesInRow = 20.0f;
	float m_radius = 5.0f;
};