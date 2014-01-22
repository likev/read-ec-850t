#include "Poco/Util/XMLConfiguration.h"
#include "Poco/RegularExpression.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/StreamCopier.h"
#include "Poco/Thread.h"
#include "Poco/File.h"
#include "Poco/Exception.h"
#include "Poco/Glob.h"
#include "Poco/DateTime.h"
#include "Poco/TimeSpan.h"
#include <Poco/DateTimeFormatter.h>

#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>

template <typename T>
T string_to(const std::string & org)
{
	T des = 0;
	std::istringstream ssin(org);

	ssin>>des;
	return des;
}

void get_station_name(std::set<std::string> & names)
{
	std::set<std::string> files;
	Poco::Glob::glob("E:/cppcode/read-ec-850t/read-A-file/ly/A*.TXT", files);

	std::set<std::string>::iterator it = files.begin();

	Poco::RegularExpression rg("^[A-Za-z0-9]+");

	names.clear();

	for (; it != files.end(); ++it)
	{
		Poco::Path path(*it);

		std::string name;
		rg.extract(path.getFileName(), name);

		names.insert(name);

		std::cout << name << std::ends;
	}
}

//提取A文件中的雨量数据
void deal_files_rain(const std::set<std::string> & names)
{
	std::set<std::string>::const_iterator it = names.begin();

	for (; it != names.end(); ++it)
	{

		Poco::DateTime begin(2008,1,1), end(2012,12,31);

		std::ofstream fout("single-A-out/" + *it + ".txt");

		for(; begin <= end; 
			begin += Poco::Timespan(Poco::DateTime::daysOfMonth(begin.year(), begin.month()),0,0,0,0) )
		{
			std::string daymonth = Poco::DateTimeFormatter::format(begin, "%Y%m");
			Poco::File file("E:/cppcode/read-ec-850t/read-A-file/single-A/"+ *it +'-'+ daymonth + ".TXT");
			
			std::cout<<file.path()<<std::endl;
			if( file.exists() ) 
			{
				std::ifstream fin(file.path() );
				
				std::string content, rain;
				Poco::StreamCopier::copyToString(fin, content);

				Poco::RegularExpression rg("^R6[^=]+", Poco::RegularExpression::RE_MULTILINE);

				rg.extract(content, rain);

				std::istringstream ssin(rain);

				std::string linestr, valstr;

				std::getline(ssin, linestr);

				Poco::DateTime aday(begin);
				while(std::getline(ssin, linestr) )
				{
					std::string daystr = Poco::DateTimeFormatter::format(aday, "%Y-%m-%d");

					std::istringstream ssin2(linestr);
					int r08, r20, r24;
					
					fout<<daystr<<' ';
					if(ssin2>>r08>>r20>>r24)
					{
						fout<<r08<<' '<<r20<<' '<<r24;
					}
					else
					{
						fout<<linestr;
					}
					fout<<std::endl;

					aday += Poco::Timespan(1,0,0,0,0);
				}
			}
		}
		fout.close();

	}

}

//提取A文件中的风速数据
void deal_files_wind(const std::set<std::string> & names)
{
	std::set<std::string>::const_iterator it = names.begin();

	for (; it != names.end(); ++it)
	{

		Poco::DateTime begin(2008,1,1), end(2012,12,31);

		std::ofstream fout("ly-out/" + *it + "-wind.txt");
		fout<<"日期 极大风速 风向 出现时间\n";

		for(; begin <= end; 
			begin += Poco::Timespan(Poco::DateTime::daysOfMonth(begin.year(), begin.month()),0,0,0,0) )
		{
			std::string daymonth = Poco::DateTimeFormatter::format(begin, "%Y%m");
			Poco::File file("E:/cppcode/read-ec-850t/read-A-file/ly/"+ *it +'-'+ daymonth + ".TXT");
			
			std::cout<<file.path()<<std::endl;
			if( file.exists() ) 
			{
				std::ifstream fin(file.path() );
				
				std::string content, windstr;
				Poco::StreamCopier::copyToString(fin, content);

				Poco::RegularExpression rg("^FN[^=]+=[^=]+=([^=]+)=", Poco::RegularExpression::RE_MULTILINE);
				Poco::RegularExpression::MatchVec matchs;

				rg.match(content, 0, matchs);

				windstr = content.substr(matchs[1].offset, matchs[1].length);

				//std::cout<<windstr<<std::endl;

				std::istringstream ssin(windstr);

				std::string linestr, valstr;

				std::getline(ssin, linestr);

				Poco::DateTime aday(begin);
				while(std::getline(ssin, linestr) )//每一天
				{
					std::string daystr = Poco::DateTimeFormatter::format(aday, "%Y-%m-%d");

					std::istringstream ssin2(linestr);
					
					std::string max10, max10time, max3, max3time;
					
					ssin2>>max10>>max10time>>max3>>max3time;
					
					fout<<daystr<<' ';
					
					fout<<string_to<int>(max3.substr(0,3))/10.0<<' '<<string_to<int>(max3.substr(3,3))<<' '
						<<max3time.substr(0,2)<<':'<<max3time.substr(2,2);
					fout<<std::endl;

					aday += Poco::Timespan(1,0,0,0,0);
				}
			}
		}
		fout.close();

	}

}

//提取A文件中的温度
void deal_files_temph(const std::set<std::string> & names)
{
	std::set<std::string>::const_iterator it = names.begin();

	for (; it != names.end(); ++it)
	{

		Poco::DateTime begin(2008,1,1), end(2012,12,31);

		std::ofstream fout("ly-out/" + *it + "-t.txt");
		fout<<"日期 平均温度(四次平均) 最高气温 出现时间 最低气温 出现时间\n";

		for(; begin <= end; 
			begin += Poco::Timespan(Poco::DateTime::daysOfMonth(begin.year(), begin.month()),0,0,0,0) )
		{
			std::string daymonth = Poco::DateTimeFormatter::format(begin, "%Y%m");
			Poco::File file("E:/cppcode/read-ec-850t/read-A-file/ly/"+ *it +'-'+ daymonth + ".TXT");
			
			std::cout<<file.path()<<std::endl;
			if( file.exists() ) 
			{
				std::ifstream fin(file.path() );
				
				std::string content, temph_str;
				Poco::StreamCopier::copyToString(fin, content);

				Poco::RegularExpression rg("^TB[^=]+=", Poco::RegularExpression::RE_MULTILINE);

				rg.extract(content, temph_str);
				
				//std::cout<<temph_str<<std::endl; 
				//return;

				std::istringstream ssin(temph_str);

				std::string linestr, valstr, linestr2;

				std::getline(ssin, linestr);

				Poco::DateTime aday(begin);
				while(std::getline(ssin, linestr) && std::getline(ssin, linestr2) )//每一天
				{
					std::string daystr = Poco::DateTimeFormatter::format(aday, "%Y-%m-%d");

					std::istringstream ssin2(linestr+' '+linestr2);

					std::vector<int> t24;

					for(int i=0; i<24; i++)
					{
						ssin2>>valstr;
						t24.push_back(string_to<int>(valstr) );
					}

					std::string maxt, maxt_time, mint, mint_time;
					
					ssin2>>maxt>>maxt_time>>mint>>mint_time;
					
					fout<<daystr<<' '<<(t24[5]+t24[11]+t24[17]+t24[23])/4.0/10<<' ';
					
					fout<<string_to<int>(maxt)/10<<' '<<maxt_time.substr(0,2)<<':'<<maxt_time.substr(2,2)<<' '
						<<string_to<int>(mint)/10<<' '<<mint_time.substr(0,2)<<':'<<mint_time.substr(2,2);
					fout<<std::endl;

					aday += Poco::Timespan(1,0,0,0,0);
				}
			}
		}
		fout.close();

	}

}

int main()
{
	std::set<std::string>  sname;
	get_station_name(sname);
	deal_files_wind(sname);
	deal_files_temph(sname);

	return 0;
}