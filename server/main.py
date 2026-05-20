from os import environ

from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
from dotenv import load_dotenv
from psycopg import connect

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")

load_dotenv()


class SensorReading(BaseModel):
    sensor: str
    temperature_c: float
    humidity_rh: float
    pressure_hpa: float


@app.post("/data")
def receive_data(data: SensorReading):
    """Receive a sensor data."""
    with connect(environ["DATABASE_URL"]) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "SELECT id FROM sensors WHERE name = %s", (data.sensor,)
            )
            row = cur.fetchone()
            if row is None:
                return {"error": "Unknown sensor"}
            sensor_id = row[0]

            cur.execute(
                """
                INSERT INTO sensor_readings
                (sensor_id, temperature_c, humidity_rh, pressure_hpa)
                VALUES (%s, %s, %s, %s)
                """,
                (
                    sensor_id,
                    data.temperature_c,
                    data.humidity_rh,
                    data.pressure_hpa,
                )
            )

        conn.commit()

    print(data)
    return {"status": "ok"}


@app.get("/data")
def get_data():
    with connect(environ["DATABASE_URL"]) as conn:
        with conn.cursor() as cur:
            cur.execute(
                """
                SELECT
                    temperature_c,
                    humidity_rh,
                    pressure_hpa,
                    created_at
                FROM sensor_readings
                WHERE created_at >= NOW() - INTERVAL '1 hour'
                ORDER BY created_at
                """
            )
            rows = cur.fetchall()

    return [
        {
            "temperature_c": row[0],
            "humidity_rh": row[1],
            "pressure_hpa": row[2],
            "created_at": row[3].isoformat(),
        }
        for row in rows
    ]
