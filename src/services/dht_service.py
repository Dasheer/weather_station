from flask import request, Response
from bson import json_util, ObjectId

from config.mongodb import mongo

def get_all_data_dht():
#     data = mongo.db.sensor_data.find()
#     result = json_util.dumps(data)
#     return Response(result, mimetype='application/json')

    data = mongo.db.sensor_data
    result = []
    for d in data.find():
        result.append({
            'id': str(d['_id']),
            'sensor_type': d['sensor_type'],
            'temperature': d['temperature'],
            'humidity': d['humidity']
        })
    return result


def create_data_dht():
    data = request.get_json()
    sensor_type = data['sensor_type']
    temperature = data['temperature']
    humidity = data['humidity']
    if sensor_type and temperature and humidity:
        id = mongo.db.sensor_data.insert_one({
            'sensor_type': sensor_type,
            'temperature': temperature,
            'humidity': humidity
            })
        response = {
            'id': str(id),
            'sensor_type': sensor_type,
            'temperature': temperature,
            'humidity': humidity
        }
        return response
    else:
        return "Error!", 400

    # data = mongo.db.sensor_data
    # result = []
    # for d in data.find():
    #     result.append({
    #         'sensor_type': d['sensor_type'],
    #         'temperature': d['temperature'],
    #         'humidity': d['humidity']
    #     })
    # return result