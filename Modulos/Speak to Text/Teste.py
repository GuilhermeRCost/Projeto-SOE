import speech_recognition as sr

def ouvir_Microfone():
    microfone = sr.Recognizer()

    with sr.Microphone() as source:
        microfone.adjust_for_ambient_noise(source)
        print("fala alguma coisa ...")
        audio = microfone.listen(source)

    try:
        frase = microfone.recognize_google(audio, language='pt-BR')

        print("Voce disse " + frase)
    except sr.UnknownValueError:
        print("Não entendi")

while True:
    ouvir_Microfone()
    continuar = str(input("Deseja continuar? ")).upper()[0]

    if continuar == "S":
        continue
    elif continuar == "N":
        break
    else:
        print("Comando invalido")

    