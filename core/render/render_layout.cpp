#include "render_layout.h"
#include "graphics_buffer.h"
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
			instance_stream_.stream = buffer;
			instance_stream_.format = vet.ToVector();
			instance_stream_.vertex_size = bytes;
			instance_stream_.type = type;
			instance_stream_.freq = freq;
		}

		streams_dirty_ = true;
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
		start_index_location_ = location;
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
}
