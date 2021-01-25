#include "stdafx.h"
#include "EditorDesc.h"

#include "Util\IniData\IniFile.h"

Editor::EditorDesc::EditorDesc()
{
	mIniFile = new IniFile(PathEditorData(L"Editor.ini"));

	wstring isFirst = mIniFile->GetData(L"Main", L"IsFirst");
	if (isFirst != IniFile::False)
	{
		mIsCreateDemo = true;
		
		Save();
	}
	else
	{
		Load();
	}
}

Editor::EditorDesc::~EditorDesc()
{
	Save();
	SafeDelete(mIniFile);
}

void Editor::EditorDesc::Load()
{
	mIsCreateDemo = stoi(mIniFile->GetData(L"Main", L"IsCreateDemo"));
}

void Editor::EditorDesc::Save()
{
	mIniFile->AddData(L"Main", L"IsFirst", IniFile::False);
	mIniFile->AddData(L"Main", L"IsCreateDemo", to_wstring(mIsCreateDemo));

	mIniFile->Save();
}
