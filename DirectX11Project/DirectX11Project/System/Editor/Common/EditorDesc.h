#pragma once
namespace Editor
{
	class EditorDesc
	{
		class IniFile* mIniFile;

		Synthesize(bool,mIsCreateDemo,IsCreateDemo)
	public:
		EditorDesc();
		~EditorDesc();

		void Load();
		void Save();
	};
}

