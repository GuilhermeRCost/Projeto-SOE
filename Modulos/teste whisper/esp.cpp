#include <cstdlib>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string text = "ola mundo";
    //std::string command = "espeak -ven+whisper \"" + text + "\"";
    std::string command = "espeak -vpt+f3 \"" + text + "\"";
    system(command.c_str());
    return 0;
}