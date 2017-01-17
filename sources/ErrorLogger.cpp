#include "errorlogger.h"
#include <math.h>

const char ErrorLogger::Filename[]="error.log";
ErrorLogger ErrorLogger::instance;
int ErrorLogger::LineCount=0;


ErrorLogger::ErrorLogger()
{
#ifdef LOGGING
	file.open(Filename);
#endif
}

ErrorLogger::~ErrorLogger()
{
#ifdef LOGGING
	file.close();
#endif
}

void ErrorLogger::Writeln(const char text[])
{
	Write(text);
	Write("\n");
}

void ErrorLogger::Write(const char text[])
{
#ifdef LOGGING
	if(LineCount<MAXLINES)
	{
		OutputDebugString(text);
		instance.file << text;
		if(++LineCount == MAXLINES)
		{
			OutputDebugString("\nErrorLogger limit reached. Who taught you to progam?");
			instance.file << "\nErrorLogger limit reached. Who taught you to progam?";
			instance.file.flush();
		}
	}
#endif
#ifdef SLOWLOG
	instance.file.flush();
#endif
}

void ErrorLogger::Writeln(double num)
{
	Write(num);
	Write("\n");
}

void ErrorLogger::Write(double num)
{
#ifdef LOGGING
	std::string output="";
	if(num<0)
	{
		output="-";
		num=-num;
	}
	int power = 0;
	int intNum = (int)num;
	while(intNum>0)
	{
		++power;
		intNum/=10;
	}
	if(power==0)
		output+="0";
	intNum=(int)num;
	num-=intNum;
	for(int i=power-1;i>=0;i--)
	{
		int order = (int)pow(10,i);
		output+='0'+intNum/order;
		intNum=intNum%order;
	}
	if(num>0)
	{
		output+=".";
	
		intNum=(int)(num*1000);
		for(int i=2;i>=0;i--)
		{
			int order = (int)pow(10,i);
			output+='0'+intNum/order;
			intNum=intNum%order;
		}
	}
	Write(output.c_str());
#endif
}

