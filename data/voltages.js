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
        moduleSummary.innerHTML = "<span>Total: " + m.total_voltage + "</span><span>Max: " + m.highest_voltage + "</span><span>Min: " + m.lowest_voltage + "</span><span>Avg: " + m.average_voltage + "</span>"
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