const eventSource = new EventSource("/api/v1/sensor/dht/stream");

eventSource.onmessage = function (event) {
    try {
        const sensorData = JSON.parse(event.data);

        // Redondear la temperatura y la humedad a dos decimales
        const temperature = sensorData.temperature.toFixed(0);
        const humidity = sensorData.humidity.toFixed(0);

        document.getElementById("temperature").innerHTML = `
            <p class="heading">${temperature}&deg;<sup>c</sup></p>
        `;

        document.getElementById("humidity").innerHTML = `
            <p>Humidity: ${humidity} %</p>
        `;
    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

eventSource.onerror = function (error) {
    console.error("EventSource failed:", error);
};