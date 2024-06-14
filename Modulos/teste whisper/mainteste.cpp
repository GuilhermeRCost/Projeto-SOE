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
#include <wiringPi.h>
#include <bitset>
#include <atomic>


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

typedef union Word_file_info {
    struct {
        uint8_t error : 1;
        uint8_t no_response : 1;
        uint8_t word_found : 1;
        uint8_t reserved : 5;  // Reserved bits to make up the rest of the byte
    } bits;
    uint8_t byte;  // Access the whole byte

        // Default constructor initializes to 0
    Word_file_info() : byte(0) {}

    // Parameterized constructor to initialize bits with a given uint8_t value
    Word_file_info(uint8_t value) {
        byte = value & 0x07; // Use bitmask to ensure bits 3-7 are 0
    }

} wfi_t;

sem_t semaphore;

const string MODEL = string(MODELS_DIR) + string(MODEL_BIN_FILE);
const string WHISPER_TRANSCRIBE_AUDIO_COMMAND = 
    string(MAIN) + 
    " -m " + MODEL + 
    " -f " + string(OUTPUT_WAV_FILE) + 
    " -l " + string(LANG) + 
    " --output-txt true -of " + string(OUTPUT_TXT_FILE_NAME) +
    " --no-prints";

// const string RECORD_AUDIO_COMMAND =  
//     "arecord -D " + string(RECORD_AUDIO_INPUT) +
//     " -f S16_LE -r 16000 -c 1 " +
//     string(OUTPUT_WAV_FILE) +
//     " -d " + std::to_string(RECORD_TIME);


uint8_t find_word_in_file(string word){
    string filename = string(OUTPUT_TXT_FILE_NAME) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return 1;
    } 

    Word_file_info res(0);

    std::string line;
    int found_at = 0;
    bool word_found = false;
    bool no_response = true;

    while (std::getline(file, line)) {
        std::cout << line << std::endl;
        int brakets = line.find(']');
        std::cout << "brakets = " << brakets << std::endl;
        if(brakets > 0) {
            std::cout << "O usuário não disse nada nesta linha" << std::endl;
            res.bits.no_response = true;
            continue;
        }
        // Convert the string to lowercase
        std::transform(line.begin(), line.end(), line.begin(),
                   [](unsigned char c) { return std::tolower(c); });

        // Use std::istringstream to break the line into words
        found_at = line.find(word);

        if (found_at>0) {
            std::cout << "The word was found in the file." << std::endl;
            word_found = true;
            no_response = false;
            break;
        } else {
            std::cout << "The word was not found in this line." << std::endl;
            no_response = false;
        }
    }
    
    res.bits.error = 0;
    res.bits.no_response = no_response;
    res.bits.word_found = word_found;

    file.close();
    return res.byte;
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
void pls_wait_message(std::atomic<bool>& runFlag) {
    while(runFlag){
        sem_wait(&semaphore);
        textToSpeech("Por favor, aguarde alguns instantes.");

    }
}


int main() {

    // Initialize WiringPi
    if (wiringPiSetupGpio() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    // Set GPIO 17 as input
    pinMode(17, INPUT);

    std::atomic<bool> runFlag(true);
    sem_init(&semaphore, 0, 0); // Initialize the semaphore with value 1
    std::thread messageThread(pls_wait_message, std::ref(runFlag)); // Start the thread

    std::unordered_map<std::string, int> menu = {
        {"hambúrguer", 1},
        {"cheeseburger", 2},
        {"coca", 3},
        {"batata", 4},
        {"combo 1", 5},
        {"combo 2", 6}
    };

    wfi_t word_info;

    std::cout << "Aguardando entrada" << std::endl;

    while (1) {
        // Read and print the value of GPIO 17
        int value = digitalRead(17);

        if(value == 1){
            textToSpeech("Olá, você dejesa fazer um pedido?");
            record_and_transcribe(5);
            word_info.byte = find_word_in_file("não");

            std::bitset<8> x(word_info.byte);
            std::cout << x << "\n\n";

            if (word_info.bits.word_found || word_info.bits.no_response) goto terminate;
            if (word_info.byte == 1) goto terminate_with_errors;

            char c;
            while (true) {

                textToSpeech("Qual é o seu pedido?");
                record_and_transcribe(6);
                for (const auto& pair : menu) {
                    std::cout << "Key: " << pair.first << std::endl;
                    word_info.byte = find_word_in_file(pair.first);
                    if (word_info.bits.error) goto terminate_with_errors;
                    if (word_info.bits.no_response) {
                        std::cout << "No response from user" << std::endl;
                        break;
                    }
                    if (word_info.bits.word_found){
                        string text = "Você pediu " + pair.first;
                        textToSpeech(text);
                        goto terminate;
                    }
                }
                
                textToSpeech("Não temos essa opção");

                break;
            }
        }
    }

    terminate:
    // Signal the thread to stop
    runFlag = false;
    sem_post(&semaphore);

    // Wait for the thread to finish
    messageThread.join();
    sem_destroy(&semaphore);

    textToSpeech("Obrigado por escolher nosso restaurante. Daqui a pouco sua comida estará pronta!");

    std::cout << "Terminated with no errors" << std::endl;
    return 0;

    terminate_with_errors:
    // Signal the thread to stop
    runFlag = false;
    sem_post(&semaphore);

    // Wait for the thread to finish
    messageThread.join();
    sem_destroy(&semaphore);

    std::cout << "Terminated with errors" << std::endl;

    return -1;

}

//-lwiringPi -pthread