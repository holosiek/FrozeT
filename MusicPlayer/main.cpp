// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
// Boost
#include <boost/filesystem.hpp>
// Standard
#include <algorithm>
#include <iostream>
#include <string>
#include <math.h>
#include <cmath>
#include <vector>
#include <random>
#include <fstream>
#include <shlobj.h>
#include <windows.h>
#include <sstream>
// TagLib
#include <tag.h>
#include <fileref.h>
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <attachedpictureframe.h>
// Bass
#include "bass.h"
// Project Related
#include "mymisc.h"
#include "button.h"
#include "config.h"
#include "spectrumComponent.h"
#include "player.h"
#include "logger.h"

/*
	TODO: Sort by modified time

	struct sortoo
	{
		bool operator ()(const std::wstring & a,const std::wstring & b)
		{
			return boost::filesystem::last_write_time(a) < boost::filesystem::last_write_time(b);// or some custom code
		}
	};

	=> TakeMusic (above return)

	std::sort(files.begin(),files.end(),sortoo());
	std::reverse(files.begin(),files.end());

	=> shuffleTracks

	< should be fixed to allow going track by track, not choosing random song >
*/
int main(int argc, char **argv){
	logSys.log("INFO - main.cpp init()", " # START #");
	for(int i=0; i<argc; i++){
		logSys.log("INFO - main.cpp main() - Program argument #" + intToString(i), argv[i]);
	}

	Player::init();
	Player::draw();

	/*
	if(argc == 1){
		takeMusicFromFolder({L""});
	} else {
		std::wstringstream ws;
		ws << argv[1];
		tracks = { ws.str() };
		playTrack();
	}
	*/

	return 0;
}