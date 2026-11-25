import "dotenv/config";
import express from "express";
import mqtt from "mqtt";
import { DatabaseSync } from "node:sqlite";
import { mkdirSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { parseTelemetry } from "./telemetry.js";

const webDirectory = dirname(fileURLToPath(import.meta.url));
const databasePath = resolve(webDirectory, process.env.DATABASE_PATH ?? "./data/plant-data.sqlite");
const port = Number(process.env.PORT ?? 3000);
const mqttUrl = process.env.MQTT_URL ?? "mqtt://172.172.12.24:1883";
const mqttTopic = process.env.MQTT_TOPIC ?? "plant/+/telemetry";

if (!Number.isInteger(port) || port < 1 || port > 65535) {
  throw new Error("PORT must be an integer between 1 and 65535");
}

mkdirSync(dirname(databasePath), { recursive: true });

const database = new DatabaseSync(databasePath);
database.exec("PRAGMA journal_mode = WAL");
database.exec(`
  CREATE TABLE IF NOT EXISTS telemetry (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    plant_id TEXT NOT NULL,
    received_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ', 'now')),
    soil REAL,
    temp REAL,
    humidity REAL,
    water_ok INTEGER,
    pump_on INTEGER,
    mode TEXT,
    low_threshold REAL,
    high_threshold REAL,
    alerts INTEGER
  );
  CREATE INDEX IF NOT EXISTS telemetry_plant_received
    ON telemetry (plant_id, received_at DESC);
`);

const insertTelemetry = database.prepare(`
  INSERT INTO telemetry (
    plant_id, soil, temp, humidity, water_ok, pump_on, mode,
    low_threshold, high_threshold, alerts
  ) VALUES (
    ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
  )
`);

const mqttClient = mqtt.connect(mqttUrl, {
  username: process.env.MQTT_USERNAME || undefined,
  password: process.env.MQTT_PASSWORD || undefined,
  clientId: `plant-dashboard-${process.pid}`,
  reconnectPeriod: 5000,
});

mqttClient.on("connect", () => {
  console.info(`[mqtt] connected to ${mqttUrl}`);
  mqttClient.subscribe(mqttTopic, { qos: 1 }, (error) => {
    if (error) {
      console.error(`[mqtt] failed to subscribe to ${mqttTopic}:`, error.message);
      return;
    }
    console.info(`[mqtt] subscribed to ${mqttTopic}`);
  });
});

mqttClient.on("reconnect", () => console.info("[mqtt] reconnecting"));
mqttClient.on("error", (error) => console.error("[mqtt] connection error:", error.message));
mqttClient.on("message", (topic, payload) => {
  try {
    const telemetry = parseTelemetry(topic, payload);
    insertTelemetry.run(
      telemetry.plantId,
      telemetry.soil,
      telemetry.temp,
      telemetry.humidity,
      telemetry.waterOk === null ? null : Number(telemetry.waterOk),
      telemetry.pumpOn === null ? null : Number(telemetry.pumpOn),
      telemetry.mode,
      telemetry.lowThreshold,
      telemetry.highThreshold,
      telemetry.alerts
    );
    console.info(`[db] saved telemetry for plant ${telemetry.plantId}`);
  } catch (error) {
    console.error(`[mqtt] ignored message on ${topic}:`, error.message);
  }
});

function apiTelemetry(row) {
  if (!row) return null;
  return {
    ...row,
    water_ok: row.water_ok === null ? null : Boolean(row.water_ok),
    pump_on: row.pump_on === null ? null : Boolean(row.pump_on),
  };
}

const app = express();
app.disable("x-powered-by");
app.use(express.static(resolve(webDirectory, "public")));

app.get("/api/telemetry/latest", (request, response, next) => {
  try {
    const plantId = typeof request.query.plantId === "string" ? request.query.plantId : null;
    const row = plantId
      ? database.prepare(`
          SELECT * FROM telemetry WHERE plant_id = ? ORDER BY received_at DESC, id DESC LIMIT 1
        `).get(plantId)
      : database.prepare(`
          SELECT * FROM telemetry ORDER BY received_at DESC, id DESC LIMIT 1
        `).get();
    response.json(apiTelemetry(row));
  } catch (error) {
    next(error);
  }
});

app.get("/api/telemetry/history", (request, response, next) => {
  try {
    const requestedLimit = Number.parseInt(request.query.limit ?? "100", 10);
    if (!Number.isInteger(requestedLimit) || requestedLimit < 1 || requestedLimit > 500) {
      response.status(400).json({ error: "limit must be an integer from 1 to 500" });
      return;
    }

    const plantId = typeof request.query.plantId === "string" ? request.query.plantId : null;
    const rows = plantId
      ? database.prepare(`
          SELECT * FROM (
            SELECT * FROM telemetry WHERE plant_id = ? ORDER BY received_at DESC, id DESC LIMIT ?
          ) ORDER BY received_at ASC, id ASC
        `).all(plantId, requestedLimit)
      : database.prepare(`
          SELECT * FROM (
            SELECT * FROM telemetry ORDER BY received_at DESC, id DESC LIMIT ?
          ) ORDER BY received_at ASC, id ASC
        `).all(requestedLimit);
    response.json(rows.map(apiTelemetry));
  } catch (error) {
    next(error);
  }
});

app.get("/api/status", (_request, response) => {
  response.json({
    mqttConnected: mqttClient.connected,
    topic: mqttTopic,
  });
});

app.use((error, _request, response, _next) => {
  console.error("[http] request failed:", error);
  response.status(500).json({ error: "Internal server error" });
});

const server = app.listen(port, "0.0.0.0", () => {
  console.info(`[web] dashboard available on http://0.0.0.0:${port}`);
  console.info(`[db] using ${databasePath}`);
});

function shutdown() {
  server.close(() => {
    mqttClient.end(false, {}, () => {
      database.close();
      process.exit(0);
    });
  });
}

process.on("SIGINT", shutdown);
process.on("SIGTERM", shutdown);
