#include "stdafx.h"
#include "Program.h"

#include "./System/Editor/EditorMain/EditorMain.h"

void Program::Awake()
{
	D3D::GetInstance()->Init();

	ImGui::Create(WindowDesc::GetHwnd(), D3DDevice, D3DDeviceContext);
	ImGui::StyleColorsDark();
}

void Program::Start()
{

}

void Program::PreUpdate()
{
	Input::GetInstance()->Update();
}

void Program::Update()
{

}

void Program::PostUpdate()
{
	ImGui::Update();
}

void Program::PreRender()
{
	D3D::GetInstance()->BeginRender();
}

void Program::Render()
{

}
void Program::PostRender()
{
	ImGui::ShowDemoWindow();
	ImGui::Render();
	D3D::GetInstance()->EndRender();
}

void Program::OnDestroy()
{
	EditorMain::ReleaseInstance();

	ImGui::Delete();
}

