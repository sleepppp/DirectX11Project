#pragma once
#include "./Util/Singleton/TSingleton.h"
#include "./System\Editor\Common\EditorCommon.h"

namespace Editor
{
	class EditorBase;

	class EditorMain : public TSingleton<EditorMain>
	{
		typedef unordered_map<EditorType, vector<EditorMain>> EditorContainer;
		typedef EditorContainer::iterator EditorIter;
	private:
		class EditorDesc* mEditorDesc;
		EditorContainer mEditorContainer;
	public:
		void Awake();
		void Start();
		void Update();
		void OnGui();
		void OnDestroy();

		EditorDesc* GetEditorDesc()const { return mEditorDesc; }
	};
}

