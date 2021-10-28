function renderModules(data) {
	output = document.getElementById("output");
    output.innerHTML = ""
    data.modules.forEach(function(m) {
        const moduleDiv = document.createElement("div");
        moduleDiv.className = "module"

        const moduleHeader = document.createElement("h2");
        moduleHeader.innerText = "Module " + m.module
        moduleDiv.appendChild(moduleHeader);

        const moduleSummary = document.createElement("div");
        moduleSummary.className = "module-summary"
        moduleSummary.innerHTML = "<span>Total: " + m.total_voltage + "v</span><span>Max: " + m.highest_voltage + "v</span><span>Min: " + m.lowest_voltage + "v</span><span>Avg: " + m.average_voltage + "v</span>"
        moduleDiv.appendChild(moduleSummary);

        const cellsNode = document.createElement("div");
        cellsNode.className = "module-cells"
        moduleDiv.appendChild(cellsNode);
        m.cells.forEach(function(c) {
            const cellNode = document.createElement("span");
            cellNode.className = 'module-cell'
            cellNode.innerText = c.cell + ": " + c.voltage
            cellsNode.appendChild(cellNode)
        })

        const temperatureSummary = document.createElement("div");
        temperatureSummary.className = "temperature-summary"
        temperatureSummary.innerHTML = "<span>Temperature 1: " + m.temperature1 + "c</span><span>Temperature 2: " + m.temperature2 + "c</span><span>Temperature 3: " + m.temperature3 + "v</span>"
        moduleDiv.appendChild(temperatureSummary);
        
        output.appendChild(moduleDiv)
    })
}

function updateVoltages() {

    const xhr = new XMLHttpRequest();

    // listen for `load` event
    xhr.onload = () => {

        // print JSON response
        if (xhr.status >= 200 && xhr.status < 300) {
            // parse JSON
            const response = JSON.parse(xhr.responseText);
            renderModules(response)
        }
    };

    xhr.open('GET', '/voltages');
    xhr.send()
}
function onLoad() {
    const timer = setInterval(updateVoltages, 5000);
    updateVoltages();
}