import pyttsx3

def cria_audio(audio):
    engine = pyttsx3.init()
    engine.setProperty('language', 'pt-BR')
    engine.setProperty('rate', 200)
    engine.say(audio)
    engine.runAndWait()

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
