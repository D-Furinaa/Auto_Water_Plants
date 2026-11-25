function optionalNumber(value) {
  if (value === null || value === undefined) return null;
  const number = Number(value);
  return Number.isFinite(number) ? number : null;
}

function optionalBoolean(value) {
  return typeof value === "boolean" ? value : null;
}

export function parseTelemetry(topic, payload) {
  const topicMatch = /^plant\/([^/]+)\/telemetry$/.exec(topic);
  if (!topicMatch) {
    throw new Error("Unexpected telemetry topic");
  }

  let message;
  try {
    message = JSON.parse(payload.toString("utf8"));
  } catch (error) {
    throw new Error("Telemetry payload is not valid JSON", { cause: error });
  }

  if (!message || typeof message !== "object" || Array.isArray(message)) {
    throw new Error("Telemetry payload must be a JSON object");
  }

  const soil = optionalNumber(message.soil);
  if (soil !== null && (soil < 0 || soil > 100)) {
    throw new Error("Soil value must be between 0 and 100");
  }

  return {
    plantId: topicMatch[1],
    soil,
    temp: optionalNumber(message.temp),
    humidity: optionalNumber(message.hum),
    waterOk: optionalBoolean(message.water_ok),
    pumpOn: optionalBoolean(message.pump),
    mode: typeof message.mode === "string" ? message.mode : null,
    lowThreshold: optionalNumber(message.low),
    highThreshold: optionalNumber(message.high),
    alerts: optionalNumber(message.alerts),
  };
}
