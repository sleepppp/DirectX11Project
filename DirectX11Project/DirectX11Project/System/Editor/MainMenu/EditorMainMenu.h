#pragma once
#include "./System/Editor/Base/EditorBase.h"
namespace Editor
{
	class EditorMainMenu : public EditorBase
	{
	protected:
		
	protected:
		friend class EditorMain;
		EditorMainMenu();
		void OnGui()override;
	};
}

