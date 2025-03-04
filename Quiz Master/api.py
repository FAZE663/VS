from flask import Flask, request, jsonify, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_restful import Api, Resource
from model import *

class chapterResources(Resource):
    def post(self):
        try:
            data=request.get_json()

            subid=data["subid"]
            chaptername=data["chapter_name"]
            chdesc=data['chdesc']

            newchap=Chapter(name=chaptername,subject_id=subid,description=chdesc)
            print("succesfully added it")
            db.session.add(newchap)
            db.session.commit()
            return jsonify({"message":"Chapter added succesfully","status":"success"})

        except Exception as e:
            db.session.rollback()
            print(str(e))
            return jsonify({"message":"Chapter not added succesfully","status":"error"})
        finally:
            db.session.close()
    def put(self):
        try:
            data=request.get_json()
            chapterid=data.get('chid')
            chaptername=data["chname"]
            chdesc=data["chdes"]
            print(f'{chapterid},{chaptername},{chdesc}')
            chapter=Chapter.query.filter_by(id=chapterid).first()
            if not chapter:
                return jsonify({'message':'Chapter not found','status':"error"})
            
            chapter.name=chaptername
            chapter.description=chdesc

            db.session.commit()
            return jsonify({"message":"Chapter aedited succesfully","status":"success"})
        except Exception as e:
            print(f'error: {str(e)}')
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
            print(f'error: {str(e)}')
            db.session.rollback()
            return jsonify({'message':'Some error occured','status':"error"}) 
        

