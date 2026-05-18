from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
import psycopg

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")


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


@app.get("/data")
def get_data():
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
                SELECT
                    temperature,
                    humidity,
                    pressure,
                    created_at
                FROM sensor_data
                ORDER BY created_at DESC
                LIMIT 100
                """
            )
            rows = cur.fetchall()

    return [
        {
            "temperature": row[0],
            "humidity": row[1],
            "pressure": row[2],
            "created_at": row[3].isoformat(),
        }
        for row in reversed(rows)
    ]
