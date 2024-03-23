from typing import Any
import requests
from time import sleep
import json

#IP = IP("0.0.0.0")


class uwaveControl:
    def __init__(self,IP,schema):
        self.IP = IP
        f = open(schema)
        code = json.load(f)
        for x in code:
            endpt = x.get("endpoint")
            parameter = x.get("parameters")
            self.load(endpt, parameter)
        f.close()

    callback = {}

    #printing saved IP
    def __str__(self):
        if(self.IP == "0.0.0.0"):
            return f"IP configuration failure, please manually set"
        else:
            print("Program connected to " + self.IP)
            res = self.test(url = "http://" + self.IP + "/")
            return res      
        
    #testing connection
    def test(self,url):
        
        response = requests.post(url)        
        return response.text
    
    #set switch to on / off
    def set(self, url, kwargs, parameters):

        params = {}
        for param in parameters:
            params[param] = kwargs[param]

        requests.post(url,params=params)
        pass

    #check the state of a particular switch
    def check(self, url, kwargs, parameters):

        params = {}
        for param in parameters:
            params[param] = kwargs[param]
        req = requests.get(url,params=params)
        return req.text

    #check all switch
    def list(self, url):

        response = requests.get(url)
        sleep(0.5)
        if response:
            message = response.text
            message = message.replace("[","")
            message = message.replace("]","")
            message = message.replace("\"","")
            message = message.replace(",","\n")
        return message

    #reset all switch to off
    def reset(self, url, kwargs, parameters):
        params = {}
        for param in parameters:
            params[param] = kwargs[param]
        requests.post(url,params)
        pass

    def resetArd(self, url, kwargs, parameters):
        params = {}
        for param in parameters:
            params[param] = kwargs[param]
        requests.post(url,params)
        pass
    

    def load(self, op, parameters):
        index = op.replace("/","")
        url = "http://" + self.IP + op

        if index == "open" or index == "close":
            fn = lambda **kwargs: self.set(url, kwargs, parameters)
        elif index == "check":
            fn = lambda **kwargs: self.check(url, kwargs, parameters)
        elif index == "":
            fn = lambda: self.test(url)
        elif index == "list":
            fn = lambda: self.list(url)
        elif index == "reset":
            fn = lambda **kwargs: self.reset(url, kwargs, parameters)
        elif index == "resetModule":
            fn = lambda **kwargs: self.resetArd(url, kwargs, parameters)

        self.callback[index] = fn
        pass

    

    #remember to change name switch in esp32 code
    def __call__(self, operation: str, **kwargs):
        if kwargs is not None:
            return self.callback[operation](**kwargs)
        else:
            return self.callback[operation]






