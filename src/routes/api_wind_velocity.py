from flask import Blueprint

from services.wind_velocity_service import get_wind_velocity_data, save_sensor_data

api_wind_velocity = Blueprint('api_wind_velocity', __name__)

@api_wind_velocity.route('/sensor/wind_velocity/stream', methods=['GET'])
def stream_data():
    return get_wind_velocity_data()

@api_wind_velocity.route('/sensor/wind_velocity', methods=['POST'])
def add_data_wind_velocity():
    return save_sensor_data()