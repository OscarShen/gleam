/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_MESH_H_
#define GLEAM_CORE_RENDER_MESH_H_

#include "renderable.h"
namespace gleam
{
	class Mesh : public Renderable
	{
	public:
		Mesh(const std::string &name, const ModelPtr &model);

		void LoadMeshInfo();

		RenderLayout &GetRenderLayout() const override { return *layout_; }

		void NumVertices(uint32_t n) { layout_->NumVertices(n); }
		uint32_t NumVertices() const { return layout_->NumVertices(); }

		void NumIndices(uint32_t n) { layout_->NumIndices(n); }
		uint32_t NumIndices() const { return layout_->NumIndices(); }

		void AddVertexStream(const void *buffer, uint32_t size, const VertexElement &element, uint32_t access_hint);
		void AddVertexStream(const GraphicsBufferPtr &buffer, const VertexElement &element);

		void AddIndexStream(const void *buffer, uint32_t size, ElementFormat format, uint32_t access_hint);
		void AddIndexStream(const GraphicsBufferPtr &buffer, ElementFormat format);

		int32_t MaterialID() const { return mtl_id_; }
		void MaterialID(int32_t mtl_id) { mtl_id_ = mtl_id; }

	protected:
		virtual void DoLoadMeshInfo();

	protected:
		std::string name_;

		RenderLayoutPtr layout_;

		int32_t mtl_id_;

		std::weak_ptr<Model> model_;
	};

	class Model : public Renderable
	{
	public:
		void LoadModelInfo();

		RenderLayout &GetRenderLayout() const override { return *layout_; }

		void OnRenderBegin() override;
		void OnRenderEnd() override;

		void NumMaterials(size_t i) { materials_.resize(i); }
		size_t NumMaterials() const { return materials_.size(); }
		MaterialPtr &GetMaterial(int32_t index) { return materials_[index]; }
		const MaterialPtr &GetMaterial(int32_t index) const { return materials_[index]; }

		void AddToRenderQueue() override;

	protected:
		virtual void DoLoadModelInfo() { }

	protected:
		RenderLayoutPtr layout_;

		std::vector<MaterialPtr> materials_;
	};

	bool LoadModel(const std::string &name, std::vector<MaterialPtr> &mtls,
		std::vector<VertexElement>& merged_ves,
		std::vector<std::vector<uint8_t>>& merged_buff, std::vector<uint8_t>& merged_indices,
		std::vector<std::string>& mesh_names, std::vector<int32_t>& mtl_ids,
		std::vector<uint32_t>& mesh_num_vertices, std::vector<uint32_t> &mesh_start_vertices,
		std::vector<uint32_t>& mesh_num_indices, std::vector<uint32_t> &mesh_start_indices
		);
}

#endif // !GLEAM_CORE_RENDER_MESH_H_
