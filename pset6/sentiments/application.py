from flask import Flask, redirect, render_template, request, url_for

import helpers
import os
import sys
from analyzer import Analyzer

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/search")
def search():

    # validate screen_name
    screen_name = request.args.get("screen_name", "")
    if not screen_name:
        return redirect(url_for("index"))

    # get screen_name's tweets
    how_many = 100
    tweets = helpers.get_user_timeline(screen_name, how_many)

    positive, negative, neutral = 0.0, 0.0, 100.0
    positives = os.path.join(sys.path[0], "positive-words.txt")
    negatives = os.path.join(sys.path[0], "negative-words.txt")
    # initialize Analyzer with the dictionaries
    myAnalyzer = Analyzer(positives, negatives)

    for t in tweets:
        # count sentiments for total
        sent = myAnalyzer.analyze(t)
        if sent > 0:
            positive = positive + 1
        if sent < 0:
            negative = negative + 1
        else:
            neutral = neutral + 1

    # generate chart
    chart = helpers.chart(positive, negative, neutral)

    # render results
    return render_template("search.html", chart=chart, screen_name=screen_name)
