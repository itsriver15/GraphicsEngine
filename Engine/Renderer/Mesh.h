#pragma once
#include <vector>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
using namespace std;

namespace nu {
	class Mesh {
	public:
		Mesh() = default;
		Mesh(const vector<Vector2>& points, const Color& color) : m_points{ points }, m_color{ color } {};
		
		void SetPoints(const vector<Vector2>& points) {
			m_points = points;
		}
		const vector<Vector2>& GetPoints() const { return m_points; }

		void SetColor(const Color& color) { m_color = color; }
		const Color& GetColor() const { return m_color; }

		float GetRadius() const;

	private:
		vector<Vector2> m_points;
		Color m_color;

	};
}