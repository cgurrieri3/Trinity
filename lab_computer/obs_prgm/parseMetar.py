import numpy as np
import os
import datetime as dt
import requests
from metar import Metar

def getMetarData(icao):
    url = f"https://aviationweather.gov/api/data/metar"
    return requests.get(url,
                        params = {'ids': icao})

def getCloudInfo(raw):
    decoded = Metar.Metar(raw)
    return f"{decoded.sky_conditions()}"

def normalizeHeight(cond, height):
    nums = []
    for word in cond.split():
        try:
            nums.append(int(word))
        except:
            pass
    for val in nums:
        temp = str(val + height)
        cond = cond.replace(str(val), temp)
    return cond

def main():
    milfordFlag = True
    milfordCode = 'KMLF'
    milfordHeight = 5042
    beaverCode = 'KU52'
    beaverHeight = 5863
    cedarCode = 'KCDC'
    cedarHeight = 5622
    codes = [milfordCode, beaverCode, cedarCode]
    raws = []
    for code in codes:
        try:
            raws.append(getMetarData(code))
        except:
            texts.append('No connection available')
    texts = []
    for raw in raws:
        if raw.text == '':
            texts.append('No data available')
        else:
            texts.append(getCloudInfo(raw.text))
    # Outputting values
    

    return(f"Milford: {normalizeHeight(texts[0], milfordHeight)}\nBeaver: {normalizeHeight(texts[1], beaverHeight)}\nCedar: {normalizeHeight(texts[2], cedarHeight)}")

if __name__ == '__main__':
    main()
