from constants import *

def read_file():

    with open(INPUT_FILE_NAME,'r') as f, open( '../result/turciphertextLetterCount.txt' ,'r') as letterCount, open('../info/TFA_REPLACE.txt', 'r') as replacedChars, open(REPLACER_OUTPUT_FILE,'w') as result:

        text = f.read() 

        # f -> input file
        # letterCount -> frequency result file

        ascending_letters = letterCount.read().replace('\n','').replace(' ','')
        replaced_chars = replacedChars.read().replace('\n','').replace(' ','')

        for i in range(0,29):
            # Turkish case
            if replaced_chars[i] == 'i':
                text = text.replace(ascending_letters[i], 'İ')
                continue
            
            text = text.replace(ascending_letters[i], replaced_chars[i].upper())

        result.write(text)
        


def main():
    read_file()

if __name__ == '__main__':
    main()