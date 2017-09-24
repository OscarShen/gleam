/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_LAYOUT_H_
#define GLEAM_CORE_RENDER_LAYOUT_H_
#include <GL/glew.h>
#include "element_format.h"
#include <map>
#include <util/array_ref.hpp>
namespace gleam
{
	enum VertexElementUsage
	{
		// vertex positions
		VEU_Position = 0,
		// vertex normals included (for lighting)
		VEU_Normal,
		// Vertex colors - diffuse
		VEU_Diffuse,
		// Vertex colors - specular
		VEU_Specular,
		// Vertex blend weights
		VEU_BlendWeight,
		// Vertex blend indices
		VEU_BlendIndex,
		// at least one set of texture coords (exact number specified in class)
		VEU_TextureCoord,
		// Vertex tangent
		VEU_Tangent,
		// Vertex binormal
		VEU_Binormal
	};

	enum TopologyType
	{
		TT_PointList,
		TT_LineList,
		TT_LineStrip,
		TT_TriangleList,
		TT_TriangleStrip,
	};

	enum StreamType
	{
		ST_Geometry,
		ST_Instance
	};

	struct VertexElement
	{
		VertexElement() { }
		VertexElement(VertexElementUsage usage, uint8_t usage_index, ElementFormat format)
			: usage(usage), usage_index(usage_index), format(format) {}
		uint16_t NumFormatBytes() const
		{
			return gleam::NumFormatBytes(format);
		}
		friend bool operator==(const VertexElement &lhs, const VertexElement &rhs)
		{
			return (lhs.usage == rhs.usage) && (lhs.usage_index == rhs.usage_index) && (lhs.format == rhs.format);
		}

		VertexElementUsage usage;
		uint8_t usage_index;
		ElementFormat format;
	};

	class RenderLayout
	{
	public:
		RenderLayout();
		virtual ~RenderLayout() = 0;

		void TopologyType(gleam::TopologyType type) { topo_type_ = type; }
		gleam::TopologyType TopologyType() const { return topo_type_; }



		void NumVertices(uint32_t n);
		uint32_t NumVertices() const;

		uint32_t NumVertexStreams() const { return static_cast<uint32_t>(vertex_streams_.size()); }

		const GraphicsBufferPtr &GetVertexStream(uint32_t index) const { return vertex_streams_[index].stream; }
		void SetVertexStream(uint32_t index, const GraphicsBufferPtr &gbuffer);

		void VertexStreamFormat(uint32_t index, ArrayRef<VertexElement> vet);
		const std::vector<VertexElement> &VertexStreamFormat(uint32_t index) const { return vertex_streams_[index].format; }

		uint32_t VertexSize(uint32_t index) const { return vertex_streams_[index].vertex_size; }
		StreamType VertexStreamType(uint32_t index) const { return vertex_streams_[index].type; }
		uint32_t VertexStreamFrequency(uint32_t index) const { return vertex_streams_[index].freq; }

		void VertexStreamFrequencyDivider(uint32_t index, StreamType type, uint32_t freq);

		void BindVertexStream(const GraphicsBufferPtr &buffer, ArrayRef<VertexElement> vet,
			StreamType type = ST_Geometry, uint32_t freq = 1);



		bool UseIndices() const { this->NumIndices(); }
		void NumIndices(uint32_t n);
		uint32_t NumIndices() const;

		const GraphicsBufferPtr &GetIndexStream() const;
		ElementFormat IndexStreamFormat() const { return index_format_; }

		void BindIndexStream(const GraphicsBufferPtr &index_stream, ElementFormat format);



		const GraphicsBufferPtr &InstanceStream() const { return instance_stream_.stream; }
		void InstanceStream(const GraphicsBufferPtr &buffer);
		const std::vector<VertexElement> &InstanceStreamFormat() const { return instance_stream_.format; }
		uint32_t InstanceSize() const { return instance_stream_.vertex_size; }
		void NumInstances(uint32_t n);
		uint32_t NumInstances() const;



		void StartVertexLocation(uint32_t location);
		uint32_t StartVertexLocation() const { return start_vertex_location_; }

		void StartIndexLocation(uint32_t location);
		uint32_t StartIndexLocation() const { return start_index_location_; }

		void StartInstanceLocation(uint32_t location);
		uint32_t StartInstanceLocation() const { return start_instance_location_; }



		void BindIndirectArgs(const GraphicsBufferPtr &args_buff);
		const GraphicsBufferPtr &GetIndirectArgs() const { return indirect_args_buff_; }
		void IndirectArgsOffset(uint32_t offset);
		uint32_t IndirectArgsOffset() const { return indirect_args_offset; }

	protected:
		gleam::TopologyType topo_type_;

		struct StreamUnit
		{
			GraphicsBufferPtr stream;
			std::vector<VertexElement> format;
			uint32_t vertex_size;

			StreamType type;
			uint32_t freq;
		};

		std::vector<StreamUnit> vertex_streams_;
		StreamUnit				instance_stream_;

		GraphicsBufferPtr		index_stream_;
		ElementFormat			index_format_;

		uint32_t				force_num_vertices_;
		uint32_t				force_num_indices_;
		uint32_t				force_num_instances_;

		uint32_t				start_vertex_location_;
		int32_t					base_vertex_location_;
		uint32_t				start_index_location_;
		uint32_t				start_instance_location_;

		GraphicsBufferPtr		indirect_args_buff_;
		uint32_t				indirect_args_offset;

		mutable bool			streams_dirty_;
	};


	class OGLRenderLayout : public RenderLayout
	{
	public:
		OGLRenderLayout() { }
		~OGLRenderLayout();

		void Active(const ShaderObjectPtr &shader) const;

	private:
		void BindVertexStreams(const ShaderObjectPtr &shader, GLuint vao) const;
		void UnbindVertexStreams(const ShaderObjectPtr &shader, GLuint vao) const;

	private:
		mutable std::map<ShaderObjectPtr, GLuint> vaos_;
	};
}

#endif // !GLEAM_CORE_RENDER_LAYOUT_H_
