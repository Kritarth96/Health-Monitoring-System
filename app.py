from flask import Flask, request, jsonify, render_template
from collections import deque
from datetime import datetime

app = Flask(__name__)

# Store last 100 sensor records in memory
data_buffer = deque(maxlen=100)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/send_data', methods=['POST'])
def receive_data():
    try:
        data = request.get_json()

        # Add timestamp
        data['timestamp'] = datetime.now().isoformat()

        # Add to buffer
        data_buffer.append(data)
        return jsonify({'status': 'success'}), 200
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 400

@app.route('/get_health_data', methods=['GET'])
def get_data():
    result = [
        [
            d.get("timestamp"),
            d.get("ecg", 0),
            d.get("spo2", 0),
            d.get("heart_rate", 0),
            d.get("temperature", 0),
            d.get("fever", False),
            0,  # Reserved field for future use
            d.get("accel_x", 0),
            d.get("accel_y", 0),
            d.get("accel_z", 0),
            d.get("fall", False)
        ]
        for d in data_buffer
    ]
    return jsonify(result)

if __name__ == '__main__':
    # Make Flask listen on all IPs (required for Arduino access)
    app.run(host='0.0.0.0', port=5000, debug=True)
