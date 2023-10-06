import requests

#Request class to send post requests to ESP32
class Request():
    def __init__(self):
        self.sadfaceurl = "http://192.168.4.1/sadface"
        self.happyfaceurl = "http://192.168.4.1/happyface"
        self.reqObj = {'happy': 'true'}

    #Send post request to endpoint for smile/sad face
    def sendRequest(self, smile: bool):
        if smile == True:
            url = self.happyfaceurl
        else:
            url = self.sadfaceurl
        
        resp = requests.post(url, json = self.reqObj)