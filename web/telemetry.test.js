import test from "node:test";
import assert from "node:assert/strict";
import { parseTelemetry } from "./telemetry.js";

test("parses telemetry published by the ESP32 topic", () => {
  const telemetry = parseTelemetry(
    "plant/1/telemetry",
    Buffer.from('{"soil":42.5,"temp":null,"hum":55,"water_ok":true,"pump":false,"mode":"auto","low":30,"high":60,"alerts":0}')
  );

  assert.deepEqual(telemetry, {
    plantId: "1",
    soil: 42.5,
    temp: null,
    humidity: 55,
    waterOk: true,
    pumpOn: false,
    mode: "auto",
    lowThreshold: 30,
    highThreshold: 60,
    alerts: 0,
  });
});

test("accepts telemetry containing only soil data", () => {
  const telemetry = parseTelemetry("plant/2/telemetry", Buffer.from('{"soil":31}'));
  assert.equal(telemetry.plantId, "2");
  assert.equal(telemetry.soil, 31);
  assert.equal(telemetry.temp, null);
  assert.equal(telemetry.humidity, null);
});

test("rejects malformed, unexpected, and out-of-range telemetry", () => {
  assert.throws(() => parseTelemetry("plant/1/status", Buffer.from("{}")), /Unexpected telemetry topic/);
  assert.throws(() => parseTelemetry("plant/1/telemetry", Buffer.from("{")), /not valid JSON/);
  assert.throws(() => parseTelemetry("plant/1/telemetry", Buffer.from('{"soil":101}')), /between 0 and 100/);
});
