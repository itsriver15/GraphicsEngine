#pragma once
#include "../Renderer/Mesh.h"
using namespace std;

namespace nu {
	class Model {
	public:
		Model() = default;
		Model(const vector<Mesh>& meshes) : m_meshes{ meshes } {
			CalculateRadius();

		}

		void AddMesh(const Mesh& mesh) {
			m_meshes.push_back(mesh); 
			CalculateRadius();
		}

		void SetMeshes(const vector<Mesh>& meshes) {

			m_meshes = meshes;
			CalculateRadius();
		}

		const vector<Mesh>& GetMeshes() const {
			return m_meshes;
		}

		float GetRadius() const { return m_radius; }
		void CalculateRadius();


	private:
		float m_radius = 0.0f;
		vector<Mesh> m_meshes;
	};
}