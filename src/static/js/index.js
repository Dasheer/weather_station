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
            <p class="title-1"> ${humidity}<sub>%</sub></p>
        `;
    } catch (error) {
        console.error("Error parsing JSON:", error);
    }
};

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

eventSource.onerror = function (error) {
    console.error("EventSource failed:", error);
};