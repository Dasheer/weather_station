from flask import request, Response, jsonify
from bson import json_util, ObjectId
import time

from config.mongodb import mongo

def get_wind_velocity_data():
    def generate():
        while True:
            data_cursor = mongo.db.wind_velocity.find({'sensor_type': 'wind_direction'}).sort([('_id', -1)]).limit(1)

            data_list = list(data_cursor)

            data = data_list[0] if data_list else None

            if data and '_id' in data:
                data['_id'] = str(data['_id'])

            result = json_util.dumps(data, default=str)
            yield f"data:{result}\n\n"
            time.sleep(5)
    return Response(generate(), mimetype='text/event-stream')


def save_sensor_data():
    try:
        data = request.get_json()
        sensor_type = data['sensor_type']
        direction = data['direction']
        geografico = data['geografico']

        if sensor_type and direction and geografico:
            id = mongo.db.wind_velocity.insert_one({
                'sensor_type': sensor_type,
                'direction': direction,
                'geografico': geografico
            })
            response = {
                'id': str(id),
                'sensor_type': sensor_type,
                'direction': direction,
                'geografico': geografico
            }
            return jsonify(response), 200
        else:
            return jsonify({'error': 'Bad Request'}), 500
    except Exception as e:
        print(e)
        return jsonify({'error': str(e)}), 500