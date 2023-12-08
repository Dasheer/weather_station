const eventSource = new EventSource("/api/v1/sensor/dht/stream");
const getDataPressure = new EventSource("/api/v1/sensor/pressure/stream");
const getDataAnemometro = new EventSource("/api/v1/sensor/anemometro/stream");
const getDataWindVelocity = new EventSource("/api/v1/sensor/wind_velocity/stream");

// Obtener datos de temperatura y humedad
eventSource.onmessage = function (event) {
    try {
        const sensorData = JSON.parse(event.data);

        // Redondear la temperatura y la humedad a dos decimales
        const temperature = sensorData.temperature.toFixed(0);
        const humidity = sensorData.humidity.toFixed(0);

        var image = "";
        var weatherDescription = '';

        if (temperature < 15) {
            image = document.getElementById("temperature").dataset.imgCold;
            weatherDescription = 'Frío';
        } else if (temperature < 25) {
            image = document.getElementById("temperature").dataset.imgCloudy;
            weatherDescription = 'Nublado';
        } else if (temperature < 35) {
            image = document.getElementById("temperature").dataset.imgSunny;
            weatherDescription = 'Soleado';
        }

        document.getElementById("temperature").innerHTML = `
            <p class="heading">${temperature}&deg;<sup>c</sup></p>
            <img src="${image}" alt="Weather Icon" class="weather-icon" />
            <p class="body-3">${weatherDescription}</p>
        `;

        document.getElementById("humidity").innerHTML = `
            <p class="title-1"> ${humidity}<sub>%</sub></p>
        `;

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

eventSource.onerror = function (error) {
    console.error("EventSource failed:", error);
};

// Obtener datos de la presión barometrica
getDataPressure.onmessage = function (event) {
    try {
        const sensorData = JSON.parse(event.data);

        // Redondear la temperatura y la humedad a dos decimales
        const pressure = sensorData.absolute_pressure.toFixed(2);

        document.getElementById("pressure").innerHTML = `
            <p class="title-1"> ${pressure} <sub>hPa</sub></p>
        `;

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

getDataPressure.onerror = function (error) {
    console.error("EventSource failed:", error);
}

// Obtener datos del anemometro
getDataAnemometro.onmessage = function (event) {
    try {
        const sensorData = JSON.parse(event.data);

        // Redondear la temperatura y la humedad a dos decimales
        const anemometro = sensorData.velocityMax.toFixed(2);

        document.getElementById("anemometro").innerHTML = `
            <p class="title-1"> ${anemometro}<sub>km/h</sub></p>
        `;

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

getDataAnemometro.onerror = function (error) {
    console.error("EventSource failed:", error);
}

// Obtener datos de la velocidad del viento
getDataWindVelocity.onmessage = function (event) {
    try {
        const sensorData = JSON.parse(event.data);

        // Redondear la temperatura y la humedad a dos decimales
        const windVelocity = sensorData.direction;
        const geografia = sensorData.geografico.toFixed(2);

        document.getElementById("windVelocity").innerHTML = `
            <p class="title-1"> ${windVelocity}</p>
            <p class="title-1"> ${geografia}</p>
        `;

    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

getDataWindVelocity.onerror = function (error) {
    console.error("EventSource failed:", error);
}

// Obtener la fecha actual
const currentDate = new Date();

// Días de la semana y meses en español
const daysOfWeek = ['Domingo', 'Lunes', 'Martes', 'Miércoles', 'Jueves', 'Viernes', 'Sábado'];
const months = ['Enero', 'Febrero', 'Marzo', 'Abril', 'Mayo', 'Junio', 'Julio', 'Agosto', 'Septiembre', 'Octubre', 'Noviembre', 'Diciembre'];

// Obtener el día de la semana y el mes en formato deseado
const dayOfWeek = daysOfWeek[currentDate.getDay()];
const day = currentDate.getDate();
const month = months[currentDate.getMonth()];

// Formatear la fecha
const formattedDate = `${dayOfWeek}, ${day} de ${month}`;

// Mostrar la fecha en el elemento con el ID "fechaMostrada"
const dateElement = document.getElementById('currentDate').innerHTML = `
    <p class="title-3 meta-text">${dayOfWeek}, ${day} de ${month}</p>
`;

document.addEventListener('DOMContentLoaded', function () {
    // Obtener el elemento con el ID "copyright"
    const copyrightElement = document.getElementById('copyright');

    // Obtener el año actual
    const currentYear = new Date().getFullYear();

    // Configurar el texto del copyright
    const copyrightText = `Copyright © ${currentYear} Weather Station. Todos los derechos reservados.`;

    // Mostrar el texto en el elemento
    copyrightElement.textContent = copyrightText;
});

