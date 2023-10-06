from request import Request
from faceDetection import Detector

#Prompts user for input, first Ryan then Talha
def start():
    while True:
        print('Please sit down Ryan so we can register your emotions. Press 1 when ready.\n')
        x = input()
        if(x == "1"):
            #Starts the facial read and sends to server
            print("Reading your face...")
            detectAndSend()
            break
        else:
            print("Please try again.")

    while True:
        print('Please sit down Talha so we can register your emotions. Press 1 when ready.\n')
        x = input()
        if(x == "1"):
            print("Reading your face...")
            detectAndSend()
            break
        else:
            print("Please try again.")

def detectAndSend():
    #Creates request and detector objects then captures video
    request = Request()
    detector = Detector()
    detector.capture()

    #Smiles detected must be greater than number needed
    if detector.smilesNum >= detector.smilesNeeded:
        request.sendRequest(True)
    else :
        request.sendRequest(False)



if __name__ == "__main__":
    start()