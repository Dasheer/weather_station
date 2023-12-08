import os
from flask import Flask, request, render_template
from dotenv import load_dotenv

from config.mongodb import mongo
from routes.api_dht import api_dht
from routes.api_pressure import api_pressure
from routes.api_anemometro import api_anemometro
from routes.api_wind_velocity import api_wind_velocity

load_dotenv()

app = Flask(__name__)

app.config["MONGO_URI"] = os.getenv("MONGO_URI")
mongo.init_app(app)

@app.route('/')
def index():
    return render_template('index.html')

app.register_blueprint(api_dht, url_prefix='/api/v1')
app.register_blueprint(api_pressure, url_prefix='/api/v1')
app.register_blueprint(api_anemometro, url_prefix='/api/v1')
app.register_blueprint(api_wind_velocity, url_prefix='/api/v1')

if __name__ == '__main__':
    app.run(debug=True)