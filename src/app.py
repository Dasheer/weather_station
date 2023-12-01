import os
from flask import Flask, request
from dotenv import load_dotenv

from config.mongodb import mongo
from routes.api_dht import api_dht

load_dotenv()

app = Flask(__name__)

app.config["MONGO_URI"] = os.getenv("MONGO_URI")
mongo.init_app(app)

@app.route('/')
def index():
    return 'Hello World!'

app.register_blueprint(api_dht, url_prefix='/api/v1')

if __name__ == '__main__':
    app.run(debug=True)