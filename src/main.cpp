#include <iostream>
#include <string>
#include "view.h"
#include "controller.h"
#include "AnsiPrint.h"
/**
 * Print my id
 * */
void
PrintMyID(std::string studId) {

    std::string str = "ID: " + studId;
    std::cout << AnsiPrint(str.c_str(),YELLOW,RED, true, true) << std::endl << std::endl;
}



int main(){
    // TODO 
    // Integrate the Simple MVC Pattern to run the program.
    View view;

    Controller controller(view);

    controller.run();


    // TODO
    // change to your student ID.
    PrintMyID("113703001");
}
