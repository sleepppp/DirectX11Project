#include "stdafx.h"
#include "EditorMain.h"

#include "./System/Editor/Common/EditorDesc.h"
void Editor::EditorMain::Awake()
{
	mEditorDesc = new EditorDesc;
}

void Editor::EditorMain::Start()
{
}

void Editor::EditorMain::Update()
{
}

void Editor::EditorMain::OnGui()
{
	if(mEditorDesc->GetIsCreateDemo())
		ImGui::ShowDemoWindow();
}

void Editor::EditorMain::OnDestroy()
{
	SafeDelete(mEditorDesc);
}
