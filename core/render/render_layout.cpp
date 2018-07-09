#include "render_layout.h"
#include "graphics_buffer.h"
#include "shader_object.h"
#include <base/context.h>
#include "mapping.h"
namespace gleam {
	RenderLayout::RenderLayout()
		: topo_type_(TT_PointList),
		index_format_(EF_Unknown),
		force_num_vertices_(0xFFFFFFFF),
		force_num_indices_(0xFFFFFFFF),
		force_num_instances_(0xFFFFFFFF),
		start_vertex_location_(0),
		start_index_location_(0),
		base_vertex_location_(0),
		start_instance_location_(0),
		streams_dirty_(true)
	{
		vertex_streams_.reserve(4);
	}

	RenderLayout::~RenderLayout()
	{

	}

	void RenderLayout::NumVertices(uint32_t n)
	{
		force_num_vertices_ = n;
		streams_dirty_ = true;
	}
	uint32_t RenderLayout::NumVertices() const
	{
		uint32_t n;
		if (0xFFFFFFFF == force_num_vertices_)
		{
			n = vertex_streams_[0].stream->Size() / vertex_streams_[0].vertex_size;
		}
		else
		{
			n = force_num_vertices_;
		}
		return n;
	}
	const GraphicsBufferPtr & RenderLayout::GetVertexStream(ArrayRef<VertexElement> vet)
	{
		uint32_t n = NumVertexStreams();
		std::vector<VertexElement> v = vet.ToVector();
		uint32_t ret = 0xffffffff;
		for (uint32_t i = 0; i < n; ++i)
		{
			if (v == VertexStreamFormat(i))
				ret = i;
		}
		CHECK_INFO(ret != 0xffffffff, "Vertex Element unmatch!");

		return GetVertexStream(ret);
	}
	void RenderLayout::SetVertexStream(uint32_t index, const GraphicsBufferPtr & gbuffer)
	{
		vertex_streams_[index].stream = gbuffer;
		streams_dirty_ = true;
	}
	void RenderLayout::VertexStreamFormat(uint32_t index, ArrayRef<VertexElement> vet)
	{
		vertex_streams_[index].format = vet.ToVector();
		uint32_t bytes = 0;
		for (size_t i = 0; i < vet.size(); ++i)
		{
			bytes += vet[i].NumFormatBytes();
		}
		vertex_streams_[index].vertex_size = bytes;
		streams_dirty_ = true;
	}
	void RenderLayout::VertexStreamFrequencyDivider(uint32_t index, StreamType type, uint32_t freq)
	{
		vertex_streams_[index].type = type;
		vertex_streams_[index].freq = freq;
		streams_dirty_ = true;
	}
	void RenderLayout::BindVertexStream(const GraphicsBufferPtr & buffer, ArrayRef<VertexElement> vet, StreamType type, uint32_t freq)
	{
		assert(buffer);
		uint32_t bytes = 0;
		for (size_t i = 0; i < vet.size(); ++i)
		{
			bytes += vet[i].NumFormatBytes();
		}

		if (ST_Geometry == type)
		{
			for (size_t i = 0; i < vertex_streams_.size(); ++i)
			{
				if (ArrayRef<VertexElement>(vertex_streams_[i].format) == vet)
				{
					vertex_streams_[i].stream = buffer;
					vertex_streams_[i].vertex_size = bytes;
					vertex_streams_[i].type = type;
					vertex_streams_[i].freq = freq;

					streams_dirty_ = true;
					return;
				}
			}
			StreamUnit su;
			su.stream = buffer;
			su.format = vet.ToVector();
			su.vertex_size = bytes;
			su.type = type;
			su.freq = freq;
			vertex_streams_.push_back(su);
		}
		else
		{
			// instance stream
			instance_stream_.stream = buffer;
			instance_stream_.format = vet.ToVector();
			instance_stream_.vertex_size = bytes;
			instance_stream_.type = type;
			instance_stream_.freq = freq;
		}

		streams_dirty_ = true;
	}
	void RenderLayout::InstanceStream(const GraphicsBufferPtr & buffer)
	{
		instance_stream_.stream = buffer;
		streams_dirty_ = true;
	}
	void RenderLayout::NumInstances(uint32_t n)
	{
		force_num_instances_ = n;
		streams_dirty_ = true;
	}
	uint32_t RenderLayout::NumInstances() const
	{
		uint32_t n;
		if (0xFFFFFFFF == force_num_instances_)
		{
			if (vertex_streams_.empty())
			{
				n = 1;
			}
			else
			{
				n = vertex_streams_[0].freq;
			}
		}
		else
		{
			n = force_num_instances_;
		}
		return n;
	}
	void RenderLayout::NumIndices(uint32_t n)
	{
		force_num_indices_ = n;
		streams_dirty_ = true;
	}
	uint32_t RenderLayout::NumIndices() const
	{
		uint32_t n = 0;
		if (index_stream_)
		{
			if (0xFFFFFFFF == force_num_indices_)
			{
				n = index_stream_->Size() / NumFormatBytes(index_format_);
			}
			else
			{
				n = force_num_indices_;
			}
		}
		return n;
	}
	const GraphicsBufferPtr & RenderLayout::GetIndexStream() const
	{
		assert(index_stream_);
		return index_stream_;
	}
	void RenderLayout::BindIndexStream(const GraphicsBufferPtr & index_stream, ElementFormat format)
	{
		assert((EF_R16UI == format) || (EF_R32UI == format));

		index_stream_ = index_stream;
		index_format_ = format;

		streams_dirty_ = true;
	}
	void RenderLayout::StartVertexLocation(uint32_t location)
	{
		start_vertex_location_ = location;
		streams_dirty_ = true;
	}
	void RenderLayout::StartIndexLocation(uint32_t location)
	{
		start_index_location_ = location;
		streams_dirty_ = true;
	}
	void RenderLayout::StartInstanceLocation(uint32_t location)
	{
		start_instance_location_ = location;
		streams_dirty_ = true;
	}
	void RenderLayout::BindIndirectArgs(const GraphicsBufferPtr & args_buff)
	{
		indirect_args_buff_ = args_buff;
		streams_dirty_ = true;
	}
	void RenderLayout::IndirectArgsOffset(uint32_t offset)
	{
		indirect_args_offset = offset;
		streams_dirty_ = true;
	}
	OGLRenderLayout::~OGLRenderLayout()
	{
		for (const auto &vao : vaos_)
		{
			glDeleteVertexArrays(1, &vao.second);
		}
	}
	void OGLRenderLayout::Active(const ShaderObjectPtr & shader) const
	{
		GLuint vao;
		auto iter = vaos_.find(shader);
		if (iter == vaos_.end())
		{
			glCreateVertexArrays(1, &vao);
			glBindVertexArray(vao);
			
			vaos_.emplace(shader, vao);
			this->BindVertexStreams(shader, vao);
			streams_dirty_ = false;
		}
		else
		{
			vao = iter->second;
			glBindVertexArray(vao);
			if (streams_dirty_)
			{
				this->BindVertexStreams(shader, vao);
				streams_dirty_ = false;
			}
		}

		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		if (this->NumVertexStreams() > 0) // cache the last vbo
		{
			OGLGraphicsBuffer &buffer = *checked_pointer_cast<OGLGraphicsBuffer>(this->GetVertexStream(this->NumVertexStreams() - 1));
			re.OverrideBindBufferCache(buffer.GLType(), buffer.GLvbo());
		}

		if (this->UseIndices())
		{
			OGLGraphicsBuffer &stream(*checked_pointer_cast<OGLGraphicsBuffer>(this->GetIndexStream()));
			assert(GL_ELEMENT_ARRAY_BUFFER == stream.GLType());
			stream.Active(true);
		}
		else
		{
			re.OverrideBindBufferCache(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
	void OGLRenderLayout::BindVertexStreams(const ShaderObjectPtr & shader, GLuint vao) const
	{
		const auto &gl_shader = checked_pointer_cast<OGLShaderObject>(shader);

		uint32_t max_vertex_streams = 16; // 16 is supported in almost all gpu support OpenGL 4.5
		std::vector<char> used_streams(max_vertex_streams, 0);
		for (uint32_t i = 0; i < this->NumVertexStreams(); ++i)
		{
			OGLGraphicsBuffer &stream = *checked_pointer_cast<OGLGraphicsBuffer>(this->GetVertexStream(i));
			const uint32_t size = this->VertexSize(i);
			const auto & vertex_stream_format = this->VertexStreamFormat(i);

			glVertexArrayVertexBuffer(vao, i, stream.GLvbo(), this->StartVertexLocation() * size, size);

			uint32_t elem_offset = 0;
			for (const auto &vs_elem : vertex_stream_format)
			{
				GLint attr = gl_shader->GetAttribLocation(vs_elem.usage, vs_elem.usage_index);
				if (attr != -1)
				{
					GLintptr offset = elem_offset + this->StartVertexLocation() * size;
					const GLint num_components = static_cast<GLint>(NumComponents(vs_elem.format));
					GLenum type;
					GLboolean normalized;
					OGLMapping::MappingVertexFormat(type, normalized, vs_elem.format);
					normalized = (((VEU_Diffuse == vs_elem.usage) || (VEU_Specular == vs_elem.usage)) && !IsFloatFormat(vs_elem.format)) ? GL_TRUE : normalized;

					assert(GL_ARRAY_BUFFER == stream.GLType());
					stream.Active(true);

					glVertexArrayAttribFormat(vao, attr, num_components, type, normalized, elem_offset);
					glVertexArrayAttribBinding(vao, attr, i);
					glEnableVertexArrayAttrib(vao, attr);

					used_streams[attr] = 1;
				}

				elem_offset += vs_elem.NumFormatBytes();
			}
		}

		if (this->InstanceStream())
		{
			OGLGraphicsBuffer &stream =
				*(checked_pointer_cast<OGLGraphicsBuffer>(this->InstanceStream()));

			const uint32_t instance_size = this->InstanceSize();
			assert(this->NumInstances() * instance_size <= stream.Size());
			
			glVertexArrayVertexBuffer(vao, this->NumVertexStreams(), stream.GLvbo(),
				this->StartInstanceLocation() * instance_size, instance_size);
			glVertexArrayBindingDivisor(vao, this->NumVertexStreams(), 1);

			const size_t inst_format_size = this->InstanceStreamFormat().size();
			uint32_t elem_offset = 0;
			for (size_t i = 0; i < inst_format_size; ++i)
			{
				VertexElement vs_elem = this->InstanceStreamFormat()[i];
				GLint attr = gl_shader->GetAttribLocation(vs_elem.usage, vs_elem.usage_index);
				if (attr != -1)
				{
					const GLint num_components = static_cast<GLint>(NumComponents(vs_elem.format));
					GLenum type;
					GLboolean normalized;
					OGLMapping::MappingVertexFormat(type, normalized, vs_elem.format);
					normalized = (((VEU_Diffuse == vs_elem.usage) || (VEU_Specular == vs_elem.usage)) && !IsFloatFormat(vs_elem.format)) ? GL_TRUE : normalized;
					GLintptr offset = elem_offset + this->StartInstanceLocation() * instance_size;

					assert(GL_ARRAY_BUFFER == stream.GLType());
					stream.Active(true);
					glVertexArrayAttribFormat(vao, attr, num_components, type, normalized, elem_offset);
					glVertexArrayAttribBinding(vao, attr, this->NumVertexStreams());
					glEnableVertexArrayAttrib(vao, attr);

					used_streams[attr] = 1;
				}
				elem_offset += vs_elem.NumFormatBytes();
			}
		}

		for (GLuint i = 0; i < max_vertex_streams; ++i)
		{
			if (!used_streams[i])
			{
				glDisableVertexArrayAttrib(vao, i);
			}
		}
	}
	void OGLRenderLayout::UnbindVertexStreams(const ShaderObjectPtr & shader, GLuint vao) const
	{
		const OGLShaderObjectPtr &gl_shader = checked_pointer_cast<OGLShaderObject>(shader);
		for (uint32_t i = 0; i < this->NumVertexStreams(); ++i)
		{
			const auto & vertex_stream_format = this->VertexStreamFormat(i);
			for (const auto &vs_elem : vertex_stream_format)
			{
				GLint attr = gl_shader->GetAttribLocation(vs_elem.usage, vs_elem.usage_index);
				if (attr != -1)
				{
					glDisableVertexArrayAttrib(vao, attr);
				}
			}
		}

		if (this->InstanceStream())
		{
			glVertexArrayBindingDivisor(vao, this->NumVertexStreams(), 0);

			const size_t inst_format_size = this->InstanceStreamFormat().size();
			for (size_t i = 0; i < inst_format_size; ++i)
			{
				const VertexElement &vs_elem = this->InstanceStreamFormat()[i];
				GLint attr = gl_shader->GetAttribLocation(vs_elem.usage, vs_elem.usage_index);
				if (attr != -1)
				{
					glDisableVertexArrayAttrib(vao, attr);
				}
			}
		}
	}
	void VertexElementUsageFromString(VertexElementUsage & usage, const std::string & name)
	{
		//// vertex positions
		//VEU_Position = 0,
		//	// vertex normals included (for lighting)
		//	VEU_Normal,
		//	// Vertex colors - diffuse
		//	VEU_Diffuse,
		//	// Vertex colors - specular
		//	VEU_Specular,
		//	// Vertex blend weights
		//	VEU_BlendWeight,
		//	// Vertex blend indices
		//	VEU_BlendIndex,
		//	// at least one set of texture coords (exact number specified in class)
		//	VEU_TextureCoord,
		//	// Vertex tangent
		//	VEU_Tangent,
		//	// Vertex binormal
		//	VEU_Binormal
		if (name == "position")
			usage = VEU_Position;
		else if (name == "normal")
			usage = VEU_Normal;
		else if (name == "diffuse")
			usage = VEU_Diffuse;
		else if (name == "specular")
			usage = VEU_Specular;
		else if (name == "blend_weight")
			usage = VEU_BlendWeight;
		else if (name == "texture_coord")
			usage = VEU_TextureCoord;
		else if (name == "tangent")
			usage = VEU_Tangent;
		else if (name == "bitangent")
			usage = VEU_Binormal;
		else
			CHECK_INFO(false, "Invalid vertex element usage : " << name);
	}
}
