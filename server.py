from flask import Flask, render_template,url_for,request,redirect,make_response
import json
from time import time

app = Flask(__name__)
@app.route('/',methods=["GET","POST"])
def main():
	return render_template("index.htm")

@app.route('/data', methods=["GET","POST"])
def data():
	Moisture = request.args.get("soilMoist")
	Intensity = request.args.get("ldrSense")
	Temperature = request.args.get("tempVal")

	print(Moisture)
	print(Intensity)
	print(Temperature)

	data = [time()*10000, Moisture, Intensity, Temperature]
	response = make_response(json.dumps(data))

	response.content_type = 'application/json'

	return response

if __name__ == "__main__":
	app.run(debug=True)


