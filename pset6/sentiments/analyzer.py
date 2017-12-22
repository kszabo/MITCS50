import nltk
import mmap

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer."""

        #instance variables
        self.posWords = []
        self.negWords = []

        #read in the positive words
        f = open(positives, "r")
        lines = f.read().splitlines()

        for word in lines:
            if word[:1] != ";" and word[:1] != "\n":
                # ignore the comments in the file
                self.posWords.append(word)

        f.close()

        # read in negative words
        f = open(negatives, "r")
        lines = f.read().splitlines()

        for word in lines:
            if word[:1] != ";" and word[:1] != "\n":
                # ingnore the comments in the file
                self.negWords.append(word)

        f.close()


    def analyze(self, text):
        """Analyze text for sentiment, returning its score."""

        sent = 0
        for word in text.split():
            # check each word in tweet
            if word.strip(":, ").lower() in self.posWords:
                sent += 1
            elif word.strip(":, ").lower() in self.negWords:
                sent -= 1

        return sent

