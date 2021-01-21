#pragma once
class IniFile final
{
	struct IniData
	{
		wstring Subject;	//항목
		wstring Title;	//제목
		wstring Body;	//값

		IniData() {}
		IniData(const wstring& subject, const wstring& title, const wstring& body)
			:Subject(subject), Title(title), Body(body) {}

		bool operator==(const IniData& other)
		{
			if (Subject == other.Subject &&
				Title == other.Title &&
				Body == other.Body)
				return true;
			return false;
		}
	};
private:
	wstring mFileName;
	unordered_map<wstring,vector<IniData>> mDataList;
	typedef unordered_map<wstring, vector<IniData>>::iterator IniIter;
public:
	IniFile(const wstring& fileName);

	void AddData(const wstring& subject, const wstring& title, const wstring& body);
	void RemoveData(const wstring& subject, const wstring& title);
	wstring GetData(const wstring& subject, const wstring& title);

	void Save();
	void Clear();
};

