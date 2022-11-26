#include "Utils.h"
#include <glm/glm.hpp>

namespace Utils {
	static uint32_t  ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
	static glm::vec3 VGMToGLM(const vgm::Vec3& v) {
		return glm::vec3(v.x, v.y, v.z);
	}
}