import cv2
import time

#Uses OpenCV classifiers to find smile and face info, tracking
#number of smiles in 10 second interval
class Detector():
    def __init__(self):
        self.smilesNum = 0

        #Classifier cascades that identify features (face and smile)
        self.face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades +'haarcascade_frontalface_default.xml')
        # self.eye_cascade = cv2.CascadeClassifier(cv2.data.haarcascades +'haarcascade_eye.xml')
        self.smile_cascade = cv2.CascadeClassifier(cv2.data.haarcascades +'haarcascade_smile.xml')
        self.smilesNeeded = 15

    #Detection function that takes in a frame and finds face/smile within it
    def detect(self, gray, frame):
        global smilesNum

        #Finds specific faces in frame
        faces = self.face_cascade.detectMultiScale(gray, 1.3, 5)
        for (x, y, w, h) in faces:
            #Draws blue rectangle around face
            cv2.rectangle(frame, (x, y), ((x + w), (y + h)), (255, 0, 0), 2)
            roi_gray = gray[y:y + h, x:x + w]
            roi_color = frame[y:y + h, x:x + w]

            #Identify smiles in frame
            smiles = self.smile_cascade.detectMultiScale(roi_gray, 1.8, 20)
    
            for (sx, sy, sw, sh) in smiles:
                #Draw red rectangle around smile and increment number of smiles detected
                cv2.rectangle(roi_color, (sx, sy), ((sx + sw), (sy + sh)), (0, 0, 255), 2)
                self.smilesNum += 1
                print(self.smilesNum)
        return frame

    def capture(self):

        timeout = time.time() + 5   # 5 seconds from now

        video_capture = cv2.VideoCapture(0)
        while video_capture.isOpened():
            #Only run until timeout or smile number met, otherwise keeps running
            if self.smilesNum == self.smilesNeeded or time.time() > timeout:
                break
                
            # Captures video_capture frame by frame
            _, frame = video_capture.read() 
        
            # To capture image in monochrome                    
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  
            
            # calls the detect() function    
            canvas = self.detect(gray, frame)   
        
            # Displays the result on camera feed                     
            cv2.imshow('Video', canvas) 
        
            # The control breaks once q key is pressed                        
            if cv2.waitKey(1) & 0xff == ord('q'):               
                break
        
        # Release the capture once all the processing is done.
        video_capture.release()                                 
        cv2.destroyAllWindows()



