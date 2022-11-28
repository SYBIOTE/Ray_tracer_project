#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include "imGuIZMOquat.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;
class CoreLayer : public Walnut::Layer
{
public:
	CoreLayer()
		:m_Camera(45.0f, .1f, 100.f), light(-1, -1, -1){


		Material& whitesphere = m_Scene.Materials.emplace_back();
		whitesphere.Albedo = { 1,1,1 };
		whitesphere.Roughness = .1;
		Material& bluesphere = m_Scene.Materials.emplace_back();
		bluesphere.Albedo = { .2,.3,1 };
		bluesphere.Roughness = 0;

		m_Scene.Spheres.push_back(Sphere{ {0,-201,0},{200}, 0});
		//m_Scene.Spheres.push_back(Sphere{ {1,0,-1} ,{0.5}, {{0,0,1}}  });
		m_Scene.Spheres.push_back(Sphere{ {0,1,-2}  ,{0.5}, 1});
		//m_Scene.Spheres.push_back(Sphere{ {-1,0,-3} ,{0.5}, {{0,1,0}} });
		
		

	} 
	virtual void OnUpdate(float delta) override {
		if (m_Camera.OnUpdate(delta)) {
			m_Renderer.ResetFrameIndex();
		}
	}
	virtual void OnUIRender() override{
		ImGui::Begin("Settings");  
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset")){
			m_Renderer.ResetFrameIndex();
		}
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		
		ImGui::Text("light direction");
		if (ImGui::gizmo3D("##Dir", light));
		

		ImGui::End();
		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);
			ImGui::Text("Object ID:%d", i);
			ImGui::DragFloat3("Position", glm::value_ptr(m_Scene.Spheres[i].Position), .1f);
			ImGui::DragFloat("Radius", &m_Scene.Spheres[i].Radius, .1f);
			ImGui::DragInt("Materials", &m_Scene.Spheres[i].MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size());
			ImGui::Separator();
			ImGui::PopID();
		}
		for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
			ImGui::PushID(i);
			ImGui::Text("Material ID:%d", i);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Materials[i].Albedo), .1f);
			ImGui::DragFloat("Roughness", &m_Scene.Materials[i].Roughness, .01f,0,1);
			ImGui::DragFloat("Metallic", &m_Scene.Materials[i].Metallic, .01f,0,1);
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		auto image = m_Renderer.GetFinalImage();
		if(image) 
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(),(float)image->GetHeight() },
						ImVec2(0,1), ImVec2(1,0));
 		ImGui::ShowDemoWindow();
		ImGui::End();
		ImGui::PopStyleVar();
		Render();
	}

	void Render() {
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene,m_Camera,light);

		m_LastRenderTime = timer.ElapsedMillis();
	}  

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	glm::vec3 light;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};



Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "SybioteVision";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<CoreLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}