from flask import Blueprint

from services.dht_service import get_all_data_dht, save_sensor_data, get_dht_data

api_dht = Blueprint('api', __name__)

@api_dht.route('/sensor/dht/stream', methods=['GET'])
def stream_data():
    return get_dht_data()

@api_dht.route('/sensor/dht', methods=['GET'])
def get_data():
    return get_all_data_dht()

@api_dht.route('/sensor/dht', methods=['POST'])
def add_data_dht():
    return save_sensor_data()
