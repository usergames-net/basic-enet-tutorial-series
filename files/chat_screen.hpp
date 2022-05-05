#ifndef CHAT_SCREEN_HPP
#define CHAT_SCREEN_HPP

#include <stdio.h>

#include <iostream>
#include <string.h>

#include <ncurses.h>

class ChatScreen {
public:
	ChatScreen();
	~ChatScreen();

	void Init();

	void PostMessage(char username[80], char msg[80]);

	std::string CheckBoxInput();


private:
	int msg_y = 0;
	WINDOW * inputwin = nullptr;

};

#endif
