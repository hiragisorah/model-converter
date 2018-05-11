#include "model-loader.h"

int main(int cnt, char** str)
{
	ModelLoader loader;

	for (int n = 1; n < cnt; ++n)
	{
		std::cout << str[n] << std::endl;

		std::string strx = str[n];

		if(!loader.Load(strx)) continue;

		strx.replace(strx.rfind("."), strx.size() - strx.rfind("."), ".sem");
		
		loader.Save(strx);
	}

	//std::string strx = "C:/Users/Selector/Documents/Visual Studio 2015/Projects/model-converter/model-converter/kaoru.fbx";

	//loader.Load(strx);

	//strx.replace(strx.rfind("."), strx.size() - strx.rfind("."), ".sem");

	//loader.Save(strx);

	//loader.LoadFromFile(strx);
	
	rewind(stdin);
	getchar();
	return 0;
}