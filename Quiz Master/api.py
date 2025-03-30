from flask import Flask, request, jsonify, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_restful import Api, Resource
from model import *
from datetime import datetime

def converttime(dur):
    dur=int(dur.strip(" mins"))
    
    hours=dur//60
    mins=dur%60

    return f"{hours:02}:{mins:02}"


class chapterResources(Resource):
    def post(self):
        try:
            data=request.get_json()

            subid=data["subid"]
            chaptername=data["chapter_name"]
            chdesc=data['chdesc']

            newchap=Chapter(name=chaptername,subject_id=subid,description=chdesc)
            
            db.session.add(newchap)
            db.session.commit()
            return jsonify({"message":"Chapter added succesfully","status":"success"})

        except Exception as e:
            db.session.rollback()
            
            return jsonify({"message":"Chapter not added succesfully","status":"error"})
        finally:
            db.session.close()
    def put(self):
        try:
            data=request.get_json()
            chapterid=data.get('chid')
            chaptername=data["chname"]
            chdesc=data["chdes"]
            
            chapter=Chapter.query.filter_by(id=chapterid).first()
            if not chapter:
                return jsonify({'message':'Chapter not found','status':"error"})
            
            chapter.name=chaptername
            chapter.description=chdesc

            db.session.commit()
            return jsonify({"message":"Chapter aedited succesfully","status":"success"})
        except Exception as e:
            
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"})

    def get(self,id):
        chapter=Chapter.query.filter_by(id=id).first()
        if chapter:
            chapter_json={
                'message':'fetched the data succesfully',
                'status':'success',
                "id":chapter.id,
                'name':chapter.name,
                'description':chapter.description
            }
            return jsonify(chapter_json)
        return jsonify({
            'message':"chapter not found",
            'status':"error"
        })
    def delete(self,id):
        try:
            chapter=Chapter.query.get(id)
            db.session.delete(chapter)
            db.session.commit()
            return jsonify({"message":"Chapter deleted","status":"success"})
        except Exception as e:
            
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"}) 
        
class quizResources(Resource):

    def put(self):
        try:
            data=request.get_json()

            qid=data['qid']
            chid=data['chid']
            name=data["name"]
            time=datetime.strptime(data['duration'],'%H:%M')
            date=datetime.strptime(data["date"],"%Y-%m-%d")
            desc=data["desc"]

            duration=time.hour*60+time.minute

            quiz=Quiz.query.get(qid)
            
            if quiz:
                quiz.chapter_id=chid
                quiz.name=name
                quiz.date_of_quiz=date
                quiz.date_duration=duration
                quiz.description=desc

                db.session.commit()
                return jsonify({'message':"Quiz edited succesfully","status":"success"})
            return jsonify({'message':'Some error occured','status':"error"}) 
        except Exception as e:
            print (str(e))
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"}) 

    def get(self,id):
        try:
            quiz=Quiz.query.get(id)
            chapter=Chapter.query.filter_by(id=quiz.chapter_id).first()

            chaptername=chapter.name
            if quiz:
                time=quiz.time_duration
                formattedtime=converttime(time)
                date=quiz.date_of_quiz
                date=date.strftime('%Y-%m-%d')


                return jsonify({
                    'message':'Fetched the quiz details succesfully',
                    'quizid':quiz.id,
                    'quizname':quiz.name,
                    'chapterid':quiz.chapter_id,
                    'chaptername':chaptername,
                    'desc':quiz.description,
                    'quiztime':formattedtime,
                    'quizdate':date,
                    'status':"Success"
                })
        except Exception as e:
            mesg=str(e)
            print(e)
            return jsonify({
                    'message':'Some Error Occured:{mesg}',
                    'status':"error"
                })
    def post(self):
        try:
            data=request.get_json()

            chid=data['chid']
            name=data["name"]
            time=datetime.strptime(data['duration'],'%H:%M')
            date=datetime.strptime(data["date"],"%Y-%m-%d")
            desc=data["desc"]

            duration=time.hour*60+time.minute

            newquiz=Quiz(chapter_id=chid,name=name,date_of_quiz=date,time_duration=duration,description=desc)

            db.session.add(newquiz)
            db.session.commit()
            return jsonify({"message":"Quiz added succesfully","status":"success"})
        except Exception as e:
            mesg=str(e)
            print (mesg)
            return jsonify({
                    'message':str('Some Error Occured: ',mesg),
                    'status':"error"
            })
    def delete(self,id):
        try:
            qstn=Quiz.query.get(id)
            db.session.delete(qstn)
            db.session.commit()
            return jsonify({"message":"Question deleted","status":"success"})
        except Exception as e:
            
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"}) 


class questionResources(Resource):
    def post(self):
        try:
            data=request.get_json()

            qstitle=data["qstitle"]
            qstn=data['qstn']
            opt1=data['opt1']
            opt2=data['opt2']
            opt3=data['opt3']
            opt4=data['opt4']
            correctopt=data['correctopt']
            chid=data['chid']
            qid=data['qid']

            qstn=Question(quiz_id=qid,chapter_id=chid,question_title=qstitle,question_statement=qstn,option1=opt1,option2=opt2,option3=opt3,option4=opt4,correct_option=correctopt)
            db.session.add(qstn)
            db.session.commit()
            return jsonify({"message":"Question added succesfully","status":"success"})
        except Exception as e:
            db.session.rollback()
        
            return jsonify({"message":"Question not added succesfully","status":"error"})
        finally:
            db.session.close()
    def get(self,id):
        try:
            qs=Question.query.get(id)
            quiz=Quiz.query.filter_by(id=qs.quiz_id).first()
            quizname=quiz.name
            chapter=Chapter.query.filter_by(id=qs.chapter_id).first()

            chaptername=chapter.name

            if qs:
                return jsonify({
                    'message':'Fetched the question details succesfully',
                    'qsid':id,
                    'quizid':qs.quiz_id,
                    'quizname':quizname,
                    'chapterid':qs.chapter_id,
                    'chaptername':chaptername,
                    'qstitle':qs.question_title,
                    'qstn':qs.question_statement,
                    'opt1':qs.option1,
                    'opt2':qs.option2,
                    'opt3':qs.option3,
                    'opt4':qs.option4,
                    'correctopt':qs.correct_option,
                    'status':"Success"
                })
        except Exception as e:
            mesg=str(e)
            
            return jsonify({
                    'message':'Some Error Occured:{mesg}',
                    'status':"error"
                })
    def delete(self,id):
        try:
            qstn=Question.query.get(id)
            db.session.delete(qstn)
            db.session.commit()
            return jsonify({"message":"Question deleted","status":"success"})
        except Exception as e:
            
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"}) 
    def put(self):
        try:
            data=request.get_json()

            qsid=data.get('qsid')
            

            qstitle=data["qstitle"]
            qstn=data['qstn']
            opt1=data['opt1']
            opt2=data['opt2']
            opt3=data['opt3']
            opt4=data['opt4']
            correctopt=data['correctopt']
            

            question=Question.query.filter_by(id=qsid).first()

            question.question_title=qstitle
            question.question_statement=qstn
            question.option1=opt1
            question.option2=opt2
            question.option3=opt3
            question.option4=opt4
            question.correct_option=correctopt  

            db.session.commit()
            return jsonify({"message":"Question Edited succesfully","status":"success"})
        except Exception as e:
            db.session.rollback()
           
            return jsonify({"message":"Question not edited succesfully","status":"error"})
        finally:
            db.session.close()

class subjectResources(Resource):
    def post(self):
        try:
            data=request.get_json()

            subname=data['subname']
            subdesc=data['subdesc']


            sub=Subject(name=subname,description=subdesc)
            db.session.add(sub)
            db.session.commit()
            return jsonify({"message":"Subject added succesfully","status":"success"})
        except Exception as e:
            db.session.rollback()
            print(f'{str(e)}')
            return jsonify({"message":"Subject not added succesfully","status":"error"})
    def delete(self,id):
        try:
            subjct=Subject.query.get(id)
            db.session.delete(subjct)
            db.session.commit()
            return jsonify({"message":"Subject deleted","status":"success"})
        except Exception as e:
            
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"})
    def get(self,id):
        try:
            subject=Subject.query.get(id)
            if subject:
                return jsonify({
                    'message':"Fetched the subject details succesfully",
                    'subid':id,
                    'subname':subject.name,
                    'subdesc':subject.description,
                    'status':"success",
                })
        except Exception as e:
            mesg=str(e)
            return jsonify({

                'message':'Some error occured: {mesg}',
                'status':'error'
            })
    def put(self):
        try:
            data=request.get_json()

            subname=data['subname']
            subdesc=data['subdesc']
            subid=data['subid']

            print(data)

            subject=Subject.query.get(subid)

            subject.name=subname
            subject.description=subdesc
            db.session.commit()
            return jsonify({"message":"Subject Edited succesfully","status":"success"})
        except Exception as e:
            db.session.rollback()
            print(str(e))
            return jsonify({"message":"Subject not edited succesfully","status":"error"})
        finally:
            db.session.close()



