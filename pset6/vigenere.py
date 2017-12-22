import cs50
import sys

def main():
    if len(sys.argv) != 2:
        print("Usage: python ./vigenere.py k")
        exit(1)

    # check input and build the key array at the same time
    asciikey = []
    for c in sys.argv[1]:
        if c.isalpha() == False:
            print("Usage: python ./vigenere.py k")
            exit(1)
        else:
            asciikey.append(ord(c.lower()) - ord("a"))
    #print("asciikey: {}".format(asciikey))

    # get user input
    print("plaintext: ", end="")
    pt = cs50.get_string()

    #variable to keep track of the alpha character
    alphaplace = 0
    print("ciphertext: ", end="")
    for p in pt:
        if p.isalpha() == True:
            # if alpha character, encrypt

            if ord(p) >= ord("a") and ord(p) <= ord("z"):
                # if lowercase
                if ord(p) + asciikey[alphaplace % len(asciikey)] > ord("z"):
                    # wrap around if needd
                    print("{}".format(chr(ord("a") + 1 + ord(p) + asciikey[alphaplace % len(asciikey)] - ord("z"))), end = "")
                else:
                    print("{}".format(chr(ord(p) + asciikey[alphaplace % len(asciikey)])), end = "")
            elif ord(p) >= ord("A") and ord(p) <= ord("Z"):
                # if uppercase
                if ord(p) + asciikey[alphaplace % len(asciikey)] > ord("Z"):
                    #wrap around if needed
                    print("{}".format(chr(ord("A") + 1 + ord(p) + asciikey[alphaplace % len(asciikey)] - ord("Z"))), end = "")
                else:
                    print("{}".format(chr(ord(p) + asciikey[alphaplace % len(asciikey)])), end = "")

            alphaplace += 1
        else:
            # not alpha, don't encrypt
            print("{}".format(p), end = "")

    print()

    exit(0)


if __name__ == "__main__":
    main()