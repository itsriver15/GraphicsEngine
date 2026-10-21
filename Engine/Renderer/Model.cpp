#include "pch.h"
#include "Model.h"
#include "Renderer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace nu
{
	bool Model::Load(const std::string& filename, const Renderer& renderer)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Could not load assimp file " << filename << ": " <<
				importer.GetErrorString() << std::endl;
			return false;
		}
		// todo: start processing the scene from the root node with ProcessNode(node, scene, renderer)
		// the root node is scene->mRootNode
		ProcessNode(scene->mRootNode, scene, renderer);
		return true;
	}

	void Model::Draw(Renderer& renderer) const
	{
		for (const auto& mesh : m_meshes)
		{
			renderer.SetVertexBuffer(*mesh.vertexBuffer);
			renderer.SetIndexBuffer(*mesh.indexBuffer);
			renderer.DrawIndex(*mesh.indexBuffer);
		}
	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene, const Renderer&
		renderer)
	{
		// process the current node meshes
		// a node only stores mesh indices, the actual meshes are stored in scene->mMeshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, renderer);
		}
		// process the current node children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, renderer);
		}
	}
	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const Renderer&
		renderer)
	{
		// get model vertex attributes
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			if (mesh->mTextureCoords[0] == nullptr)
			{
				vertex.texcoord = glm::vec2{ 0, 0 };
			}
			else
			{
				vertex.texcoord = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
			}

			if (mesh->mNormals == nullptr) {
				vertex.normal = glm::vec3{ 0, 0, 0 };
			}
			else {
				vertex.normal = glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			}

			if (mesh->mTangents == nullptr)
			{
				vertex.tangent = glm::vec3{ 0, 0, 0 };
			}
			else
			{
				vertex.tangent = glm::vec3{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
			}
			vertices.push_back(vertex);
		}
		// get model indices
		std::vector<uint32_t> indices;
		indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		// create the vertex buffer (uploads to the gpu)
		Mesh newMesh;
		newMesh.vertexBuffer = std::make_unique<VertexBuffer>();
		if (!newMesh.vertexBuffer->Create(vertices, renderer))
		{
			std::cerr << "Could not create vertex buffer for mesh " << mesh->mName.C_Str() << std::endl;
			return;
		}
		// create the index buffer (uploads to the gpu)
		newMesh.indexBuffer = std::make_unique<IndexBuffer>();
		if (!newMesh.indexBuffer->Create(indices, renderer))
		{
			std::cerr << "Could not create index buffer for mesh " << mesh->mName.C_Str() << std::endl;
			return;
		}
		//add newMesh to m_meshes
		m_meshes.push_back(std::move(newMesh));
	}
}
