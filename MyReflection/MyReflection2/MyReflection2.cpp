
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

string FindClassName(const vector<string> buffers)
{
	string result;
	for (int i = 0; i < buffers.size(); ++i)
	{

	}
	return result;
}

int main()
{
	// {{ Read Class h File ~
	vector<string> bufferList;
	ifstream inputStream("Test.h");
	if (inputStream.is_open())
	{
		string buffer;
		while (inputStream.peek() != EOF)
		{
			getline(inputStream, buffer);
			bufferList.push_back(buffer);
		}
	}
	inputStream.close();
	// }}

	cout << FindClassName(bufferList) << endl;
	
}