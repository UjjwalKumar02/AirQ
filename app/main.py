from fastapi import FastAPI
import joblib
from datetime import datetime, timezone
import requests

model = joblib.load(
    "C:/Users/saksh/OneDrive/desktop/backend_cc/devops_projects/07_major/models/model.pkl"
)

app = FastAPI()

NODE_WS_API = "http://localhost:8080/send-data"

feedback_map = {
    "Clean Air": "Fresh and safe air. Great for outdoor activities.",
    "Good": "Air is fine. Safe for most people.",
    "Moderate": "Okay air. Sensitive people should limit exposure.",
    "Unhealthy": "Poor air. Wear a mask and reduce outdoor time.",
    "Hazardous": "Very dangerous air. Stay indoors.",
}


@app.post("/predict")
def predict(data: dict):
    aq = data["aq"]
    temp = data["temp"]
    humidity = data["humidity"]

    prediction = model.predict([[aq, temp, humidity]])[0]

    response_data = {
        "aqi": aq,
        "temperature": temp,
        "humidity": humidity,
        "prediction": prediction,
        "airClass": prediction,
        "message": feedback_map[prediction],
    }

    try:
        requests.post(NODE_WS_API, json=response_data)
    except:
        print("WS server not reachable")

    return {"class": prediction, "message": feedback_map[prediction]}
