from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from model import db, User, Subject, Chapter, Quiz, Question, Score
from app import app
import random

# Assuming models are already defined (User, Subject, Chapter, Quiz, Question, Score)

def reset_and_seed_large_dataset():
    with app.app_context():
        db.drop_all()
        db.create_all()

        subjects = [
            Subject(name="Mathematics", description="Algebra, Geometry, Calculus."),
            Subject(name="Science", description="Physics, Chemistry, Biology."),
            Subject(name="History", description="World History and Civilization."),
            Subject(name="Computer Science", description="Programming, Data Structures."),
            Subject(name="English", description="Literature, Grammar, and Composition."),
        ]

        users = [
            User(username=f"user{i}", password="password", full_name=f"User {i}", qualification="B.Sc", dob=datetime(2000, 1, 1))
            for i in range(1, 11)
        ]

        db.session.add_all(subjects + users)
        db.session.commit()

        for subject in subjects:
            for month in range(1, 7):
                for i in range(1, 4):  # 3 quizzes per month
                    chapter = Chapter(name=f"{subject.name} Chapter {i}", subject=subject)
                    quiz = Quiz(
                        name=f"{subject.name} Quiz {i} (Month {month})",
                        chapter=chapter,
                        date_of_quiz=datetime(2024, month, random.randint(1, 28)),
                        time_duration=str(random.randint(30, 60)),
                        description=f"Quiz {i} for {subject.name} in Month {month}"
                    )
                    db.session.add(chapter)
                    db.session.add(quiz)
                    
                    for user in users:
                        score = Score(user=user, quiz=quiz, total_scored=random.randint(5, 10))
                        db.session.add(score)

        db.session.commit()
        print("Large sample database created with multiple quizzes per month!")

if __name__ == "__main__":
    reset_and_seed_large_dataset()
