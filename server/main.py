from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class SensorData(BaseModel):
    temperature: float
    humidity: float
    pressure: float

@app.post("/data")
def receive_data(data: SensorData):
    """Receive a sensor data."""
    print(data)
    return {"status": "ok"}
