import requests
import json
from bs4 import BeautifulSoup

__DEFAULT_CITY__ = '北京'


class Weather:
    def __init__(self,city=''):
        self.scity = city if city else __DEFAULT_CITY__
        self.error = self.fetch_weather()
        
    def fetch_weather(self):
        try:
            x = 'http://toy1.weather.com.cn/search?cityname={}'.format(self.scity)
            x = json.loads(requests.get(x).text[1:-1])[0]['ref']
            x = x[:x.index('~')]
            t = 'http://www.weather.com.cn/weather/{}.shtml'.format(x)
            t = requests.get(t)
            t.encoding='utf-8'
            t = BeautifulSoup(t.text,features='lxml').find_all('ul',class_='t clearfix')[0]
            self.wea = [x.text for x in t.find_all('p',class_='wea')]
            self.tem = [(x.span.text[:-1] if x.span else None,x.i.text[:-1]) for x in t.find_all('p',class_='tem')]
            self.win = [(x.em.span.get('title'),x.i.text) for x in t.find_all('p',class_='win')]
            return 0
        except:
            return 1
    
    def afetch_weather(self):
        x = 'http://toy1.weather.com.cn/search?cityname={}'.format(self.city)
        x = json.loads(requests.get(x).text[1:-1])[0]['ref']
        x = x[:x.index('~')]
        t = 'http://www.weather.com.cn/weather/{}.shtml'.format(x)
        t = requests.get(t)
        t.encoding='utf-8'
        t = BeautifulSoup(t.text).find_all('ul',class_='t clearfix')[0]
        self.wea = [x.text for x in t.find_all('p',class_='wea')]
        self.tem = [(x.span.text[:-1] if x.span else None,x.i.text[:-1]) for x in t.find_all('p',class_='tem')]
        self.win = [(x.em.span.get('title'),x.i.text) for x in t.find_all('p',class_='win')]
        return 0
        
    def city(self):
        return self.scity

    def wind(self,offset=0):
        return self.win[offset]
    
    def weather(self,offset=0):
        return self.wea[offset]
    
    def temperature(self,offset=0):
        return self.tem[offset]
    
    def all(self,offset=0):
        return self.win[offset],self.wea[offset],self.tem[offset]
    
    def __repr__(self):
        if not self.error:
            return '<{} 天气:{} 温度:{} 风力:{}>'.format(self.city(),self.weather(),self.temperature(),self.wind())
        else:
            return '<Weather Error>'