from constants import *
import requests
import string

TEXT = None

def fetch_text():
    global TEXT
    response = requests.get( URL )
    TEXT = response.text.replace('\n','').replace('\'','').lower()
    #print(TEXT)

def countLetters():
    global TEXT
    dict = {}

    for l in string.ascii_lowercase:

            occurences = TEXT.count(l)
            
            print(occurences)

            dict[l] = occurences

    # This below is for the Turkish part, I uncomment it while trying English analysis
    for l in ['ş','ç','ö','ı','ğ','ü']:
            occurences = TEXT.count(l)
            
            dict[l] = occurences

    sorted_items = sorted_items = sorted(dict.items(), key=lambda item: item[1], reverse = True)

    result = ''


    total_len = len(TEXT) / 100

    print(len(TEXT))


    with open('../info/TFA.txt', 'w') as f:

        for item in sorted_items:

            line = f'{item[0]} : {item[1]} : { item[1] / total_len }\n' # For debug
            #line = f'{item[0]}\n' # Order the letters descending order with frequency

            result += line

        f.write(result)

    

def main():
    fetch_text()
    countLetters()

if __name__ == '__main__':
    main()