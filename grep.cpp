#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "grep.h"

int grep_system( string output, string cmd, bool *grepFinished ){
	
	//delete the output file
	string remove = "rm ";
	system( (remove + output + " 2>&-").c_str() );	/*hide error*/

	system( (cmd + ">>" + output).c_str() );
	//ruitusleep(2500*1000);
	
	*grepFinished = true;
	
	return 0;
}

int dummygrep(char* buffer)
{
	std::stringstream ss;

	for (int i = 0; i < 1; ++i)
	{
		ss << "Log Message " << i << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
	}

	const std::string tmp = ss.str();
	const char* cstr = tmp.c_str();

    strcpy(buffer, cstr);

	return strlen(cstr);
}
