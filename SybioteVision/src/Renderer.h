#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer {
public:
	Renderer() = default;
	void OnResize(uint32_t width, uint32_t height);
	void Render(glm::vec3 lightDir);
	std::shared_ptr<Walnut::Image>  GetFinalImage();
private:
	glm::vec4 PerPixel(glm::vec2 coor, glm::vec3 lightDir);
	std::shared_ptr<Walnut::Image> m_FinalImage;
	
	uint32_t* m_ImageData = nullptr;
};