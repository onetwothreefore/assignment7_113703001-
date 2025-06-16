#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <termios.h>
#include <vector>
#include "view.h"
#include "gameObject.h"

class Controller{

public:
       
    Controller(View&);
    void run();
private:

    void handleInput(int);
    void update();

    // Model
    std::vector<GameObject*> _objs;
	
	// assign6 加入控制的 Rock 索引
    int _controlledIndex = -1;

    // View
    View& _view; 
};

//static struct termios old_termios, new_termios;
void reset_terminal();
void configure_terminal();
int read_input();

#endif
