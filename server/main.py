from fastapi import FastAPI
from pydantic import BaseModel
import psycopg

app = FastAPI()


class SensorData(BaseModel):
    temperature: float
    humidity: float
    pressure: float


@app.post("/data")
def receive_data(data: SensorData):
    """Receive a sensor data."""
    with psycopg.connect(
        host="localhost",
        port=5432,
        dbname="sensors",
        user="postgres",
        password="postgres"
    ) as conn:
        with conn.cursor() as cur:
            cur.execute(
                """
                INSERT INTO sensor_data
                (temperature, humidity, pressure)
                VALUES (%s, %s, %s)
                """,
                (
                    data.temperature,
                    data.humidity,
                    data.pressure,
                )
            )
        conn.commit()

    print(data)
    return {"status": "ok"}
