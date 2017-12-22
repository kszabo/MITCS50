from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/", methods=["GET", "POST"])
@login_required
def index():

    userid = session.get("user_id")

    # database lookup; also set placeholders
    purchases = db.execute("SELECT userid, symbol, symbol as name, numshares, 0 AS price, 0 AS total FROM view_purchases WHERE userid = :userid", userid=userid)

    if request.method == "GET":

        usercash = db.execute("SELECT cash FROM users WHERE id = :userid", userid=userid)
        cash = usercash[0]['cash']

        # look up current share price and calculate current value
        for purchase in purchases:
            symbolquote = lookup(purchase['symbol'])
            purchase['price'] = usd(symbolquote["price"])
            purchase['total'] = usd(symbolquote["price"] * purchase['numshares'])

        # append the CASH row to the end of the list
        purchases.append({'userid': userid, 'symbol': 'CASH', 'name': '', 'numshares': '', 'price': '', 'total': usd(cash)})

        return render_template("index.html", purchases=purchases, history="no")

    else:

        # compare to all purchases
        for purchase in purchases:

            # check for user's action request
            actionvalue = request.form.get('action_' + purchase['symbol'])

            if actionvalue == 'buy' or actionvalue == 'sell':
                # check if number entered
                numshares = int(request.form.get('numtrade_' + purchase['symbol']))

                # request user to enter number of shares to trade
                if not numshares or numshares == 0:
                    return apology('Please enter number of shares to trade')

                # look up price and calculate $
                symbolquote = lookup(purchase['symbol'])
                totalprice = int(numshares) * symbolquote['price']


            if actionvalue == 'buy':
                # check if user has enough $
                cash = db.execute("SELECT cash FROM users WHERE id = :userid", userid=userid)
                if cash[0]['cash'] < totalprice:
                    return apology("You do not have enough funds")

                # update necessary records
                db.execute("INSERT INTO purchases (userid, symbol, price, purchdate, numshares) VALUES (:userid, :symbol, :price, :purchdate, :numshares)",
                            userid=userid, symbol=purchase['symbol'], price=symbolquote['price'], purchdate=currentdate(), numshares=numshares)
                db.execute("UPDATE users SET cash = cash - :debit WHERE id = :userid", debit=totalprice, userid=userid)
            elif actionvalue == 'sell':
                db.execute("INSERT INTO purchases (userid, symbol, price, purchdate, numshares) VALUES (:userid, :symbol, :price, :purchdate, :numshares)",
                            userid=userid, symbol=purchase['symbol'], price=symbolquote['price'], purchdate=currentdate(), numshares=-(numshares))
                db.execute("UPDATE users SET cash = cash + :credit WHERE id = :userid", credit=totalprice, userid=userid)



            '''
            if symboltosell == "on":

                # find out how many
                numtosell = int(request.form.get("sell_" + purchase['symbol']))

                # if tries to sell more than she's got, error
                if numtosell > purchase['numshares']:
                    return apology("You do not have enough shares for this sale")
                else:
                    # check current price
                    price = float((lookup(purchase['symbol']))["price"])

                    # debit the sale to cash
                    db.execute("UPDATE users SET cash = cash + :sold WHERE id = :userid", sold=(price * numtosell), userid=userid)

                    # record the sale
                    db.execute("INSERT INTO purchases (userid, symbol, price, purchdate, numshares) VALUES (:userid, :symbol, :price, :purchdate, :numshares)",
                                userid=userid, symbol=purchase['symbol'], price=price, purchdate=datetime.datetime.now().strftime("%m-%d-%Y %H:%M:%S"), numshares=-(numtosell))

            '''

    return redirect(url_for("history"))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    if request.method == "POST":
        userid = session.get("user_id")
        symbol = request.form.get("symbol")

        # if user didn't input a symbol
        if symbol == "":
            return apology("Please enter a symbol")

        symbolquote = lookup(request.form.get("symbol"))

        # if symbol is incorrect
        if not symbolquote:
            return apology("Symbol does not exist")

        #if user didn't input number of shares
        numshares = request.form.get("numshares")
        if numshares == "":
            return apology("Please enter number of shares")
        elif int(numshares) < 0:
            return apology("Please enter a positive int for number of shares")

        # look up symbol price and available cash
        symbolquote = lookup(symbol)
        cash = db.execute("SELECT cash FROM users WHERE id = :userid", userid=userid)

        totalprice = symbolquote["price"] * int(numshares)
        if totalprice > cash[0]['cash']:
            return apology("Sorry, you do not have enough $")
        else:
            # record the purchase
            db.execute("INSERT INTO purchases (userid, symbol, price, purchdate, numshares) VALUES (:userid, :symbol, :price, :purchdate, :numshares)",
                        userid=userid, symbol=symbolquote["symbol"], price=symbolquote["price"], purchdate=currentdate(), numshares=numshares)
            # debit the account
            db.execute("UPDATE users SET cash = :cash WHERE id = :userid", cash=cash[0]['cash']-totalprice, userid=userid)

        #return render_template("quoted.html", symbolquote=symbolquote)
        # redirect user to home page
        return redirect(url_for("index", history="no"))

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    userid = session.get("user_id")

    purchases = db.execute("SELECT symbol, price, numshares, purchdate FROM purchases WHERE userid = :userid ORDER BY purchaseid DESC", userid=userid)
    return render_template("index.html", purchases=purchases, history="yes")


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index", history="no"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":

        # if user didn't input a symbol
        if request.form.get("symbol") == "":
            return apology("Please enter a symbol")

        symbolquote = lookup(request.form.get("symbol"))

        # if symbol is incorrect
        if not symbolquote:
            return apology("Symbol does not exist")

        return render_template("quoted.html", symbolquote=symbolquote)
    else:
        return render_template("quote.html")

@app.route("/quoted", methods=["GET"])
@login_required
def quoted():
    """Display quoute"""
    return render_template("quoted.html")

@app.route("/register", methods=["GET", "POST"])
def register():

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        """Register user."""
        if not request.form.get("username"):
            return apology("Please provide a username")
        else:
            # check if username already exists
            rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
            if len(rows) >= 1:
                return apology("The username already exists")

        if request.form["password1"] == "":
            return apology("Please provide a password")
        elif request.form["password1"] != request.form["password2"]:
            return apology("The two passwords do not match")
        else:
            db.execute("INSERT INTO users (username, hash) VALUES (:username, :password1)", username=request.form.get("username"), password1=pwd_context.hash(request.form.get("password1")))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")

    return render_template("login.html")

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    userid = session.get("user_id")

    if request.method == "POST":

        # database lookup; also set placeholders for html
        purchases = db.execute("SELECT userid, symbol, numshares FROM view_purchases WHERE userid = :userid", userid=userid)

        # compare to all purchases
        for purchase in purchases:

            # if user intends to sell this stock
            symboltosell = request.form.get(purchase['symbol'])

            if symboltosell == "on":

                # find out how many
                numtosell = int(request.form.get("sell_" + purchase['symbol']))

                # if tries to sell more than she's got, error
                if numtosell > purchase['numshares']:
                    return apology("You do not have enough shares for this sale")
                else:
                    # check current price
                    price = float((lookup(purchase['symbol']))["price"])

                    # debit the sale to cash
                    db.execute("UPDATE users SET cash = cash + :sold WHERE id = :userid", sold=(price * numtosell), userid=userid)

                    # record the sale
                    db.execute("INSERT INTO purchases (userid, symbol, price, purchdate, numshares) VALUES (:userid, :symbol, :price, :purchdate, :numshares)",
                                userid=userid, symbol=purchase['symbol'], price=price, purchdate=currentdate(), numshares=-(numtosell))


        # redirect user to home page
        return redirect(url_for("index", history="no"))
    else:

        # database lookup; also set placeholders
        purchases = db.execute("SELECT userid, symbol, numshares FROM view_purchases WHERE userid = :userid", userid=userid)

        return render_template("sell.html", purchases=purchases)