import cs50

def main():
    i = get_height()
    build(i)
    exit(0)


def get_height():
    while True:
        print("How tall half-pyramid you need?: ", end="")
        n = cs50.get_int()
        if n >= 0 and n <= 23:
            break
    return n


def build(height):
    for h in range(0, height):
        for i in range (0, height - h - 1):
            print(" ", end="")

        for i in range (0, h + 1):
            print("#", end="")

        print("  ", end="")

        for i in range (0, h + 1):
            print("#", end="")

        print()

if __name__ == "__main__":
    main()

