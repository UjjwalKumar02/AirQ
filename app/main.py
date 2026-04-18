from fastapi import FastAPI
import joblib

model = joblib.load(
    "C:/Users/saksh/OneDrive/desktop/backend_cc/devops_projects/07_major/models/model.pkl"
)

app = FastAPI()

feedback_map = {
    "Clean Air": "Excellent air ✅",
    "Good": "Air is fine 🙂",
    "Moderate": "Limit exposure 😐",
    "Unhealthy": "Wear a mask 😷",
    "Hazardous": "Stay indoors ⚠️",
}


@app.post("/predict")
def predict(data: dict):
    aq = data["aq"]
    temp = data["temp"]
    humidity = data["humidity"]

    res = model.predict([[aq, temp, humidity]])[0]

    return {"class": res, "message": feedback_map[res]}
