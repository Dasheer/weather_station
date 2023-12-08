from flask import request, Response, jsonify
from bson import json_util, ObjectId
import time

from config.mongodb import mongo

def get_anemometro_data():
    def generate():
        while True:
            data_cursor = mongo.db.sensor_anemometro.find({'sensor_type': 'anemometro'}).sort([('_id', -1)]).limit(1)

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
        velocity = data['velocity']
        velocityMax = data['velocityMax']

        if sensor_type and velocity and velocityMax:
            id = mongo.db.sensor_anemometro.insert_one({
                'sensor_type': sensor_type,
                'velocity': velocity,
                'velocityMax': velocityMax
            })
            response = {
                'id': str(id),
                'sensor_type': sensor_type,
                'velocity': velocity,
                'velocityMax': velocityMax
            }
            return jsonify(response), 200
        else:
            return jsonify({'error': 'Bad Request'}), 500
    except Exception as e:
        print(e)
        return jsonify({'error': str(e)}), 500