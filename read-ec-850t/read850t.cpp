#include "Poco/Util/XMLConfiguration.h"
#include "Poco/RegularExpression.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/StreamCopier.h"
#include "Poco/Thread.h"
#include "Poco/File.h"
#include "Poco/Exception.h"


#include <iostream>
#include <vector>
#include <fstream>

void deal_path(const std::string &path) //处理目录
{
	Poco::DirectoryIterator pfile(path+"\\temper-p\\850");

	std::ofstream fout08("850t08.txt", std::ios_base::app), fout20("850t20.txt", std::ios_base::app);

	while(pfile.name().size()) 
	{
		if(pfile.name().substr(9,3) == "024")
		{
			std::cout<<pfile.path().toString()<<std::endl;


			std::ifstream fin(pfile.path().toString());

			std::string content;
			Poco::StreamCopier::copyToString(fin, content);
			fin.close();

			Poco::RegularExpression rg1("\\s112.50\\s+35.00\\s+0\\s+([\\d.-]+)");

			Poco::RegularExpression::MatchVec matchs;

			if(rg1.match(content, 0, matchs) )
			{
				std::string hour = pfile.name().substr(6,2),
					value = content.substr(matchs[1].offset, matchs[1].length);

				if(hour == "08")
				{
					fout08<<"20"+pfile.name().substr(0,6)+' '+value+"\n";
				}
				else if(hour == "20")
				{
					fout20<<"20"+pfile.name().substr(0,6)+' '+value+"\n";
				}
			}
		}

		pfile++;
	}
}


int main()
{
	while(true)
	{
		std::string path = "W:\\ecmwf";

		std::cout<<"请输入EC资料所在路径： ";
		std::cin>>path;

		deal_path(path);
	}

	return 0;
}