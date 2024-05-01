from gtts import gTTS
from playsound import playsound

def cria_audio(audio):
    tts = gTTS(audio,lang='pt-br')
    #Salva o arquivo de audio
    tts.save('audios/hello.mp3')
    print("Estou aprendendo o que você disse...")
    #Da play ao audio
    playsound('audios/hello.mp3')

while True:
    frase = str(input("Escreva uma frase: "))
    cria_audio(frase)
    
    continuar = str(input("Deseja continuar? ")).upper()[0]

    if continuar == "S":
        continue
    elif continuar == "N":
        break
    else:
        print("Comando invalido")
