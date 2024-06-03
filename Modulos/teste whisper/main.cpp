#include <iostream>
#include <unordered_map> 
#include <fstream>
#include <sstream> 
#include <cstdlib>
#include <string>
#include <algorithm> 
#include <cctype> 
#include <thread>
#include <semaphore.h>

#define WHISPER_DIR "~/Documents/SOE/whisper_repo/whisper.cpp/"
#define MODELS_DIR "~/Documents/SOE/whisper_repo/whisper.cpp/models/"

#define MAIN "~/Documents/SOE/whisper_repo/whisper.cpp/main"
#define MODEL_BIN_FILE "ggml-base.bin"
#define OUTPUT_WAV_FILE "output.wav"
#define LANG "pt"
#define OUTPUT_TXT_FILE_NAME "output"
#define RECORD_TIME 5 //seconds
#define RECORD_AUDIO_INPUT "plughw:3,0"

//#define RECORD_AUDIO "arecord -D plughw:3,0 -f S16_LE -r 16000 -c 1 output.wav -d 3"
//#define TRANSCRIBE_AUDIO_WHISPER "~/Documents/SOE/whisper_repo/whisper.cpp/main -m ~/Documents/SOE/whisper_repo/whisper.cpp/models/ggml-base.bin -f output.wav -l pt --output-txt true -of output"

using std::system, std::string;

sem_t semaphore;

const string MODEL = string(MODELS_DIR) + string(MODEL_BIN_FILE);
const string WHISPER_TRANSCRIBE_AUDIO_COMMAND = 
    string(MAIN) + 
    " -m " + MODEL + 
    " -f " + string(OUTPUT_WAV_FILE) + 
    " -l " + string(LANG) + 
    " --output-txt true -of " + string(OUTPUT_TXT_FILE_NAME);

// const string RECORD_AUDIO_COMMAND =  
//     "arecord -D " + string(RECORD_AUDIO_INPUT) +
//     " -f S16_LE -r 16000 -c 1 " +
//     string(OUTPUT_WAV_FILE) +
//     " -d " + std::to_string(RECORD_TIME);

bool find_word_in_file(string word){
    std::string filename = "output.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return 1;
    } 

    std::string line;
    int found_at = 0;

    while (std::getline(file, line)) {
        // Convert the string to lowercase
        std::transform(line.begin(), line.end(), line.begin(),
                   [](unsigned char c) { return std::tolower(c); });

        std::cout << line << std::endl;

        // Use std::istringstream to break the line into words
        found_at = line.find(word);
    }

    bool found = false;
    if (found_at>0) {
        std::cout << "The word was found in the file." << std::endl;
        found = true;
    } else {
        std::cout << "The word was not found in the file." << std::endl;
    }

    file.close();
    return found;
}

void textToSpeech(std::string speech)
{
   
    std::string command = "espeak -v pt-br+male4 -s 155 \"" + speech + "\"";
    system(command.c_str());
    return ;
}

void record_and_transcribe(int record_time){
const string RECORD_AUDIO_COMMAND =  
    "arecord -D " + string(RECORD_AUDIO_INPUT) +
    " -f S16_LE -r 16000 -c 1 " +
    string(OUTPUT_WAV_FILE) +
    " -d " + std::to_string(record_time);

    system(RECORD_AUDIO_COMMAND.c_str());

    std::cout << "\nFinished recording\n" << std::endl;

    sem_post(&semaphore);

    system(WHISPER_TRANSCRIBE_AUDIO_COMMAND.c_str());
    std::cout << '\n';

}

// Function that the thread will execute
void pls_wait_message() {
    while(true){
        sem_wait(&semaphore);
        textToSpeech("Por favor, aguarde alguns instantes.");
    }
}


int main() {

    sem_init(&semaphore, 0, 0); // Initialize the semaphore with value 1
    std::thread messageThread(pls_wait_message); // Start the thread

    std::unordered_map<std::string, int> menu = {
        {"hamburger", 1},
        {"x burger", 2},
        {"coca", 3},
        {"batata", 4},
        {"combo 1", 5},
        {"combo 2", 6}
    };

    textToSpeech("Olá, você dejesa fazer um pedido?");
    record_and_transcribe(3);
    bool word_found = find_word_in_file("não");
    if (word_found) return 0;

    char c;
    while (true) {

        std::cout << "Deseja continuar? (s, n)" << std::endl;
        std::cin >> c;
        if (c == 'N' || c == 'n') return 0;
        std::cin.ignore(); // Clear the newline character from the buffer
    }

    messageThread.join(); // Wait for the thread to finish (not reached in this example)
    sem_destroy(&semaphore); // Clean up the semaphore

    find_word_in_file("pizza");

    return 0;

}
