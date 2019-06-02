// Standard
#include <string>
// Project Related
#include "logger.hpp"
#include "config.hpp"
#include "player.hpp"

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

int main(int a_argc, char** a_argv){
	// Clear logs
	Logger::clearLog();
	Logger::log("INFO - main.cpp init()", " # START #");

	// Log main args
	for(int i=0; i<a_argc; i++){
		Logger::log("INFO - main.cpp init() - Program argument #" + std::to_string(i), a_argv[i]);
	}

	// Initialize config
	Config::init(cfg);

	// Start main init
	Player::init();
	Player::draw();

	/*
	 TODO: 
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