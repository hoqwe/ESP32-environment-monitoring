CREATE TABLE IF NOT EXISTS sensors (
    id      SMALLSERIAL PRIMARY KEY,
    name    TEXT NOT NULL UNIQUE
);

INSERT INTO sensors (name) VALUES ('indoor'), ('balcony')
ON CONFLICT DO NOTHING;

CREATE TABLE IF NOT EXISTS sensor_readings (
    id               SERIAL PRIMARY KEY,

    sensor_id        SMALLINT NOT NULL
                     REFERENCES sensors(id)
                     ON DELETE CASCADE,

    temperature_c    REAL NOT NULL,
    humidity_rh      REAL NOT NULL,
    pressure_hpa     REAL NOT NULL,

    created_at       TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_sensor_readings_created_at
ON sensor_readings(sensor_id, created_at DESC);
