const plantFilter = document.querySelector("#plant-filter");
const connection = document.querySelector("#connection");
const connectionLabel = document.querySelector("#connection-label");
const svgNamespace = "http://www.w3.org/2000/svg";

function queryString() {
  const params = new URLSearchParams();
  if (plantFilter.value) params.set("plantId", plantFilter.value);
  return params.toString();
}

async function fetchJson(url) {
  const response = await fetch(url);
  if (!response.ok) throw new Error(`Request failed (${response.status})`);
  return response.json();
}

function formatTime(value) {
  if (!value) return "Chưa có dữ liệu";
  return new Date(value).toLocaleString("vi-VN");
}

function setState(element, value, positiveLabel, negativeLabel) {
  element.textContent = value === null ? "--" : value ? positiveLabel : negativeLabel;
  element.classList.toggle("is-on", value === true);
  element.classList.toggle("is-off", value === false);
}

function updateLatest(data) {
  if (!data) {
    document.querySelector("#soil-value").textContent = "--";
    document.querySelector("#soil-meter").style.width = "0%";
    setState(document.querySelector("#pump-state"), null, "ĐANG CHẠY", "ĐANG TẮT");
    setState(document.querySelector("#water-state"), null, "ĐỦ NƯỚC", "HẾT NƯỚC");
    document.querySelector("#soil-time").textContent = "Chưa có dữ liệu";
    document.querySelector("#pump-mode").textContent = "Chế độ: --";
    document.querySelector("#alerts").textContent = "Cảnh báo: --";
    document.querySelector("#climate-value").textContent = "--";
    return;
  }

  document.querySelector("#soil-value").textContent = data.soil ?? "--";
  document.querySelector("#soil-meter").style.width = `${Math.max(0, Math.min(100, data.soil ?? 0))}%`;
  document.querySelector("#soil-time").textContent = `Cập nhật: ${formatTime(data.received_at)}`;
  setState(document.querySelector("#pump-state"), data.pump_on, "ĐANG CHẠY", "ĐANG TẮT");
  setState(document.querySelector("#water-state"), data.water_ok, "ĐỦ NƯỚC", "HẾT NƯỚC");
  document.querySelector("#pump-mode").textContent = `Chế độ: ${data.mode ?? "--"}`;
  document.querySelector("#alerts").textContent = `Cảnh báo: ${data.alerts ?? "--"}`;

  const climate = [];
  if (data.temp !== null) climate.push(`${data.temp} °C`);
  if (data.humidity !== null) climate.push(`${data.humidity}%`);
  document.querySelector("#climate-value").textContent = climate.join(" · ") || "--";
}

function svgElement(name, attributes = {}) {
  const element = document.createElementNS(svgNamespace, name);
  for (const [key, value] of Object.entries(attributes)) element.setAttribute(key, value);
  return element;
}

function drawChart(rows) {
  const chart = document.querySelector("#soil-chart");
  const emptyMessage = document.querySelector("#chart-empty");
  chart.replaceChildren();
  const samples = rows.filter((row) => Number.isFinite(row.soil));
  document.querySelector("#sample-count").textContent = `${samples.length} mẫu gần nhất`;

  if (samples.length === 0) {
    emptyMessage.hidden = false;
    return;
  }
  emptyMessage.hidden = true;

  for (const value of [0, 25, 50, 75, 100]) {
    const y = 210 - value * 1.8;
    chart.append(svgElement("line", { x1: 48, y1: y, x2: 704, y2: y, class: "grid-line" }));
    const label = svgElement("text", { x: 8, y: y + 4, class: "axis-label" });
    label.textContent = `${value}%`;
    chart.append(label);
  }

  const points = samples.map((row, index) => {
    const x = samples.length === 1 ? 376 : 48 + index * (656 / (samples.length - 1));
    const y = 210 - row.soil * 1.8;
    return `${x},${y}`;
  }).join(" ");
  chart.append(svgElement("polyline", { points, class: "soil-line" }));

  samples.forEach((row, index) => {
    const x = samples.length === 1 ? 376 : 48 + index * (656 / (samples.length - 1));
    const y = 210 - row.soil * 1.8;
    const point = svgElement("circle", { cx: x, cy: y, r: 3.5, class: "soil-point" });
    const title = svgElement("title");
    title.textContent = `${row.soil}% · ${formatTime(row.received_at)}`;
    point.append(title);
    chart.append(point);
  });
}

async function refreshDashboard() {
  try {
    const query = queryString();
    const suffix = query ? `?${query}` : "";
    const [status, latest, history] = await Promise.all([
      fetchJson("/api/status"),
      fetchJson(`/api/telemetry/latest${suffix}`),
      fetchJson(`/api/telemetry/history?limit=100${query ? `&${query}` : ""}`),
    ]);
    connection.classList.toggle("connected", status.mqttConnected);
    connectionLabel.textContent = status.mqttConnected ? "MQTT đã kết nối" : "Đang chờ MQTT";
    updateLatest(latest);
    drawChart(history);
    document.querySelector("#last-updated").textContent = `Làm mới lúc ${new Date().toLocaleTimeString("vi-VN")}`;
  } catch (error) {
    connection.classList.remove("connected");
    connectionLabel.textContent = "Không kết nối được máy chủ";
    console.error("Không thể tải dữ liệu dashboard:", error);
  }
}

plantFilter.addEventListener("change", refreshDashboard);
refreshDashboard();
window.setInterval(refreshDashboard, 5000);
