#include "pch.h"
#include "../Renderer/Model.h"

namespace nu {
	void Model::CalculateRadius() {
		m_radius = 0.0f;

		//check for mesh radius
		
		for (const auto& mesh : m_meshes) {
			//get mesh radius
			float radius = mesh.GetRadius();
			//if raidus is larger than current radius, radius is new raideus.
			if (radius > m_radius) m_radius = radius;
		}
	}
}
