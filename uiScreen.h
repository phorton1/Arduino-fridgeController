//-----------------------------------------
// uiScreen.h
//-----------------------------------------

#pragma once




class uiScreen
{
    public:

        uiScreen() {}
        void init();

        void displayLine(int line_num, const char *msg);
            // will truncate the message to 16 chars

};


extern uiScreen ui_screen;
    // in fridge.cpp

