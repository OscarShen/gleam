#define _SCL_SECURE_NO_WARNINGS
#include "mesh.h"
#include <base/context.h>
#include <render/render_engine.h>
#include <util/hash.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <render/material.h>
#include <render/texture.h>
namespace gleam {

	class ModelLoadingDesc : public ResLoadingDesc
	{
	private:
		struct ModelDesc
		{
			struct ModelData
			{
				std::vector<MaterialPtr> mtls;
				std::vector<VertexElement> merged_elements;
				bool all_index_16_bit;
				std::vector<std::vector<uint8_t>> merged_buffer;
				std::vector<uint8_t> merged_indices;
				std::vector<GraphicsBufferPtr> merged_vbs;
				GraphicsBufferPtr merged_ib;
				std::vector<std::string> mesh_names;
				std::vector<int32_t> mtl_ids;
				std::vector<uint32_t> mesh_num_vertices;
				std::vector<uint32_t> mesh_start_vertices;
				std::vector<uint32_t> mesh_num_indices;
				std::vector<uint32_t> mesh_start_indices;
			};

			std::string res_name;
			uint32_t access_hint;
			std::shared_ptr<ModelData> model_data;
			std::shared_ptr<Model> model; 
			std::function<ModelPtr(const std::string &)> CreateModelFunc;
			std::function<MeshPtr(const std::string &, const ModelPtr &)> CreateMeshFunc;
		};

	public:
		ModelLoadingDesc(const std::string &res_name, uint32_t access_hint,
			std::function<ModelPtr(const std::string &)> create_model_func,
			std::function<MeshPtr(const std::string &, const ModelPtr &)> create_mesh_func)
		{
			model_desc_.res_name = res_name;
			model_desc_.access_hint = access_hint;
			model_desc_.CreateModelFunc = create_model_func;
			model_desc_.CreateMeshFunc = create_mesh_func;
			model_desc_.model_data = std::make_shared<ModelDesc::ModelData>();
		}

		std::shared_ptr<void> CreateResource()
		{
			ModelPtr model = model_desc_.CreateModelFunc("Model");
			model_desc_.model = model;
			return model;
		}

		uint64_t Type() const override
		{
			static const uint64_t type = CT_HASH("ModelLoadingDesc");
			return type;
		}

		void Load() override
		{
			const ModelPtr &model = model_desc_.model;

			CHECK_INFO(LoadModel(
				model_desc_.res_name,
				model_desc_.model_data->mtls,
				model_desc_.model_data->merged_elements,
				model_desc_.model_data->merged_buffer,
				model_desc_.model_data->merged_indices,
				model_desc_.model_data->mesh_names,
				model_desc_.model_data->mtl_ids,
				model_desc_.model_data->mesh_num_vertices,
				model_desc_.model_data->mesh_start_vertices,
				model_desc_.model_data->mesh_num_indices,
				model_desc_.model_data->mesh_start_indices), "Load model failed : " << model_desc_.res_name);

			this->AssignModelInfo();

			// create resource
			for (size_t i = 0; i < model_desc_.model_data->merged_buffer.size(); ++i)
			{
				model_desc_.model_data->merged_vbs[i]->CreateResource(&model_desc_.model_data->merged_buffer[i][0]);
			}
			model_desc_.model_data->merged_ib->CreateResource(&model_desc_.model_data->merged_indices[0]);

			this->AddSubPath();

			model->LoadModelInfo();

			for (uint32_t i = 0; i < model->NumSubrenderables(); ++i)
			{
				checked_pointer_cast<Mesh>(model->Subrenderable(i))->LoadMeshInfo();
			}

			// release resources in Host
			model_desc_.model_data.reset();
		}

		bool Match(const ResLoadingDesc &rhs) const override
		{
			if (this->Type() == rhs.Type())
			{
				const ModelLoadingDesc &desc = static_cast<const ModelLoadingDesc &>(rhs);
				return model_desc_.res_name == desc.model_desc_.res_name &&
					model_desc_.access_hint == desc.model_desc_.access_hint;
			}
			return false;
		}

		std::shared_ptr<void> Resource() const override
		{
			return model_desc_.model;
		}

	private:
		void AssignModelInfo()
		{
			RenderEngine &re = Context::Instance().RenderEngineInstance();
			const ModelPtr &model = model_desc_.model;

			// materials
			model->NumMaterials(model_desc_.model_data->mtls.size());
			for (size_t mtl_index = 0; mtl_index < model_desc_.model_data->mtls.size(); ++mtl_index)
			{
				model->GetMaterial(static_cast<int32_t>(mtl_index)) = model_desc_.model_data->mtls[mtl_index];
			}

			// vertices, indices
			model_desc_.model_data->merged_vbs.resize(model_desc_.model_data->merged_buffer.size());
			for (size_t i = 0; i < model_desc_.model_data->merged_buffer.size(); ++i)
			{
				model_desc_.model_data->merged_vbs[i] = re.MakeVertexBufferHandler(BU_Static, model_desc_.access_hint,
					static_cast<uint32_t>(model_desc_.model_data->merged_buffer[i].size()));
			}
			model_desc_.model_data->merged_ib = re.MakeIndexBufferHandler(BU_Static, model_desc_.access_hint,
				static_cast<uint32_t>(model_desc_.model_data->merged_indices.size()));

			// meshes
			std::vector<MeshPtr> meshes(model_desc_.model_data->mesh_names.size());
			for (uint32_t mesh_index = 0; mesh_index < model_desc_.model_data->mesh_names.size(); ++mesh_index)
			{
				meshes[mesh_index] = model_desc_.CreateMeshFunc(model_desc_.model_data->mesh_names[mesh_index], model);
				MeshPtr &mesh = meshes[mesh_index];

				mesh->MaterialID(model_desc_.model_data->mtl_ids[mesh_index]);

				for (uint32_t buffer_index = 0; buffer_index < model_desc_.model_data->merged_buffer.size(); ++buffer_index)
				{
					mesh->AddVertexStream(model_desc_.model_data->merged_vbs[buffer_index], model_desc_.model_data->merged_elements[buffer_index]);
				}
				mesh->AddIndexStream(model_desc_.model_data->merged_ib, EF_R32UI);

				mesh->NumVertices(model_desc_.model_data->mesh_num_vertices[mesh_index]);
				mesh->NumIndices(model_desc_.model_data->mesh_num_indices[mesh_index]);

				mesh->StartVertexLocation(model_desc_.model_data->mesh_start_vertices[mesh_index]);
				mesh->StartIndexLocation(model_desc_.model_data->mesh_start_indices[mesh_index]);

			}

			model->AssignSubrenderable(meshes.begin(), meshes.end());
		}

		void AddSubPath()
		{
			std::string sub_path;
			auto sub_path_loc = model_desc_.res_name.find_last_of('/');
			if (sub_path_loc != std::string::npos)
			{
				sub_path = ResLoader::Instance().Locate(model_desc_.res_name.substr(0, sub_path_loc));
				if (!sub_path.empty())
				{
					ResLoader::Instance().AddPath(sub_path);
				}
			}
		}

	private:
		ModelDesc model_desc_;
	};

	Mesh::Mesh(const std::string & name, const ModelPtr & model)
		:model_ptr_(model), name_(name), mtl_id_(-1)
	{
		layout_ = Context::Instance().RenderEngineInstance().MakeRenderLayout();
		layout_->TopologyType(TT_TriangleList);
	}

	void Mesh::LoadMeshInfo()
	{
		this->DoLoadMeshInfo();
	}

	void Mesh::AddVertexStream(const void * buffer, uint32_t size, const VertexElement & element, uint32_t access_hint)
	{
		GraphicsBufferPtr gbuffer = Context::Instance().RenderEngineInstance().MakeVertexBuffer(BU_Static, access_hint, size, buffer);
		this->AddVertexStream(gbuffer, element);
	}

	void Mesh::AddVertexStream(const GraphicsBufferPtr & buffer, const VertexElement & element)
	{
		layout_->BindVertexStream(buffer, element);
	}

	void Mesh::AddIndexStream(const void * buffer, uint32_t size, ElementFormat format, uint32_t access_hint)
	{
		GraphicsBufferPtr gbuffer = Context::Instance().RenderEngineInstance().MakeIndexBuffer(BU_Static, access_hint, size, buffer);
		this->AddIndexStream(gbuffer, format);
	}

	void Mesh::AddIndexStream(const GraphicsBufferPtr & buffer, ElementFormat format)
	{
		layout_->BindIndexStream(buffer, format);
	}

	void Mesh::DoLoadMeshInfo()
	{
		if (this->MaterialID() >= 0) {
			ModelPtr model = model_ptr_.lock();
			mtl_ = model->GetMaterial(this->MaterialID()); 

			for (size_t i = 0; i < TS_NumTextureSlots; ++i)
			{
				if (!mtl_->tex_names[i].empty())
				{
					textures_[i] = LoadTexture(mtl_->tex_names[i], EAH_GPU_Read | EAH_Immutable);
				}
			}
		}
	}

	Model::Model(const std::string & name)
		: name_(name)
	{
	}

	void Model::LoadModelInfo()
	{
		this->DoLoadModelInfo();
	}

	void Model::OnRenderBegin()
	{
		for (const auto &mesh : subrenderables_)
		{
			mesh->OnRenderBegin();
		}
	}

	void Model::OnRenderEnd()
	{
		for (const auto &mesh : subrenderables_)
		{
			mesh->OnRenderEnd();
		}
	}

	void Model::ModelMatrix(const glm::mat4 & model)
	{
		for (const auto &mesh : subrenderables_)
		{
			checked_pointer_cast<Mesh>(mesh)->ModelMatrix(model);
		}
	}

	void Model::AddToRenderQueue()
	{
		for (const auto &mesh : subrenderables_)
		{
			mesh->AddToRenderQueue();
		}
	}

	struct AssimpMesh
	{
		std::string				name;
		std::vector<glm::vec3>	vertices;
		std::vector<glm::vec3>	normals;
		std::vector<glm::vec2>	texCoords;
		std::vector<glm::vec3>	tangents;
		std::vector<glm::vec3>	bitangents;
		std::vector<uint32_t>	indices;
		uint32_t				mtlIndex;
	};

	struct AssimpMaterial
	{
		enum AssimpTextureSlot
		{
			ATS_Albedo = 0,
			ATS_Metalness,
			ATS_Glossiness,
			ATS_Emissive,
			ATS_Normal,
			ATS_Height,

			ATS_NumTextureSlots
		};
		uint32_t	mtl_id;
		std::string name;

		glm::vec4	albedo;
		float		metalness;
		float		glossiness;
		glm::vec3	emissive;
		bool		transparent;
		bool		tow_sided;
		std::array<std::string, ATS_NumTextureSlots> tex_name;
	};

	void ProcessMaterial(const aiScene *scene, std::vector<AssimpMaterial> &aMaterial)
	{
		for (decltype(scene->mNumMaterials) index = 0; index < scene->mNumMaterials; ++index)
		{
			uint32_t mtl_id = static_cast<uint32_t>(aMaterial.size());
			aMaterial.emplace_back();

			aiMaterial *mtl = scene->mMaterials[index];

			std::string name;
			glm::vec3 albedo(0, 0, 0);
			float metalness = 0;
			float shininess = 1;
			glm::vec3 emissive(0, 0, 0);
			float opacity = 1;
			bool transparent = false;
			bool two_sided = false;

			aiString ai_name;
			aiColor4D ai_albedo;
			float ai_opacity;
			float ai_shininess;
			aiColor4D ai_emissive;
			int ai_two_sided;

			if (AI_SUCCESS == aiGetMaterialString(mtl, AI_MATKEY_NAME, &ai_name))
			{
				name = ai_name.C_Str();
			}
			if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &ai_albedo))
			{
				albedo = glm::vec3(ai_albedo.r, ai_albedo.g, ai_albedo.b);
			}
			if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &ai_emissive))
			{
				emissive = glm::vec3(ai_emissive.r, ai_emissive.g, ai_emissive.b);
			}

			unsigned int max = 1;
			if (AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_OPACITY, &ai_opacity, &max))
			{
				opacity = ai_opacity;
			}

			max = 1;
			if (AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &ai_shininess, &max))
			{
				shininess = ai_shininess;

				max = 1;
				float strength;
				if (AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max))
				{
					shininess *= strength;
				}
			}
			shininess = glm::clamp(shininess, 1.0f, MAX_SHININESS);

			if ((opacity < 1) || (aiGetMaterialTextureCount(mtl, aiTextureType_OPACITY) > 0))
			{
				transparent = true;
			}

			max = 1;
			if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &ai_two_sided, &max))
			{
				two_sided = ai_two_sided ? true : false;
			}

			aMaterial[mtl_id].mtl_id = mtl_id;
			aMaterial[mtl_id].name = name;
			aMaterial[mtl_id].albedo = glm::vec4(albedo, opacity);
			aMaterial[mtl_id].metalness = metalness;
			aMaterial[mtl_id].glossiness = Shininess2Glossiness(shininess);
			aMaterial[mtl_id].emissive = emissive;
			aMaterial[mtl_id].transparent = transparent;
			aMaterial[mtl_id].tow_sided = two_sided;

			// textures
			unsigned int count = aiGetMaterialTextureCount(mtl, aiTextureType_DIFFUSE);
			if (count > 0)
			{
				aiString str;
				aiGetMaterialTexture(mtl, aiTextureType_DIFFUSE, 0, &str, 0, 0, 0, 0, 0, 0);

				aMaterial[mtl_id].tex_name[AssimpMaterial::ATS_Albedo] = str.C_Str();
			}

			count = aiGetMaterialTextureCount(mtl, aiTextureType_SHININESS);
			if (count > 0)
			{
				aiString str;
				aiGetMaterialTexture(mtl, aiTextureType_SHININESS, 0, &str, 0, 0, 0, 0, 0, 0);

				aMaterial[mtl_id].tex_name[AssimpMaterial::ATS_Glossiness] = str.C_Str();
			}

			count = aiGetMaterialTextureCount(mtl, aiTextureType_EMISSIVE);
			if (count > 0)
			{
				aiString str;
				aiGetMaterialTexture(mtl, aiTextureType_EMISSIVE, 0, &str, 0, 0, 0, 0, 0, 0);

				aMaterial[mtl_id].tex_name[AssimpMaterial::ATS_Emissive] = str.C_Str();
			}

			count = aiGetMaterialTextureCount(mtl, aiTextureType_NORMALS);
			if (count > 0)
			{
				aiString str;
				aiGetMaterialTexture(mtl, aiTextureType_NORMALS, 0, &str, 0, 0, 0, 0, 0, 0);

				aMaterial[mtl_id].tex_name[AssimpMaterial::ATS_Normal] = str.C_Str();
			}

			count = aiGetMaterialTextureCount(mtl, aiTextureType_HEIGHT);
			if (count > 0)
			{
				aiString str;
				aiGetMaterialTexture(mtl, aiTextureType_HEIGHT, 0, &str, 0, 0, 0, 0, 0, 0);

				aMaterial[mtl_id].tex_name[AssimpMaterial::ATS_Height] = str.C_Str();
			}
		}
	}

	void ProcessMesh(aiMesh *mesh, const aiScene *scene, AssimpMesh *aMesh)
	{
		aMesh->name = mesh->mName.C_Str();

		aMesh->vertices.reserve(mesh->mNumVertices);
		aMesh->normals.reserve(mesh->mNumVertices);
		aMesh->texCoords.reserve(mesh->mNumVertices);
		aMesh->tangents.reserve(mesh->mNumVertices);

		bool has_normal = (mesh->mNormals != nullptr);
		bool has_tangent = (mesh->mTangents != nullptr);
		bool has_bitangent = (mesh->mBitangents != nullptr);
		bool has_texCoord = (mesh->mTextureCoords[0] != nullptr); // only use texCoord0
		assert(has_normal && has_tangent && has_bitangent);
		for (decltype(mesh->mNumVertices) i = 0; i < mesh->mNumVertices; ++i)
		{
			// Assimp will generate normals, uvCoords, tangents, binormal if these aren't contained in model file...
			aMesh->vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			aMesh->normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			aMesh->tangents.emplace_back(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			aMesh->bitangents.emplace_back(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			if(has_texCoord)
				aMesh->texCoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}

		// triangulate
		aMesh->indices.reserve(mesh->mNumFaces * 3);
		for (decltype(mesh->mNumFaces) i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace &face = mesh->mFaces[i];
			for (decltype(face.mNumIndices) j = 0; j < face.mNumIndices; ++j)
			{
				aMesh->indices.push_back(static_cast<uint32_t>(face.mIndices[j]));
			}
		}

		aMesh->mtlIndex = static_cast<uint32_t>(mesh->mMaterialIndex);
	}

	void ProcessNode(aiNode *node, const aiScene *scene, std::vector<AssimpMesh> &meshes)
	{
		for (decltype(node->mNumMeshes) i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back();
			ProcessMesh(mesh, scene, &meshes.back());
		}

		for (decltype(node->mNumChildren) i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, meshes);
		}
	}

	bool LoadModel(const std::string & name, std::vector<MaterialPtr>& mtls, std::vector<VertexElement>& merged_ves, std::vector<std::vector<uint8_t>>& merged_buff, std::vector<uint8_t>& merged_indices, std::vector<std::string>& mesh_names, std::vector<int32_t>& mtl_ids, std::vector<uint32_t>& mesh_num_vertices, std::vector<uint32_t> &mesh_start_vertices,
		std::vector<uint32_t>& mesh_num_indices, std::vector<uint32_t> &mesh_start_indices)
	{
		std::string file_name = ResLoader::Instance().Locate(name);

		if (file_name.empty())
		{
			CHECK_INFO(false, "can't locate model file : " << name);
			return false;
		}

		const aiScene *scene = aiImportFile(file_name.c_str(), aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
		
		CHECK_INFO(scene, aiGetErrorString());
		
		std::vector<AssimpMesh> meshes;
		std::vector<AssimpMaterial> materials;
		ProcessNode(scene->mRootNode, scene, meshes);
		ProcessMaterial(scene, materials);

		assert(meshes.size() > 0);

		bool has_texCoord = meshes[0].texCoords.size() == meshes[0].vertices.size();
		uint32_t num_merged_ves = 4;
		if (has_texCoord)
		{
			++num_merged_ves;
		}

		merged_ves.resize(num_merged_ves);
		merged_ves[0].usage = VEU_Position;
		merged_ves[0].format = EF_BGR32F;
		merged_ves[0].usage_index = 0;
		merged_ves[1].usage = VEU_Normal;
		merged_ves[1].format = EF_BGR32F;
		merged_ves[1].usage_index = 0;
		merged_ves[2].usage = VEU_Tangent;
		merged_ves[2].format = EF_BGR32F;
		merged_ves[2].usage_index = 0;
		merged_ves[3].usage = VEU_Binormal;
		merged_ves[3].format = EF_BGR32F;
		merged_ves[3].usage_index = 0;
		if (has_texCoord) {
			merged_ves[4].usage = VEU_TextureCoord;
			merged_ves[4].format = EF_GR32F;
			merged_ves[4].usage_index = 0;
		}

		uint32_t all_num_vertices = 0, all_num_indices = 0;
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			all_num_vertices += static_cast<uint32_t>(meshes[i].vertices.size());
			all_num_indices += static_cast<uint32_t>(meshes[i].indices.size());
		}

		merged_buff.resize(merged_ves.size());
		for (size_t i = 0; i < merged_buff.size(); ++i)
		{
			merged_buff[i].resize(all_num_vertices * merged_ves[i].NumFormatBytes());
		}

		uint8_t *cur_vertices   = merged_buff[0].data();
		uint8_t *cur_normals    = merged_buff[1].data();
		uint8_t *cur_tangents	= merged_buff[2].data();
		uint8_t *cur_bitangents = merged_buff[3].data();
		uint8_t *cur_texCoords  = merged_buff[4].data();
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			size_t stride = meshes[i].vertices.size();
			std::copy((uint8_t*)meshes[i].vertices.data(),		(uint8_t*)(&meshes[i].vertices.back() + 1),		cur_vertices);
			std::copy((uint8_t*)meshes[i].normals.data(),		(uint8_t*)(&meshes[i].normals.back() + 1),		cur_normals);
			std::copy((uint8_t*)meshes[i].tangents.data(),		(uint8_t*)(&meshes[i].tangents.back() + 1),		cur_tangents);
			std::copy((uint8_t*)meshes[i].bitangents.data(),	(uint8_t*)(&meshes[i].bitangents.back() + 1),	cur_bitangents);
			cur_vertices	+=	stride * merged_ves[0].NumFormatBytes();
			cur_normals		+=	stride * merged_ves[1].NumFormatBytes();
			cur_tangents	+=	stride * merged_ves[2].NumFormatBytes();
			cur_bitangents	+=	stride * merged_ves[3].NumFormatBytes();
			if (has_texCoord)
			{
				std::copy((uint8_t*)meshes[i].texCoords.data(), (uint8_t*)(&meshes[i].texCoords.back() + 1), cur_texCoords);
				cur_texCoords += stride * merged_ves[4].NumFormatBytes();
			}
		}

		merged_indices.resize(all_num_indices * sizeof(uint32_t));
		uint8_t *cur_indices = merged_indices.data();
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			std::copy((uint8_t*)(meshes[i].indices.data()), (uint8_t*)(&meshes[i].indices.back() + 1), cur_indices);
			cur_indices += meshes[i].indices.size() * sizeof(uint32_t);
		}

		uint32_t num_meshes = static_cast<uint32_t>(meshes.size());
		mesh_names.resize(num_meshes);
		mtl_ids.resize(num_meshes);
		mesh_num_vertices.resize(num_meshes);
		mesh_start_vertices.resize(num_meshes);
		mesh_num_indices.resize(num_meshes);
		mesh_start_indices.resize(num_meshes);
		uint32_t start_vertices = 0, start_indices = 0;
		for (uint32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index)
		{
			mesh_names[mesh_index] = meshes[mesh_index].name;
			mtl_ids[mesh_index] = meshes[mesh_index].mtlIndex;

			mesh_num_vertices[mesh_index] = static_cast<uint32_t>(meshes[mesh_index].vertices.size());
			mesh_start_vertices[mesh_index] = start_vertices;
			start_vertices += mesh_num_vertices[mesh_index];

			mesh_num_indices[mesh_index] = static_cast<uint32_t>(meshes[mesh_index].indices.size());
			mesh_start_indices[mesh_index] = start_indices;
			start_indices += mesh_num_indices[mesh_index];
		}

		uint32_t num_materials = static_cast<uint32_t>(materials.size());
		mtls.resize(num_materials);
		for (uint32_t i = 0; i < num_materials; ++i)
		{
			mtls[i] = std::make_shared<Material>();
			mtls[i]->name = materials[i].name;
			mtls[i]->albedo = materials[i].albedo;
			mtls[i]->emissive = materials[i].emissive;
			mtls[i]->glossiness = materials[i].glossiness;
			mtls[i]->metalness = materials[i].metalness;
			// TextureSlot should relate to AssimpTextureSlot
			mtls[i]->tex_names = materials[i].tex_name;
			mtls[i]->transparent = materials[i].transparent;
			mtls[i]->two_sided = materials[i].tow_sided;
		}

		return true;
	}
	ModelPtr LoadModel(const std::string & name, uint32_t access_hint, std::function<ModelPtr(const std::string &)> create_model_func, std::function<MeshPtr(const std::string &, const ModelPtr &)> create_mesh_func)
	{
		return ResLoader::Instance().QueryT<Model>(std::make_shared<ModelLoadingDesc>(name, access_hint, create_model_func, create_mesh_func));
	}
}
