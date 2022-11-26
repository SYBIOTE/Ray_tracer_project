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


void Renderer::Render(const Camera& camera,glm::vec3 lightDir){
	Ray ray;
	ray.Origin = camera.GetPosition();
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
			glm::vec4 color = TraceRay(ray, lightDir);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			
			m_ImageData[x + y  * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);

		}
	} 
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray, glm::vec3 lightDir) {
	glm::vec3 sphereColor(1,1,1);
	float r = 0.5f;
	
	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - r * r;
	float d = b * b - 4.0f * a * c;

	if (d < 0.0f) // not hit
		return glm::vec4(0, 0, 0, 1);

	float fart = ((- b + glm::sqrt(d)) / (2.0f * a));
	float closet = ((- b - glm::sqrt(d)) / (2.0f * a));
	
	glm::vec3 closehitpoint = ray.Origin + ray.Direction * closet;
	glm::vec3 normal = glm::normalize(closehitpoint);
	float i = glm::max(glm::dot(normal, -glm::normalize(lightDir)), 0.0f);
	sphereColor *= (i + .1f);
	return glm::vec4(sphereColor, 1.0f);
}
