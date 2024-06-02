#include <iostream>
#include <unordered_map> 
#include <fstream>
#include <sstream> 
#include <cstdlib>
#include <string>

#define WHISPER_DIR "~/Documents/SOE/whisper_repo/whisper.cpp/"
#define MODELS_DIR "~/Documents/SOE/whisper_repo/whisper.cpp/models/"

#define MAIN "~/Documents/SOE/whisper_repo/whisper.cpp/main"
#define MODEL_BIN_FILE "ggml-base.bin"
#define OUTPUT_WAV_FILE "output.wav"
#define LANG "pt"
#define OUTPUT_TXT_FILE_NAME "output"

#define RECORD_AUDIO "arecord -D plughw:3,0 -f S16_LE -r 16000 -c 1 output.wav -d 3"
//#define TRANSCRIBE_AUDIO_WHISPER "~/Documents/SOE/whisper_repo/whisper.cpp/main -m ~/Documents/SOE/whisper_repo/whisper.cpp/models/ggml-base.bin -f output.wav -l pt --output-txt true -of output"

using std::system, std::string;

const string MODEL = string(MODELS_DIR) + string(MODEL_BIN_FILE);
const string WHISPER_TRANSCRIBE_AUDIO_COMMAND = 
    string(MAIN) + 
    " -m " + MODEL + 
    " -f " + string(OUTPUT_WAV_FILE) + 
    " -l " + string(LANG) + 
    " --output-txt true -of " + string(OUTPUT_TXT_FILE_NAME);

int find_word_in_file(){
    std::string filename = "output.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return 1;
    }

    std::string line;
    int found = 0;

    while (std::getline(file, line)) {
        // Use std::istringstream to break the line into words
        found = line.find("pizza");
    }

    if (found) {
        std::cout << "The word was found in the file." << std::endl;
    } else {
        std::cout << "The word was not found in the file." << std::endl;
    }

    file.close();
    return found;
}

void textToSpeech(std::string speech)
{
   
    std::string command = "espeak -vpt+f3 -s 120 \"" + speech + "\"";
    system(command.c_str());
    return ;
}

int main() {

    std::unordered_map<std::string, int> menu = {
        {"hamburger", 1},
        {"x burger", 2},
        {"coca", 3},
        {"batata", 4},
        {"combo 1", 5},
        {"combo 2", 6}
    };

    while (true) {

        textToSpeech("Voce deseja continuear?");

        std::cout << "Deseja continuar? (s, n)" << std::endl;
        char c;
        std::cin >> c;
        if (c == 'N' || c == 'n') return 0;

        system(RECORD_AUDIO);

        std::cout << "\nFinished recording\n" << std::endl;

        system(WHISPER_TRANSCRIBE_AUDIO_COMMAND.c_str());
        std::cin.ignore(); // Clear the newline character from the buffer
    }

    find_word_in_file();

}
