from flask import Blueprint

from services.anemometro_service import get_anemometro_data, save_sensor_data

api_anemometro = Blueprint('api_anemometro', __name__)

@api_anemometro.route('/sensor/anemometro/stream', methods=['GET'])
def stream_data():
    return get_anemometro_data()

@api_anemometro.route('/sensor/anemometro', methods=['POST'])
def add_data_anemometro():
    return save_sensor_data()