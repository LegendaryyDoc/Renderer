#pragma once

class context
{
	struct GLFWwindow * window;

public: 
	bool init(int width, int height, const char * title); // window size and title
	void tick(); // update
	void term(); // clean up
	void clear(); // clear the screen

	bool shouldClose() const;
};