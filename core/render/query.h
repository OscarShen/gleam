#pragma once
#ifndef GLEAM_QUERY_H_
#define GLEAM_QUERY_H_

#include <gleam.h>
#include <GL/glew.h>

namespace gleam
{
	class Query
	{
	public:
		virtual ~Query();

		virtual void Begin() = 0;
		virtual void End() = 0;
	};

	class ConditionalRender : public Query
	{
	public:
		virtual void BeginConditionalRender() = 0;
		virtual void EndConditionalRender() = 0;
		virtual bool AnySamplesPassed() = 0;
	};

	class OGLConditionalRender : public ConditionalRender
	{
	public:
		OGLConditionalRender();
		~OGLConditionalRender();

		void Begin() override;
		void End() override;
		void BeginConditionalRender() override;
		void EndConditionalRender() override;
		bool AnySamplesPassed() override;

	private:
		GLuint query_;
	};
}

#endif // !GLEAM_QUERY_H_
