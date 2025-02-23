from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import render_template
import os


app=Flask(__name__)
basedir = os.path.abspath(os.path.dirname(__file__))
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(basedir, 'database', 'db.sqlite3')
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)      

from model import *

@app.route("/admin",methods=['GET','POST'])
def adminhome():
    return render_template("adminhome.html",usertype="admin",username="admin")


@app.route("/user",methods=["POST",'GET'])
def userhome():
    return render_template("userhome.html", usertype='user',username='User')

@app.route("/user/scores",methods=['GET'])
def scorepage():
    return render_template("scorepage.html",usertype='user',username='User')

@app.route("/<usertype>/summary",methods=['GET','POST'])
def summarypage(usertype):
    if usertype=='user':
        #images for student
        pass
    else :
        pass
        #images for admin
    
    return render_template("summary.html",usertype=usertype,username="usertype")

@app.route("/admin/quiz",methods=["GET","POST"])
def quizpage():
    return render_template("quizpage.html",usertype='admin',username='admin')


with app.app_context():
    db.create_all()



if __name__=="__main__":
    app.debug=True
    app.run() 


