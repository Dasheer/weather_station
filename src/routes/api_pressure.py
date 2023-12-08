from flask import Blueprint

from services.pressure_service import get_pressure_data, save_sensor_data

api_pressure = Blueprint('api_pressure', __name__)

@api_pressure.route('/sensor/pressure/stream', methods=['GET'])
def get_data():
    return get_pressure_data()

@api_pressure.route('/sensor/pressure', methods=['POST'])
def add_data_pressure():
    return save_sensor_data()