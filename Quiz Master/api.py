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
