from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import render_template,redirect,flash,url_for,session
import os
from model import *
from api import *
from sqlalchemy import desc
from flask_cors import CORS



app=Flask(__name__)
basedir = os.path.abspath(os.path.dirname(__file__))
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(basedir, 'database', 'db.sqlite3')
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db.init_app(app)  
app.secret_key='QuizMaster'
api=Api(app)
CORS(app)





@app.route('/admin',methods=['GET','POST','PUT'])
def adminhome():
    subjects = Subject.query.all()

    subject_data = []
    for subject in subjects:
        chapters = Chapter.query.filter_by(subject_id=subject.id).all()
        chapter_data = []

        for chapter in chapters:
            question_count = Question.query.filter_by(chapter_id=chapter.id).count()
            chapter_data.append({
                'id': chapter.id,
                'title': chapter.name,
                'question_count': question_count
            })

        subject_data.append({
            'id': subject.id,
            'name': subject.name,
            'chapters': chapter_data
        })
    return render_template("adminhome.html",usertype="admin",username="admin",subdet=subject_data)


@app.route("/user",methods=["POST",'GET'])
def userhome():
    quizzes=Quiz.query.join(Chapter).join(Subject).order_by(desc(Quiz.date_of_quiz)).all()
    today=datetime.today().strftime('%d-%m-%Y')
    quiz_details = []
    for quiz in quizzes:
        qstns=Question.query.filter_by(quiz_id=quiz.id).count()

        quiz_details.append({
            'quiz_id': quiz.id,
            'subject_name': quiz.chapter.subject.name,
            'chapter_name': quiz.chapter.name,
            'date_of_quiz': quiz.date_of_quiz.strftime('%d-%m-%Y'),
            'num_questions': qstns,  
            'time_duration': quiz.time_duration
        })
        

    return render_template("userhome.html", usertype='user',username=session.get('username'),quiz=quiz_details,today=today)

@app.route("/user/scores",methods=['GET'])
def scorepage():
    USER = session.get('id') 
    scores=Score.query.filter_by(user_id=USER).join(Quiz).join(Chapter).join(Subject)

    score_data=[]
    for i in scores:

        qstns=Question.query.filter_by(quiz_id=i.quiz_id).count()

        score_data.append({
            'id':i.quiz_id,
            'subject':i.quiz.chapter.subject.name,
            'chapter':i.quiz.chapter.name,
            'quizname':i.quiz.name,
            'date':i.quiz.date_of_quiz.strftime('%d-%m-%Y'),
            'num': qstns ,
            'score':i.total_scored
        })

        

    return render_template("scorepage.html",usertype='user',username=session.get('username'),data=score_data)

@app.route("/<usertype>/summary",methods=['GET','POST'])
def summarypage(usertype):
    if usertype=='user':
        USER=session.get("id")

        subcount = (
            Quiz.query
            .join(Chapter, Quiz.chapter_id == Chapter.id)
            .join(Subject, Chapter.subject_id == Subject.id)
            .with_entities(Subject.name, db.func.count(Quiz.id).label('quiz_count'))
            .group_by(Subject.name)
            .all()
        )

        img1_labels = [row.name for row in subcount]
        img1_counts = [row.quiz_count for row in subcount]

        quiz_count = (
            Score.query
            .join(Quiz, Score.quiz_id == Quiz.id)
            .filter(Score.user_id == USER)
            .with_entities(db.func.strftime('%m', Quiz.date_of_quiz).label('month'), db.func.count().label('quiz_count'))
            .group_by(db.func.strftime('%m', Quiz.date_of_quiz))
            .all()
        )
        img2_labels=[datetime.strptime(str(month), "%m").strftime("%B") for month, count in quiz_count]
        img2_counts = [count for month, count in quiz_count]

        
    else :
        top_scores_by_subject = (
            Score.query
            .join(Quiz, Score.quiz_id == Quiz.id)
            .join(Chapter, Quiz.chapter_id == Chapter.id)
            .join(Subject, Chapter.subject_id == Subject.id)
            .with_entities(
                Subject.name.label('subject_name'),
                db.func.max(Score.total_scored).label('top_score')  # Get the highest score per subject
            )
            .group_by(Subject.name)  # Group by subject
            .all()
        )

        # Prepare data for chart
        img1_labels = []
        img1_counts = []

        for row in top_scores_by_subject:
            img1_labels.append(row.subject_name)
            img1_counts.append(row.top_score)
            

        attempts=(
            Score.query
            .join(Quiz)
            .join(Chapter)
            .join(Subject)
            .with_entities(
                Subject.name.label("subject_name"),
                db.func.count(Score.id).label("total_attempts")
            )
            .group_by(Subject.name)
            .all()
        )
        img2_labels=[i for i,_ in attempts ]
        img2_counts=[i for _,i in attempts]
    
    return render_template("summary.html",usertype=usertype,username=session.get('username'),img2_counts=img2_counts,img2_labels=img2_labels,img1_counts=img1_counts,img1_labels=img1_labels)

@app.route("/admin/quiz",methods=["GET","POST"])
def quizpage():
    data = (
        Quiz.query
        .outerjoin(Question)
        .with_entities(
            Quiz.id.label("qid"),
            Quiz.name.label("name"),
            Question.id.label("id"),
            Question.question_statement.label("question")
        )
        .order_by(Quiz.id)
        .all()
    )

    # Restructure data to group questions under each quiz
    quizzes = {}
    for row in data:
        qid = row.qid
        if qid not in quizzes:
            quizzes[qid] = {
                "name": row.name,
                "qid": row.qid,
                "questions": []
            }
        
        if row.id:  # Add questions only if they exist
            quizzes[qid]["questions"].append({
                "id": row.id,
                "question": row.question
            })
    
    chapters=Chapter.query.with_entities(
        Chapter.name.label('name'),
        Chapter.id.label("id"),
        Chapter.subject_id.label('subid')
    ).all()

    return render_template("quizpage.html",usertype='admin',username='admin',data=quizzes.values(),chapters=chapters)


@app.route("/",methods=['GET','POST'])
def login():
    try:
        if request.method=='GET':
            return render_template("login.html")
        elif request.method=='POST':
            data=User.query.filter_by(username=request.form["username"]).first()
            print(data.username,data.password)
            if data.password==request.form['password']:
                if data.role=='admin':
                    session["username"]=request.form['username']
                    session['usertype']="admin"
                    return redirect(url_for("adminhome"))
                else:
                    session["username"]=request.form['username']
                    session['usertype']="user"
                    session['id']=data.id
                    return redirect(url_for("userhome"))
            else:
                flash("Invalid Username or Password","danger")
                return render_template("login.html")
                
    except Exception as e:
        flash("Invalid Username or Password","danger")
        return render_template("login.html")


@app.route('/signup',methods=['GET','POST'])
def signup():
    try:
        if request.method=="POST":
            dob=request.form['dob']
            formatted_dob=datetime.strptime(dob,'%Y-%m-%d').date()
            newuser=User(
                username=request.form["username"],
                password=request.form['password'],
                full_name=request.form['fullname'],
                qualification=request.form['Qualification'],
                dob=formatted_dob,
                role='user'
                )
            
            db.session.add(newuser)
            db.session.commit()
            return redirect(url_for('login'))
    except Exception as e:
        print("error",str(e))
        db.session.rollback()
        flash("ERROR: Please try again.", "error")
    print('error1')
    return render_template("signup.html")

@app.route("/logout",methods=["GET"])
def logout():
    session.clear()
    return redirect(url_for("login"))

@app.route("/user/instructions/<int:id>")
def instructions(id):
    return render_template("instructions.html",id=id)       

@app.route("/user/attend/<int:id>")
def atttendquz(id):
    data=Question.query.filter_by(quiz_id=id).all()
    quizdet=Quiz.query.filter_by(id=id).first()

    return render_template("attendquiz.html",data=data,quizdet=quizdet)

@app.route("/submitquiz/<int:id>",methods=["POST"])
def submit(id):
    try:
        qstns=Question.query.filter_by(quiz_id=id).all()
        score=0
        totalqs=len(qstns)

        for i in qstns:
            ans=request.form.get(f'q{i.id}')

            if ans and int(ans)==i.correct_option:
                score+=1
        prcntscore=(score/totalqs)*100
        newscore=Score(user_id=session.get("id"),quiz_id=id,total_scored=prcntscore)
        db.session.add(newscore)
        db.session.commit()

        flash("The Quiz is attended succesfully and the score has been updated in scores page","success")
        return redirect(url_for("userhome"))
    except Exception as e:
            db.session.rollback()
            print(str(e))
            flash("An error occurred while submitting the quiz. Please try again.", "danger")
            return redirect(url_for('userhome'))
    
@app.route("/<usertype>/search",methods=['GET'])
def search(usertype):
    Query=request.args.get('query')
    results=[]

    if Query:
        subjects=Subject.query.filter(Subject.name.ilike(f'%{Query}%')).all()
        results.extend([{'type': 'Subject', 'name': i.name} for i in subjects])

        quizzes=Quiz.query.filter(Quiz.name.ilike(f'%{Query}%')).all()
        results.extend([{'type': 'Quiz', 'name': i.name} for i in quizzes])

        chapters=Chapter.query.filter(Chapter.name.ilike(f'%{Query}%')).all()
        results.extend([{'type': 'Chapter', 'name': i.name} for i in chapters])

        users=User.query.filter(User.username.ilike(f'%{Query}%')).all()
        results.extend([{'type': 'User', 'name': i.username} for i in users])
    print(results)
    return render_template('searchpage.html',usertype=usertype,username=session.get('username'),result=results,query=Query)


with app.app_context():
    db.create_all()
    db.session.commit()

api.add_resource(chapterResources, '/api/chapter','/api/chapter/<int:id>')
api.add_resource(quizResources, '/api/quiz/<int:id>','/api/quiz')
api.add_resource(questionResources, '/api/question','/api/question/<int:id>')
api.add_resource(subjectResources, '/api/subject','/api/subject/<int:id>')

if __name__=="__main__":
    app.debug=True
    app.run() 


