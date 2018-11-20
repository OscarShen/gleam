#include "query.h"
#include <base/context.h>
#include <render/render_engine.h>

#include <GL/glew.h>

gleam::Query::~Query()
{
}

gleam::OGLConditionalRender::OGLConditionalRender()
{
	glGenQueries(1, &query_);
}

gleam::OGLConditionalRender::~OGLConditionalRender()
{
	glDeleteQueries(1, &query_);
}

void gleam::OGLConditionalRender::Begin()
{
	glBeginQuery(GL_ANY_SAMPLES_PASSED, query_);
}

void gleam::OGLConditionalRender::End()
{
	glEndQuery(GL_ANY_SAMPLES_PASSED);
}

void gleam::OGLConditionalRender::BeginConditionalRender()
{
	glBeginConditionalRender(query_, GL_QUERY_WAIT);
}

void gleam::OGLConditionalRender::EndConditionalRender()
{
	glEndConditionalRender();
}

bool gleam::OGLConditionalRender::AnySamplesPassed()
{
	GLuint available = 0;
	while (!available)
	{
		glGetQueryObjectuiv(query_, GL_QUERY_RESULT_AVAILABLE, &available);
	}

	GLuint ret;
	glGetQueryObjectuiv(query_, GL_QUERY_RESULT, &ret);
	return ret != 0;
}



