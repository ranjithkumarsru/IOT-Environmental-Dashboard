
// AWS API URL
const API_URL = "https://esyjdox5fj.execute-api.us-east-1.amazonaws.com/prod/readings";

// Dashboard elements
const temperatureElement = document.getElementById("temperature");
const humidityElement = document.getElementById("humidity");
const historyBody = document.getElementById("historyBody");
const statusElement = document.querySelector(".status");

// Chart data
const labels = [];
const temperatureData = [];
const humidityData = [];

// Temperature chart
const tempChart = new Chart(
    document.getElementById("tempChart"),
    {
        type: "line",
        data: {
            labels: labels,
            datasets: [{
                label: "Temperature (°C)",
                data: temperatureData,
                borderColor: "#ff9b70",
                backgroundColor: "rgba(255,155,112,0.15)",
                fill: true,
                tension: 0.4
            }]
        },
        options: {
            responsive: true,
            plugins: {
                legend: {
                    labels: { color: "white" }
                }
            },
            scales: {
                x: { ticks: { color: "white" } },
                y: { ticks: { color: "white" } }
            }
        }
    }
);

// Humidity chart
const humidityChart = new Chart(
    document.getElementById("humidityChart"),
    {
        type: "line",
        data: {
            labels: labels,
            datasets: [{
                label: "Humidity (%)",
                data: humidityData,
                borderColor: "#4db8ff",
                backgroundColor: "rgba(77,184,255,0.15)",
                fill: true,
                tension: 0.4
            }]
        },
        options: {
            responsive: true,
            plugins: {
                legend: {
                    labels: { color: "white" }
                }
            },
            scales: {
                x: { ticks: { color: "white" } },
                y: {
                    min: 0,
                    max: 100,
                    ticks: { color: "white" }
                }
            }
        }
    }
);

// Update connection status
function updateStatus(connected) {
    statusElement.innerHTML = "";

    const dot = document.createElement("span");
    dot.className = "dot";
    dot.style.background = connected ? "#55e0a2" : "#ff5555";

    statusElement.appendChild(dot);
    statusElement.appendChild(
        document.createTextNode(
            connected ? " AWS Connected" : " Connection Error"
        )
    );

    statusElement.style.color = connected ? "#55e0a2" : "#ff7777";
}

// Format timestamp
function formatTime(timestamp) {
    return new Date(timestamp).toLocaleString();
}

// Fetch actual readings from AWS
async function updateDashboard() {
    try {
        const response = await fetch(API_URL);

        if (!response.ok) {
            throw new Error("API request failed: " + response.status);
        }

        const result = await response.json();
        const readings = result.readings || [];

        if (readings.length === 0) {
            throw new Error("No sensor readings available");
        }

        // Latest reading is the first item
        const latest = readings[0];

        // Display actual temperature and humidity
        temperatureElement.textContent =
            Number(latest.temperature).toFixed(1);

        humidityElement.textContent =
            Number(latest.humidity).toFixed(1);

        // Update connection status
        updateStatus(true);

        // Arrange readings from oldest to newest for charts
        const chronological = [...readings].reverse();

        labels.length = 0;
        temperatureData.length = 0;
        humidityData.length = 0;

        chronological.forEach(reading => {
            labels.push(
                new Date(reading.timestamp).toLocaleTimeString()
            );

            temperatureData.push(Number(reading.temperature));
            humidityData.push(Number(reading.humidity));
        });

        tempChart.update();
        humidityChart.update();

        // Update history table (newest first)
        historyBody.replaceChildren();

        readings.forEach(reading => {
            const row = document.createElement("tr");

            const timeCell = document.createElement("td");
            const tempCell = document.createElement("td");
            const humidityCell = document.createElement("td");

            timeCell.textContent = formatTime(reading.timestamp);
            tempCell.textContent =
                Number(reading.temperature).toFixed(1) + " °C";
            humidityCell.textContent =
                Number(reading.humidity).toFixed(1) + " %";

            row.append(timeCell, tempCell, humidityCell);
            historyBody.appendChild(row);
        });

        console.log("AWS readings updated successfully:", readings);

    } catch (error) {
        console.error("Dashboard error:", error);
        updateStatus(false);
    }
}

// Initial update
updateDashboard();

// Automatically refresh every 5 seconds
setInterval(updateDashboard, 5000);
