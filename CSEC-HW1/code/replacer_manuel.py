from constants import *

def read_file():

    with open(REPLACER_INPUT_FILE,'r') as f:

        with open(REPLACER_OUTPUT_FILE + str(COUNT) + '.txt','w') as res:

            text = f.read()

            text = text.replace(REPLACED_CHAR, NEW_CHAR)

            res.write(text)


def main():
    read_file()

if __name__ == '__main__':
    main()