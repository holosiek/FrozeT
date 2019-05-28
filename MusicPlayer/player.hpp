#pragma once

namespace Player{
	// Main Functions
	void init();
	void draw();

	// Folder Functions
	int CALLBACK dialogHandler(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	std::string UTF16ToUTF8(const std::wstring &s);
	std::string browseFilesDialog(std::wstring const a_dialogTitle);
	std::vector<std::wstring> takeMusic(boost::filesystem::path p);
	void takeMusicFromFolder(const std::wstring a_dir);

	// Tracks-related Functions
	void shuffleTracks(std::vector<std::wstring> &a_tracks);

	// Player States
	void playTrack();
	void playNext();
	void playPrevious();
	void pauseSong();

	// Player Functions
	void refreshAlbum();
	void changeChannel(unsigned short a_whichOne);
	
	// Window-related Functions
	void windowSetFullscreen(sf::WindowHandle a_handle);
	void saveMode(bool a_state);
	void onWindowClick(bool a_check);
	void changePlayButtonIcon(int a_state);
}