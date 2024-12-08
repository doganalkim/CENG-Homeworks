import string
from constants import *

def read_file():
    
    dict = {}

    with open(INPUT_FILE_NAME, 'r') as f:

        text = f.read()

        total_len = len(text) / 100

        for l in string.ascii_lowercase:

            occurences = text.count(l)

            line = f'{l} : {occurences} : { occurences / total_len }\n'

            dict[l] = occurences

        # Below is Turkish specific chars handling
        for l in ['ş','ç','ö','ı','ğ','ü']:
            occurences = text.count(l)

            line = f'{l} : {occurences} : { occurences / total_len }\n'

            dict[l] = occurences


    sorted_items = sorted_items = sorted(dict.items(), key=lambda item: item[1], reverse = True)

    result = ''

    with open(OUTPUT_FILE_NAME, 'w') as f:

        for item in sorted_items:

            #line = f'{item[0]} : {item[1]}\n' # For debug
            line = f'{item[0]}\n' # Order the letters descending order with frequency

            result += line

        f.write(result)
        


def main():

    read_file()


if __name__ == '__main__':
    main()