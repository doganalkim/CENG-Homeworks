with open('../files/randomtext.txt', 'r+') as f:
    t = f.read()
    
    print(t)

    t = t.replace(' ', '')
    t = t.replace(',', '')
    t = t.replace('.', '')

    t = t.lower()

    with open('../files/randomtext.txt', 'r+') as new_f:
        new_f.write(t)