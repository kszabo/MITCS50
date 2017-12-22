import cs50

def main():

    print("Number: ", end="")
    credit = cs50.get_string()

    c = 0
    digitSum = 0
    for i in reversed(range(len(credit))):
        c += 1
        if c % 2 == 0:
            digit = int(credit[i])
            if digit * 2 >= 10:
                digitSum += 1 + (digit * 2) - 10
            else:
                digitSum += digit * 2

            #print("second {} from back: {}, digitSum={}".format(c, digit, digitSum))

    c = 0
    for i in reversed(range(len(credit))):
        c += 1
        if c % 2 == 1:
            digit = int(credit[i])
            digitSum += digit

    if digitSum % 10 != 0:
        print("INVALID")
    else:
        print("{}".format(valid_format(credit)))

    exit(0)


def valid_format(credit):

    firstdigit = ""
    for i in range(len(credit)):
        if i == 0 or i == 1:
            firstdigit += credit[i]

    if len(credit) == 15 and (int(firstdigit) == 34 or int(firstdigit) == 37):
        cctype = "AMEX"
    elif len(credit) == 16 and (int(firstdigit) == 51 or int(firstdigit) == 52):
        cctype = "MASTERCARD"
    elif len(credit) == 16 and (int(firstdigit) == 53 or int(firstdigit) == 54):
        cctype = "MASTERCARD"
    elif len(credit) == 16 and int(firstdigit) == 55:
        cctype = "MASTERCARD"
    elif (len(credit) == 13 or len(credit) == 16) and int(firstdigit) // 10 == 4:
        cctype = "VISA"
    else:
        cctype = "INVALID"

    return cctype



if __name__ == "__main__":
    main()
