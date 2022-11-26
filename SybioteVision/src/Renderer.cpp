#include "Renderer.h"
#include "Walnut/Random.h"
#include "vgMath.h"

namespace Utils {
	static uint32_t  ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() {
	return m_FinalImage;
	
}


void Renderer::OnResize(uint32_t width, uint32_t height) {

	if (m_FinalImage) {
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight())
			return;
		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}


void Renderer::Render(glm::vec3 lightDir){
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coor = { (float)x  / (float)m_FinalImage->GetWidth() , (float)y / (float)m_FinalImage->GetHeight() };
			coor = coor * 2.0f - 1.0f;
			glm::vec4 color = PerPixel(coor, lightDir);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y  * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);

		}
	} 
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coor, glm::vec3 lightDir) {
	glm::vec3 rayOrg(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDir(coor.x, coor.y, -1.0f);
	glm::vec3 sphereColor(1,1,1);
	float r = 0.5f;
	
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrg, rayDir);
	float c = glm::dot(rayOrg, rayOrg) - r * r;
	float d = b * b - 4.0f * a * c;

	if (d < 0.0f) // not hit
		return glm::vec4(0, 0, 0, 1);

	float fart = ((- b + glm::sqrt(d)) / (2.0f * a));
	float closet = ((- b - glm::sqrt(d)) / (2.0f * a));
	
	glm::vec3 closehitpoint = rayOrg + rayDir * closet;
	glm::vec3 normal = glm::normalize(closehitpoint);
	float i = glm::max(glm::dot(normal, -glm::normalize(lightDir)), 0.0f);
	sphereColor *= i;
	return glm::vec4(sphereColor, 1.0f);
}