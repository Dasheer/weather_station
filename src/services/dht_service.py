from flask import request, Response, jsonify
from bson import json_util, ObjectId
import time

from config.mongodb import mongo

def get_dht_data():
    def generate():
        while True:
            # Obtiene el último registro de la colección sensor_data
            data_cursor = mongo.db.sensor_dht.find({'sensor_type': 'dht'}).sort([('_id', -1)]).limit(1)

            # Convierte el cursor en una lista
            data_list = list(data_cursor)

            # Extrae el primer (y único) elemento de la lista
            data = data_list[0] if data_list else None

            # Formatea el objeto JSON para que sea legible
            if data and '_id' in data:
                data['_id'] = str(data['_id'])

            result = json_util.dumps(data, default=str)
            yield f"data:{result}\n\n"
            time.sleep(5)
    return Response(generate(), mimetype='text/event-stream')

def get_all_data_dht():
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

def save_sensor_data():
    try:
        data = request.get_json()
        sensor_type = data['sensor_type']
        temperature = data['temperature']
        humidity = data['humidity']
        if sensor_type and temperature and humidity:
            id = mongo.db.sensor_dht.insert_one({
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
            return jsonify(response), 200
        else:
            return jsonify({'error': 'Bad Request'}), 400
    except Exception as e:
        return jsonify({'error': str(e)}), 500
