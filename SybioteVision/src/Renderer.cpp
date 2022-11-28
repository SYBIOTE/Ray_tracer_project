#include "Renderer.h"
#include "Walnut/Random.h"
#include "vgMath.h"
#include <vector>
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

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

}


void Renderer::Render(const Scene& scene,const Camera& camera,glm::vec3& lightDir){
	
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
	

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
		 
			glm::vec4 color = PerPixel(x,y,lightDir);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;
			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedColor /= (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y  * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);

		}
	} 

	m_FinalImage->SetData(m_ImageData);
	
	if (m_Settings.Accumulate) 
		m_FrameIndex ++;
	else
		m_FrameIndex = 1;
}


glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y, glm::vec3& lightDir)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	
	glm::vec3 color(0.0f);
	float multiplier = 1.0f;
	int bounces = 2;
	
	for (int i = 0; i < bounces; i++) {
		
		Renderer::HitPayload payload = TraceRay(ray);

		if (payload.Hitdistance < 0.0f) {
			glm::vec3 skyColor = glm::vec3(0.6f,0.7f,0.9f);
			color += skyColor * multiplier;
			break;
		}
		
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		float intensity = glm::max(glm::dot(payload.WorldNormal, -glm::normalize(lightDir)), 0.0f);
		glm::vec3 sphereColor = material.Albedo;
		sphereColor *= (intensity+.1);
		color += sphereColor * multiplier;
		
		multiplier *= .7f;
		
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction,
			payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-.5f,.5f)); 
	}
	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray) {
	
	int closestSphere = -1;
	float hitDistance = FLT_MAX;
	for (size_t i = 0; i < m_ActiveScene->Spheres.size() ; i++) {
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 org = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(org, ray.Direction);
		float c = glm::dot(org, org) - sphere.Radius * sphere.Radius;
		float d = b * b - 4.0f * a * c;

		if (d < 0.0f) // not hit
			continue;

		float fart = ((-b + glm::sqrt(d)) / (2.0f * a));
		float closet = ((-b - glm::sqrt(d)) / (2.0f * a));
		
		if (closet > 0.0f && closet < hitDistance) {
			hitDistance = closet;
			closestSphere = i;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance , closestSphere);
	
} 


Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.Hitdistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene -> Spheres[objectIndex];
	glm::vec3 org = ray.Origin - closestSphere.Position;

	payload.WorldPosition = org + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);
	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.Hitdistance = -1.0f;
	return payload;
}